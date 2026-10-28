#!/usr/bin/env python3
"""Read-only teleport target audit for Project Phoenix.

Bu arac DB ve SMP kaynaklarini degistirmez. Amaci server tarafindaki
teleport target contract'ini data seviyesinde dogrulamak ve bozuk kayitlari
raporlamaktir.
"""

from __future__ import annotations

import argparse
import json
import re
import shlex
import struct
import subprocess
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Tuple


PORTAL_NORMAL = 0
PORTAL_SLAYER = 1
PORTAL_VAMPIRE = 2
PORTAL_MULTI_TARGET = 3
PORTAL_PRIVATE = 4
PORTAL_GUILD = 5
PORTAL_BATTLE = 6
PORTAL_OUSTERS = 7

SINGLE_TARGET_PORTALS = {
    PORTAL_NORMAL,
    PORTAL_SLAYER,
    PORTAL_VAMPIRE,
    PORTAL_GUILD,
    PORTAL_BATTLE,
    PORTAL_OUSTERS,
}

PORTAL_NAMES = {
    PORTAL_NORMAL: "NORMAL",
    PORTAL_SLAYER: "SLAYER",
    PORTAL_VAMPIRE: "VAMPIRE",
    PORTAL_MULTI_TARGET: "MULTI_TARGET",
    PORTAL_PRIVATE: "PRIVATE",
    PORTAL_GUILD: "GUILD",
    PORTAL_BATTLE: "BATTLE",
    PORTAL_OUSTERS: "OUSTERS",
}

FLAG_BLOCK_WALKING = 0x02
FLAG_PORTAL = 0x80


@dataclass
class ZoneInfoRow:
    zone_id: int
    ename: str
    zone_type: str
    available: int
    smp_filename: str


@dataclass
class TeleportTarget:
    source: str
    source_table: str
    source_id: str
    source_zone_id: Optional[int]
    source_x: Optional[int]
    source_y: Optional[int]
    target_zone_id: Optional[int]
    target_x: Optional[int]
    target_y: Optional[int]
    portal_type: str
    raw: str


@dataclass
class AuditIssue:
    severity: str
    code: str
    source: str
    message: str
    target_zone_id: Optional[int] = None
    target_x: Optional[int] = None
    target_y: Optional[int] = None


@dataclass
class SmpPortal:
    source_zone_id: int
    source_file: str
    source_x: int
    source_y: int
    portal_type: int
    target_zone_id: int
    target_x: int
    target_y: int
    target_index: int


@dataclass
class SmpInfo:
    path: str
    zone_id: int
    width: int
    height: int
    flags: bytes
    portals: List[SmpPortal]
    issues: List[AuditIssue]
    case_mismatch: bool = False
    requested_name: str = ""
    actual_name: str = ""


def eprint(message: str) -> None:
    print(message, file=sys.stderr)


def mysql_unescape(value: str) -> str:
    if value == r"\N":
        return ""

    sentinel = "\x01"
    value = value.replace(r"\\", sentinel)
    value = value.replace(r"\n", "\n")
    value = value.replace(r"\t", "\t")
    value = value.replace(r"\r", "\r")
    value = value.replace(r"\0", "\x00")
    value = value.replace(r"\Z", "\x1a")
    return value.replace(sentinel, "\\")


def run_mysql(mysql_command: str, sql: str, timeout: int) -> List[Dict[str, str]]:
    cmd = shlex.split(mysql_command)
    cmd.extend(["-B", "--default-character-set=utf8", "-e", sql])
    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=timeout,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "mysql command failed")

    lines = result.stdout.splitlines()
    if not lines:
        return []

    headers = lines[0].split("\t")
    rows: List[Dict[str, str]] = []
    for line in lines[1:]:
        cols = line.split("\t")
        while len(cols) < len(headers):
            cols.append("")
        rows.append({headers[i]: mysql_unescape(cols[i]) for i in range(len(headers))})
    return rows


def fetch_db_snapshot(mysql_command: str, timeout: int) -> Dict[str, List[Dict[str, str]]]:
    return {
        "zone_info": run_mysql(
            mysql_command,
            (
                "SELECT ZoneID, EName, Type, Available, SmpFileName AS SMPFilename "
                "FROM ZoneInfo ORDER BY ZoneID"
            ),
            timeout,
        ),
        "triggers": run_mysql(
            mysql_command,
            (
                "SELECT TriggerID, TriggerType, NPC, NPCNames, QuestID, "
                "HEX(Actions) AS ActionsHex "
                "FROM Triggers "
                "WHERE Actions LIKE '%ActionType%ActivatePortal%' "
                "ORDER BY TriggerID"
            ),
            timeout,
        ),
        "zone_triggers": run_mysql(
            mysql_command,
            (
                "SELECT ZoneID, X1, Y1, X2, Y2, "
                "HEX(Actions) AS ActionsHex, HEX(CounterActions) AS CounterActionsHex "
                "FROM ZoneTriggers "
                "WHERE Actions LIKE '%ActionType%ActivatePortal%' "
                "   OR CounterActions LIKE '%ActionType%ActivatePortal%' "
                "ORDER BY ZoneID, X1, Y1, X2, Y2"
            ),
            timeout,
        ),
    }


def load_snapshot(path: Path) -> Dict[str, List[Dict[str, str]]]:
    return json.loads(path.read_text(encoding="utf-8"))


def save_snapshot(path: Path, snapshot: Dict[str, List[Dict[str, str]]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(snapshot, indent=2, ensure_ascii=False), encoding="utf-8")


def parse_int(value: object) -> Optional[int]:
    try:
        if value is None:
            return None
        text = str(value).strip()
        if not text:
            return None
        return int(text)
    except ValueError:
        return None


def decode_hex_text(value: object) -> str:
    if value is None:
        return ""
    text = str(value).strip()
    if not text:
        return ""
    try:
        return bytes.fromhex(text).decode("utf-8", errors="replace")
    except ValueError:
        return text


ACTION_HEADER_RE = re.compile(r"(?mi)^[ \t]*ActionType\s*:\s*(\S+)")
PARAM_RE = re.compile(r"(?m)^[ \t]*([A-Za-z_]\w*)\s*:\s*([^\n]+?)\s*$")


def split_action_blocks(text: str) -> Iterable[Tuple[str, str]]:
    if not text:
        return []

    matches = list(ACTION_HEADER_RE.finditer(text))
    blocks: List[Tuple[str, str]] = []
    for index, match in enumerate(matches):
        end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
        blocks.append((match.group(1).strip(), text[match.start():end]))
    return blocks


def parse_params(block: str) -> Dict[str, str]:
    params: Dict[str, str] = {}
    for match in PARAM_RE.finditer(block):
        key = match.group(1).strip()
        if key == "ActionType":
            continue
        params[key] = match.group(2).strip()
    return params


def extract_activate_portals(
    raw_actions: str,
    source_table: str,
    source_id: str,
    source_zone_id: Optional[int] = None,
    source_x: Optional[int] = None,
    source_y: Optional[int] = None,
) -> List[TeleportTarget]:
    targets: List[TeleportTarget] = []
    for action_type, block in split_action_blocks(raw_actions):
        if action_type != "ActivatePortal":
            continue
        params = parse_params(block)
        targets.append(
            TeleportTarget(
                source=f"{source_table}:{source_id}",
                source_table=source_table,
                source_id=source_id,
                source_zone_id=source_zone_id,
                source_x=source_x,
                source_y=source_y,
                target_zone_id=parse_int(params.get("ZoneID")),
                target_x=parse_int(params.get("X")),
                target_y=parse_int(params.get("Y")),
                portal_type="DB_ACTIVATE_PORTAL",
                raw=block.strip(),
            )
        )
    return targets


def build_zone_info(rows: List[Dict[str, str]]) -> Dict[int, ZoneInfoRow]:
    zones: Dict[int, ZoneInfoRow] = {}
    for row in rows:
        zone_id = parse_int(row.get("ZoneID"))
        if zone_id is None:
            continue
        zones[zone_id] = ZoneInfoRow(
            zone_id=zone_id,
            ename=row.get("EName", "") or "",
            zone_type=row.get("Type", "") or "",
            available=parse_int(row.get("Available")) or 0,
            smp_filename=row.get("SMPFilename", "") or row.get("SmpFileName", "") or "",
        )
    return zones


def build_db_targets(snapshot: Dict[str, List[Dict[str, str]]]) -> List[TeleportTarget]:
    targets: List[TeleportTarget] = []

    for row in snapshot.get("triggers", []):
        source_id = f"TriggerID={row.get('TriggerID', '')}"
        actions = decode_hex_text(row.get("ActionsHex")) or row.get("Actions", "") or ""
        targets.extend(
            extract_activate_portals(
                actions,
                "Triggers.Actions",
                source_id,
            )
        )

    for row in snapshot.get("zone_triggers", []):
        zid = parse_int(row.get("ZoneID"))
        x1 = parse_int(row.get("X1"))
        y1 = parse_int(row.get("Y1"))
        x2 = parse_int(row.get("X2"))
        y2 = parse_int(row.get("Y2"))
        source_id = f"ZoneID={zid},Rect=({x1},{y1})-({x2},{y2})"
        actions = decode_hex_text(row.get("ActionsHex")) or row.get("Actions", "") or ""
        counter_actions = (
            decode_hex_text(row.get("CounterActionsHex"))
            or row.get("CounterActions", "")
            or ""
        )
        targets.extend(
            extract_activate_portals(
                actions,
                "ZoneTriggers.Actions",
                source_id,
                source_zone_id=zid,
                source_x=x1,
                source_y=y1,
            )
        )
        targets.extend(
            extract_activate_portals(
                counter_actions,
                "ZoneTriggers.CounterActions",
                source_id,
                source_zone_id=zid,
                source_x=x1,
                source_y=y1,
            )
        )
    return targets


def build_map_index(map_dir: Path) -> Dict[str, Path]:
    index: Dict[str, Path] = {}
    if not map_dir.exists():
        return index
    for path in map_dir.rglob("*.smp"):
        index[path.name.lower()] = path
    return index


def read_byte(data: bytes, pos: int, label: str) -> Tuple[int, int]:
    if pos + 1 > len(data):
        raise ValueError(f"truncated while reading {label} at offset {pos}")
    return data[pos], pos + 1


def read_word(data: bytes, pos: int, label: str) -> Tuple[int, int]:
    if pos + 2 > len(data):
        raise ValueError(f"truncated while reading {label} at offset {pos}")
    return struct.unpack_from("<H", data, pos)[0], pos + 2


def read_dword(data: bytes, pos: int, label: str) -> Tuple[int, int]:
    if pos + 4 > len(data):
        raise ValueError(f"truncated while reading {label} at offset {pos}")
    return struct.unpack_from("<I", data, pos)[0], pos + 4


def read_mstring(data: bytes, pos: int, label: str, max_len: int = 1024 * 1024) -> Tuple[bytes, int]:
    length, pos = read_dword(data, pos, f"{label}.length")
    if length > max_len:
        raise ValueError(f"{label} length too large: {length}")
    if pos + length > len(data):
        raise ValueError(f"truncated while reading {label} payload at offset {pos}")
    return data[pos:pos + length], pos + length


def parse_smp_inline(path: Path, requested_name: str, case_mismatch: bool) -> SmpInfo:
    data = path.read_bytes()
    pos = 0
    issues: List[AuditIssue] = []
    portals: List[SmpPortal] = []

    try:
        _version, pos = read_mstring(data, pos, "version", 128)
        zone_id, pos = read_word(data, pos, "zone_id")
        _zone_group_id, pos = read_word(data, pos, "zone_group_id")
        _zone_name, pos = read_mstring(data, pos, "zone_name", 4096)
        _zone_type, pos = read_byte(data, pos, "zone_type")
        _zone_level, pos = read_byte(data, pos, "zone_level")
        _description, pos = read_mstring(data, pos, "description")
        width, pos = read_word(data, pos, "width")
        height, pos = read_word(data, pos, "height")
    except ValueError as exc:
        return SmpInfo(
            path=str(path),
            zone_id=0,
            width=0,
            height=0,
            flags=b"",
            portals=[],
            issues=[
                AuditIssue(
                    severity="CRITICAL",
                    code="SMP_HEADER_PARSE_FAILED",
                    source=str(path),
                    message=str(exc),
                )
            ],
            case_mismatch=case_mismatch,
            requested_name=requested_name,
            actual_name=path.name,
        )

    flags = bytearray()
    for y in range(height):
        for x in range(width):
            try:
                flag, pos = read_byte(data, pos, f"tile_flag[{x},{y}]")
            except ValueError as exc:
                issues.append(
                    AuditIssue(
                        severity="CRITICAL",
                        code="SMP_TILE_TRUNCATED",
                        source=str(path),
                        message=str(exc),
                    )
                )
                return SmpInfo(
                    path=str(path),
                    zone_id=zone_id,
                    width=width,
                    height=height,
                    flags=bytes(flags),
                    portals=portals,
                    issues=issues,
                    case_mismatch=case_mismatch,
                    requested_name=requested_name,
                    actual_name=path.name,
                )
            flags.append(flag)

            if not (flag & FLAG_PORTAL):
                continue

            try:
                portal_type, pos = read_byte(data, pos, f"portal_type[{x},{y}]")
                if portal_type in SINGLE_TARGET_PORTALS:
                    target_zone_id, pos = read_word(data, pos, "portal.target_zone_id")
                    target_x, pos = read_byte(data, pos, "portal.target_x")
                    target_y, pos = read_byte(data, pos, "portal.target_y")
                    portals.append(
                        SmpPortal(
                            source_zone_id=zone_id,
                            source_file=str(path),
                            source_x=x,
                            source_y=y,
                            portal_type=portal_type,
                            target_zone_id=target_zone_id,
                            target_x=target_x,
                            target_y=target_y,
                            target_index=0,
                        )
                    )
                elif portal_type == PORTAL_MULTI_TARGET:
                    size, pos = read_byte(data, pos, "portal.multi.size")
                    for target_index in range(size):
                        target_zone_id, pos = read_word(
                            data, pos, f"portal.multi[{target_index}].target_zone_id"
                        )
                        target_x, pos = read_byte(
                            data, pos, f"portal.multi[{target_index}].target_x"
                        )
                        target_y, pos = read_byte(
                            data, pos, f"portal.multi[{target_index}].target_y"
                        )
                        portals.append(
                            SmpPortal(
                                source_zone_id=zone_id,
                                source_file=str(path),
                                source_x=x,
                                source_y=y,
                                portal_type=portal_type,
                                target_zone_id=target_zone_id,
                                target_x=target_x,
                                target_y=target_y,
                                target_index=target_index,
                            )
                        )
                    if size == 0:
                        issues.append(
                            AuditIssue(
                                severity="WARNING",
                                code="SMP_MULTI_PORTAL_EMPTY",
                                source=f"{path}:{x},{y}",
                                message="multi portal has zero targets",
                            )
                        )
                else:
                    issues.append(
                        AuditIssue(
                            severity="WARNING",
                            code="SMP_UNSUPPORTED_PORTAL_TYPE",
                            source=f"{path}:{x},{y}",
                            message=(
                                f"unsupported portal type {portal_type}; server loader "
                                "does not read a target for this type"
                            ),
                        )
                    )
            except ValueError as exc:
                issues.append(
                    AuditIssue(
                        severity="CRITICAL",
                        code="SMP_PORTAL_STRUCT_TRUNCATED",
                        source=f"{path}:{x},{y}",
                        message=str(exc),
                    )
                )
                return SmpInfo(
                    path=str(path),
                    zone_id=zone_id,
                    width=width,
                    height=height,
                    flags=bytes(flags),
                    portals=portals,
                    issues=issues,
                    case_mismatch=case_mismatch,
                    requested_name=requested_name,
                    actual_name=path.name,
                )

    if pos < len(data):
        issues.append(
            AuditIssue(
                severity="INFO",
                code="SMP_TRAILING_BYTES",
                source=str(path),
                message=f"{len(data) - pos} trailing byte(s) after server-style tile pass",
            )
        )

    if case_mismatch:
        issues.append(
            AuditIssue(
                severity="WARNING",
                code="SMP_CASE_MISMATCH",
                source=str(path),
                message=(
                    f"ZoneInfo requests {requested_name!r}, actual file is {path.name!r}; "
                    "Linux server lowercases the requested path"
                ),
            )
        )

    return SmpInfo(
        path=str(path),
        zone_id=zone_id,
        width=width,
        height=height,
        flags=bytes(flags),
        portals=portals,
        issues=issues,
        case_mismatch=case_mismatch,
        requested_name=requested_name,
        actual_name=path.name,
    )


def load_smp_for_zone(
    zone: ZoneInfoRow,
    map_dir: Optional[Path],
    map_index: Dict[str, Path],
    cache: Dict[int, Optional[SmpInfo]],
) -> Optional[SmpInfo]:
    if zone.zone_id in cache:
        return cache[zone.zone_id]
    if map_dir is None:
        cache[zone.zone_id] = None
        return None

    requested = zone.smp_filename
    if not requested:
        cache[zone.zone_id] = None
        return None

    exact = map_dir / requested.lower()
    if exact.exists():
        try:
            info = parse_smp_inline(exact, requested, case_mismatch=False)
        except OSError as exc:
            info = SmpInfo(
                path=str(exact),
                zone_id=zone.zone_id,
                width=0,
                height=0,
                flags=b"",
                portals=[],
                issues=[
                    AuditIssue(
                        severity="CRITICAL",
                        code="SMP_READ_FAILED",
                        source=str(exact),
                        message=str(exc),
                    )
                ],
                requested_name=requested,
                actual_name=exact.name,
            )
        cache[zone.zone_id] = info
        return info

    found = map_index.get(requested.lower())
    if found is not None:
        info = parse_smp_inline(found, requested, case_mismatch=True)
        cache[zone.zone_id] = info
        return info

    cache[zone.zone_id] = None
    return None


def flag_at(info: SmpInfo, x: int, y: int) -> Optional[int]:
    if x < 0 or y < 0 or x >= info.width or y >= info.height:
        return None
    idx = y * info.width + x
    if idx >= len(info.flags):
        return None
    return info.flags[idx]


def find_suitable_position(info: SmpInfo, cx: int, cy: int) -> Optional[Tuple[int, int]]:
    # Server ZoneUtil::findSuitablePosition spiral'ini veri seviyesinde taklit eder.
    x = cx
    y = cy
    sx = 1
    sy = 0
    max_count = 1
    count = 1
    check_count = 300

    while check_count > 0:
        if x > 0 and y > 0 and x < info.width and y < info.height:
            flag = flag_at(info, x, y)
            if flag is not None:
                walking_blocked = (flag & FLAG_BLOCK_WALKING) != 0
                has_portal = (flag & FLAG_PORTAL) != 0
                if not walking_blocked and not has_portal:
                    return x, y

        x += sx
        y += sy
        count -= 1
        if count == 0:
            if sx == 0:
                max_count += 1
            temp = sx
            sx = -sy
            sy = temp
            count = max_count
        check_count -= 1

    return None


def validate_target(
    target: TeleportTarget,
    zones: Dict[int, ZoneInfoRow],
    map_dir: Optional[Path],
    map_index: Dict[str, Path],
    smp_cache: Dict[int, Optional[SmpInfo]],
) -> List[AuditIssue]:
    issues: List[AuditIssue] = []

    if target.target_zone_id is None or target.target_x is None or target.target_y is None:
        issues.append(
            AuditIssue(
                severity="CRITICAL",
                code="TARGET_PARAM_MISSING",
                source=target.source,
                message="ActivatePortal target must include ZoneID, X and Y",
                target_zone_id=target.target_zone_id,
                target_x=target.target_x,
                target_y=target.target_y,
            )
        )
        return issues

    zone = zones.get(target.target_zone_id)
    if zone is None:
        issues.append(
            AuditIssue(
                severity="CRITICAL",
                code="TARGET_ZONE_MISSING",
                source=target.source,
                message=f"target ZoneID {target.target_zone_id} is not present in ZoneInfo",
                target_zone_id=target.target_zone_id,
                target_x=target.target_x,
                target_y=target.target_y,
            )
        )
        return issues

    if zone.available == 0:
        issues.append(
            AuditIssue(
                severity="WARNING",
                code="TARGET_ZONE_UNAVAILABLE",
                source=target.source,
                message=f"target ZoneID {zone.zone_id} ({zone.ename}) has Available=0",
                target_zone_id=target.target_zone_id,
                target_x=target.target_x,
                target_y=target.target_y,
            )
        )

    info = load_smp_for_zone(zone, map_dir, map_index, smp_cache)
    if map_dir is not None and info is None:
        issues.append(
            AuditIssue(
                severity="CRITICAL",
                code="TARGET_SMP_MISSING",
                source=target.source,
                message=f"target ZoneID {zone.zone_id} SMP file not found: {zone.smp_filename!r}",
                target_zone_id=target.target_zone_id,
                target_x=target.target_x,
                target_y=target.target_y,
            )
        )
        return issues

    if info is None:
        return issues

    if info.width <= 0 or info.height <= 0:
        issues.append(
            AuditIssue(
                severity="CRITICAL",
                code="TARGET_SMP_INVALID",
                source=target.source,
                message=f"target ZoneID {zone.zone_id} SMP could not be parsed",
                target_zone_id=target.target_zone_id,
                target_x=target.target_x,
                target_y=target.target_y,
            )
        )
        return issues

    if target.target_x < 0 or target.target_y < 0 or target.target_x >= info.width or target.target_y >= info.height:
        issues.append(
            AuditIssue(
                severity="CRITICAL",
                code="TARGET_COORD_OUT_OF_BOUNDS",
                source=target.source,
                message=(
                    f"target ({target.target_x},{target.target_y}) is outside "
                    f"ZoneID {zone.zone_id} SMP bounds {info.width}x{info.height}"
                ),
                target_zone_id=target.target_zone_id,
                target_x=target.target_x,
                target_y=target.target_y,
            )
        )
        return issues

    exact_flag = flag_at(info, target.target_x, target.target_y)
    if exact_flag is None:
        issues.append(
            AuditIssue(
                severity="CRITICAL",
                code="TARGET_TILE_UNREADABLE",
                source=target.source,
                message="target tile flag could not be read from SMP",
                target_zone_id=target.target_zone_id,
                target_x=target.target_x,
                target_y=target.target_y,
            )
        )
        return issues

    landing = find_suitable_position(info, target.target_x, target.target_y)
    if landing is None:
        issues.append(
            AuditIssue(
                severity="CRITICAL",
                code="TARGET_NO_SUITABLE_LANDING",
                source=target.source,
                message=(
                    f"no non-blocked, non-portal walking tile within server "
                    f"findSuitablePosition search from ({target.target_x},{target.target_y})"
                ),
                target_zone_id=target.target_zone_id,
                target_x=target.target_x,
                target_y=target.target_y,
            )
        )
    else:
        if (exact_flag & FLAG_BLOCK_WALKING) != 0:
            issues.append(
                AuditIssue(
                    severity="WARNING",
                    code="TARGET_EXACT_TILE_WALK_BLOCKED",
                    source=target.source,
                    message=(
                        f"exact target tile is walking-blocked; runtime will land at "
                        f"nearest suitable tile {landing}"
                    ),
                    target_zone_id=target.target_zone_id,
                    target_x=target.target_x,
                    target_y=target.target_y,
                )
            )
        if (exact_flag & FLAG_PORTAL) != 0:
            issues.append(
                AuditIssue(
                    severity="WARNING",
                    code="TARGET_EXACT_TILE_IS_PORTAL",
                    source=target.source,
                    message=(
                        f"exact target tile is also a portal; runtime will land at "
                        f"nearest suitable tile {landing}"
                    ),
                    target_zone_id=target.target_zone_id,
                    target_x=target.target_x,
                    target_y=target.target_y,
                )
            )

    return issues


def validate_zone_trigger_rectangles(
    snapshot: Dict[str, List[Dict[str, str]]],
    zones: Dict[int, ZoneInfoRow],
    map_dir: Optional[Path],
    map_index: Dict[str, Path],
    smp_cache: Dict[int, Optional[SmpInfo]],
) -> List[AuditIssue]:
    issues: List[AuditIssue] = []
    for row in snapshot.get("zone_triggers", []):
        zone_id = parse_int(row.get("ZoneID"))
        x1 = parse_int(row.get("X1"))
        y1 = parse_int(row.get("Y1"))
        x2 = parse_int(row.get("X2"))
        y2 = parse_int(row.get("Y2"))
        source = f"ZoneTriggers:ZoneID={zone_id},Rect=({x1},{y1})-({x2},{y2})"

        if zone_id is None or x1 is None or y1 is None or x2 is None or y2 is None:
            issues.append(
                AuditIssue(
                    severity="CRITICAL",
                    code="SOURCE_RECT_PARAM_MISSING",
                    source=source,
                    message="ZoneTriggers source rectangle has missing numeric fields",
                )
            )
            continue

        if x1 > x2 or y1 > y2:
            issues.append(
                AuditIssue(
                    severity="CRITICAL",
                    code="SOURCE_RECT_REVERSED",
                    source=source,
                    message="ZoneTriggers rectangle has reversed bounds",
                )
            )
            continue

        zone = zones.get(zone_id)
        if zone is None:
            issues.append(
                AuditIssue(
                    severity="CRITICAL",
                    code="SOURCE_ZONE_MISSING",
                    source=source,
                    message=f"source ZoneID {zone_id} is not present in ZoneInfo",
                )
            )
            continue

        info = load_smp_for_zone(zone, map_dir, map_index, smp_cache)
        if map_dir is not None and info is None:
            issues.append(
                AuditIssue(
                    severity="CRITICAL",
                    code="SOURCE_SMP_MISSING",
                    source=source,
                    message=f"source ZoneID {zone.zone_id} SMP file not found: {zone.smp_filename!r}",
                )
            )
            continue
        if info is None or info.width <= 0 or info.height <= 0:
            continue

        if x1 < 0 or y1 < 0 or x2 >= info.width or y2 >= info.height:
            issues.append(
                AuditIssue(
                    severity="CRITICAL",
                    code="SOURCE_RECT_OUT_OF_BOUNDS",
                    source=source,
                    message=(
                        f"source rectangle is outside ZoneID {zone.zone_id} "
                        f"SMP bounds {info.width}x{info.height}"
                    ),
                )
            )
    return issues


def smp_portal_to_target(portal: SmpPortal) -> TeleportTarget:
    ptype = PORTAL_NAMES.get(portal.portal_type, str(portal.portal_type))
    return TeleportTarget(
        source=f"SMP:{Path(portal.source_file).name}:{portal.source_x},{portal.source_y}#{portal.target_index}",
        source_table="SMP.inline_portal",
        source_id=f"{Path(portal.source_file).name}:{portal.source_x},{portal.source_y}#{portal.target_index}",
        source_zone_id=portal.source_zone_id,
        source_x=portal.source_x,
        source_y=portal.source_y,
        target_zone_id=portal.target_zone_id,
        target_x=portal.target_x,
        target_y=portal.target_y,
        portal_type=f"SMP_{ptype}",
        raw="",
    )


def run_audit(
    snapshot: Dict[str, List[Dict[str, str]]],
    map_dir: Optional[Path],
    include_smp_portals: bool,
) -> Dict[str, object]:
    zones = build_zone_info(snapshot.get("zone_info", []))
    db_targets = build_db_targets(snapshot)
    map_index = build_map_index(map_dir) if map_dir is not None else {}
    smp_cache: Dict[int, Optional[SmpInfo]] = {}

    issues: List[AuditIssue] = []
    issues.extend(
        validate_zone_trigger_rectangles(snapshot, zones, map_dir, map_index, smp_cache)
    )

    targets: List[TeleportTarget] = list(db_targets)

    # Map parser issue'lari once target zone load'lariyla ortaya cikar. SMP
    # portallarini audit etmek icin tum ZoneInfo SMP'lerini de okuyabiliriz.
    if include_smp_portals and map_dir is not None:
        for zone in zones.values():
            info = load_smp_for_zone(zone, map_dir, map_index, smp_cache)
            if info is None:
                continue
            for portal in info.portals:
                targets.append(smp_portal_to_target(portal))

    for target in targets:
        issues.extend(validate_target(target, zones, map_dir, map_index, smp_cache))

    seen_smp_issue_keys = set()
    for info in smp_cache.values():
        if info is None:
            continue
        for issue in info.issues:
            key = (issue.severity, issue.code, issue.source, issue.message)
            if key in seen_smp_issue_keys:
                continue
            seen_smp_issue_keys.add(key)
            issues.append(issue)

    counts: Dict[str, int] = {"CRITICAL": 0, "WARNING": 0, "INFO": 0}
    for issue in issues:
        counts[issue.severity] = counts.get(issue.severity, 0) + 1

    return {
        "summary": {
            "zone_info_rows": len(zones),
            "db_activate_portal_targets": len(db_targets),
            "total_targets_audited": len(targets),
            "smp_files_loaded": sum(1 for item in smp_cache.values() if item is not None),
            "issues": counts,
        },
        "issues": [asdict(issue) for issue in issues],
        "targets": [asdict(target) for target in targets],
    }


def render_markdown(report: Dict[str, object], max_issues: int) -> str:
    summary = report["summary"]
    issues = report["issues"]
    lines = [
        "# Teleport Data Audit",
        "",
        "Read-only audit. No DB or SMP data was modified.",
        "",
        "## Summary",
        "",
        f"- ZoneInfo rows: {summary['zone_info_rows']}",
        f"- DB ActivatePortal targets: {summary['db_activate_portal_targets']}",
        f"- Total targets audited: {summary['total_targets_audited']}",
        f"- SMP files loaded: {summary['smp_files_loaded']}",
        f"- Critical issues: {summary['issues'].get('CRITICAL', 0)}",
        f"- Warnings: {summary['issues'].get('WARNING', 0)}",
        f"- Info: {summary['issues'].get('INFO', 0)}",
        "",
    ]

    if not issues:
        lines.extend(["## Issues", "", "No issues found."])
        return "\n".join(lines) + "\n"

    grouped: Dict[Tuple[object, ...], Dict[str, object]] = {}
    for issue in issues:
        key = (
            issue["severity"],
            issue["code"],
            issue.get("target_zone_id"),
            issue.get("target_x"),
            issue.get("target_y"),
            issue["message"],
        )
        if key not in grouped:
            grouped[key] = {"issue": issue, "count": 0, "sources": []}
        grouped[key]["count"] = int(grouped[key]["count"]) + 1
        sources = grouped[key]["sources"]
        if len(sources) < 5:
            sources.append(issue["source"])

    by_severity: Dict[str, List[Dict[str, object]]] = {
        "CRITICAL": [],
        "WARNING": [],
        "INFO": [],
    }
    for group in grouped.values():
        issue = group["issue"]
        by_severity.setdefault(issue["severity"], []).append(group)

    lines.extend(
        [
            f"- Issue groups: {len(grouped)}",
            "",
        ]
    )

    for severity in ("CRITICAL", "WARNING", "INFO"):
        bucket = sorted(
            by_severity.get(severity, []),
            key=lambda item: (-int(item["count"]), str(item["issue"]["code"])),
        )
        if not bucket:
            continue
        lines.extend([f"## {severity}", ""])
        for group in bucket[:max_issues]:
            issue = group["issue"]
            sources = ", ".join(f"`{s}`" for s in group["sources"])
            count = int(group["count"])
            target = ""
            if issue.get("target_zone_id") is not None:
                target = (
                    f" target=ZoneID {issue.get('target_zone_id')} "
                    f"({issue.get('target_x')},{issue.get('target_y')})"
                )
            lines.append(
                f"- `{issue['code']}` count={count}{target}: {issue['message']}"
            )
            lines.append(f"  - examples: {sources}")
        if len(bucket) > max_issues:
            lines.append(
                f"- ... {len(bucket) - max_issues} more {severity.lower()} issue group(s)"
            )
        lines.append("")

    return "\n".join(lines)


def main(argv: Optional[List[str]] = None) -> int:
    parser = argparse.ArgumentParser(
        description="Read-only DB/SMP teleport target audit for Project Phoenix."
    )
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("--mysql-command", help="Command prefix, e.g. 'sudo mysql DARKEDEN'")
    source.add_argument("--db-json", type=Path, help="Previously exported DB snapshot JSON")
    parser.add_argument("--write-db-json", type=Path, help="Write fetched DB snapshot JSON")
    parser.add_argument("--map-dir", type=Path, help="Server data/MAP directory with .smp files")
    parser.add_argument(
        "--skip-smp-portals",
        action="store_true",
        help="Only audit DB ActivatePortal rows; do not scan inline SMP portals.",
    )
    parser.add_argument("--report-md", type=Path, help="Write Markdown report")
    parser.add_argument("--report-json", type=Path, help="Write JSON report")
    parser.add_argument("--max-issues", type=int, default=200)
    parser.add_argument("--timeout", type=int, default=60)
    parser.add_argument(
        "--fail-on",
        choices=("never", "critical", "warning"),
        default="never",
        help="Process exit policy. Default never keeps audit non-destructive.",
    )
    args = parser.parse_args(argv)

    try:
        if args.db_json is not None:
            snapshot = load_snapshot(args.db_json)
        else:
            snapshot = fetch_db_snapshot(args.mysql_command, args.timeout)
            if args.write_db_json is not None:
                save_snapshot(args.write_db_json, snapshot)

        report = run_audit(
            snapshot=snapshot,
            map_dir=args.map_dir,
            include_smp_portals=not args.skip_smp_portals,
        )

        if args.report_json is not None:
            args.report_json.parent.mkdir(parents=True, exist_ok=True)
            args.report_json.write_text(
                json.dumps(report, indent=2, ensure_ascii=False),
                encoding="utf-8",
            )

        markdown = render_markdown(report, args.max_issues)
        if args.report_md is not None:
            args.report_md.parent.mkdir(parents=True, exist_ok=True)
            args.report_md.write_text(markdown, encoding="utf-8")
        else:
            print(markdown)

        issue_counts = report["summary"]["issues"]
        critical = issue_counts.get("CRITICAL", 0)
        warning = issue_counts.get("WARNING", 0)
        if args.fail_on == "critical" and critical:
            return 2
        if args.fail_on == "warning" and (critical or warning):
            return 2
        return 0
    except Exception as exc:
        eprint(f"ERROR: {exc}")
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
