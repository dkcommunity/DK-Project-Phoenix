#!/bin/bash
# start-servers.sh — DarkEden Phoenix server lifecycle
# ------------------------------------------------------------------
# Priority: In EVERY COMMAND, first FORCE-kill the necessary processes,
# then restart them. This eliminates the risk of stale connections, orphans PIDs, and half-dead states.
# Use the SIGTERM (graceful 5s) -> SIGKILL (force) sequence.
#
# Usage:
#   start-servers.sh                    # all (kill 3 + start 3) + wait for ready + return
#   start-servers.sh all
#   start-servers.sh shared             # kill + start sharedserver only
#   start-servers.sh login
#   start-servers.sh game
#   start-servers.sh kill               # Kill 3 servers, restart
#   start-servers.sh status             # just list pgrep
#
# Optional flags (post-command):
#   --tail        Live log stream after `Ready` using `tail -f` (exit with Ctrl-C)
#   --no-wait     Spawn and return immediately (without waiting for `Ready`); init continues in parallel
#   --as-root     Skip auto-switch; spawn as root user (advanced)
#   --short       Sequential startup: kill all, start shared›login›game, tail each log
#                 until ready, then move to next automatically (no user input)
#   --help|-h     Display this help text
#
# Default behavior: spawn with nohup + wait up to 4 minutes for "ready" + return.
# Thanks to nohup, Ctrl-C or an SSH disconnect won't affect the servers.
#
# USER MANAGEMENT:
# The script can run as the root or phoenix user. By default, it
# spawns under the phoenix user (production-correct, log ownership
# consistency). If started as root, the script automatically switches to the phoenix user
# (runuser > sudo > su fallback) and re-executes itself.
# The --as-root flag overrides this behavior (spawn as root).
# ------------------------------------------------------------------

# User/Permission Management
TARGET_USER="phoenix"
SKIP_USER_SWITCH=0
for _arg in "$@"; do
    [[ "$_arg" == "--as-root" ]] && SKIP_USER_SWITCH=1
done

if [[ $SKIP_USER_SWITCH -eq 0 && "$(id -un)" == "root" ]]; then
    if id "$TARGET_USER" &>/dev/null; then
        echo "[info] A root user has been detected; '${TARGET_USER}' user'a switch + re-exec"
        if command -v runuser &>/dev/null; then
            exec runuser -u "$TARGET_USER" -- "$0" "$@"
        elif command -v sudo &>/dev/null; then
            exec sudo -u "$TARGET_USER" -- "$0" "$@"
        else
            # legacy su fallback (sub-shell + arg preservation)
            _args_quoted=$(printf ' %q' "$@")
            exec su -l "$TARGET_USER" -c "${0}${_args_quoted}"
        fi
        echo "[err] The user switch failed (runuser/sudo/su not available)"
        exit 3
    else
        echo "[err] target user '${TARGET_USER}' could not be found in the system — --as-root flag'i ile devam edebilirsiniz"
        exit 3
    fi
fi
# From here on: we are running as the phoenix user (or as root using --as-root)

BIN="/home/phoenix/phoenix/ServerSRC/bin"
CONF="/home/phoenix/phoenix/ServerSRC/conf"

# ============================================================================
# Core dump + heap corruption tespit ayarlari (memory: reference_gdb_core_workflow)
# ============================================================================
# Crash teshisi icin gereken env setup:
# - ulimit -c unlimited: SIGSEGV/SIGABRT/SIGBUS'ta core dump uretilir
# - MALLOC_CHECK_=2: heap corruption tespit edilince abort (early-detect)
# - LIBC_FATAL_STDERR_=1: glibc fortify_fail mesajlari stderr/log'a yazilir
# - kernel.core_pattern: persistent (/etc/sysctl.d/99-coredump.conf) -
#   /home/phoenix/cores/core.%e.%p (VM admin tarafindan kurulu)
#
# Bu env'ler nohup ile spawn edilen server process'lerine inherit olur.
# ============================================================================

# Cores dizinini garanti et (kernel.core_pattern target'i)
mkdir -p "$HOME/cores" 2>/dev/null

# Core dump unlimited (ulimit -c degerinde sinirsiz)
ulimit -c unlimited 2>/dev/null

# MALLOC_CHECK_=2: glibc heap corruption tespiti, abort on detect
export MALLOC_CHECK_=2

# LIBC_FATAL_STDERR_=1: *** buffer overflow detected *** ve benzeri fortify_fail
# mesajlari stderr'e yazilir, server log'una dusurmek icin
export LIBC_FATAL_STDERR_=1

# Surec'in iyice oldugune emin olmak icin bekleme suresi (saniye)
GRACEFUL_WAIT=5
POST_KILL_SETTLE=1

# Tek bir server'i her sekilde sonlandir.
# 1. SIGTERM gonder, 5sn'ye kadar graceful exit bekle
# 2. Hala yasiyorsa SIGKILL ile zorla sonlandir
# 3. Final dogrulama
kill_one() {
    local name="$1"

    if ! pgrep -x "$name" > /dev/null; then
        echo "[kill] $name : already DOWN"
        return 0
    fi

    echo "[kill] $name : sending SIGTERM..."
    pkill -TERM -x "$name" 2>/dev/null

    local i
    for ((i=1; i<=GRACEFUL_WAIT; i++)); do
        sleep 1
        if ! pgrep -x "$name" > /dev/null; then
            echo "[kill] $name : graceful exit after ${i}s"
            sleep "$POST_KILL_SETTLE"
            return 0
        fi
    done

    echo "[kill] $name : SIGTERM timeout, sending SIGKILL..."
    pkill -KILL -x "$name" 2>/dev/null
    sleep "$POST_KILL_SETTLE"

    if pgrep -x "$name" > /dev/null; then
        echo "[kill] $name : !!! STILL ALIVE after SIGKILL — manual intervention needed"
        pgrep -a -x "$name"
        return 1
    fi
    echo "[kill] $name : force-killed"
    return 0
}

# Surec ozel siralama: bagimliliklar yukaridan asagi.
# gameserver -> loginserver -> sharedserver.
# Kill sirasi tersi gerekmiyor ama mantiken once gameserver gitmeli ki
# loginserver hala calismakta oldugunu fark eden client'lar disconnect olsun.
kill_all() {
    echo "==========================================="
    echo "  Killing all 3 servers (force-kill ladder)"
    echo "==========================================="
    kill_one gameserver
    kill_one loginserver
    kill_one sharedserver
    echo "==========================================="
    echo "  Kill phase complete"
    echo "==========================================="
}

start_one() {
    local name="$1"
    local log="/tmp/${name}.log"
    cd "$BIN" || { echo "[start] cd $BIN FAILED"; return 1; }
    nohup stdbuf -oL "$BIN/$name" -f "$CONF/$name.conf" > "$log" 2>&1 &
    local pid=$!
    echo "[start] $name : PID=$pid  log=$log"
}

status() {
    echo "--- pgrep status:"
    pgrep -a -x 'sharedserver|loginserver|gameserver' || echo "  (none running)"
}

# Ready detection function: the port of whichever server is started is set to LISTEN
# Wait for it to finish. For the game server, also look for the "init complete" marker.
# (smsservice2222... = sharedserver bridge ready; PaySystem init outputs).
# Waits for up to 8 minutes (Phoenix full initialization: 5–7 minutes: 270+ zone + 30+ manager).
wait_for_ready() {
    # A game server's "full init" typically takes 60–180 seconds in the real world.
    # This is just a timeout—if the server is fast, it returns early.
    local max_seconds=240
    local elapsed=0
    local progress_every=20
    local last_progress=0
    local last_zone_count=0
    local stuck_threshold=60   # If the zone count doesn't increase during the 60s, a "stuck" warning
    local stuck_since=0
    local ports_to_check=""
    local check_game_init=0

    case "$CMD" in
        shared) ports_to_check="9977" ;;
        login)  ports_to_check="9999" ;;
        game)   ports_to_check="9998"; check_game_init=1 ;;
        all)    ports_to_check="9977 9999 9998"; check_game_init=1 ;;
    esac

    echo
    echo "--- Ready is expected (max ${max_seconds}s, port'lar: ${ports_to_check})"
    echo "    Cancel: Ctrl-C (servers will continue running in the background with nohup)"

    while [[ $elapsed -lt $max_seconds ]]; do
        local all_listen=1
        for port in $ports_to_check; do
            if ! ss -tln 2>/dev/null | awk '{print $4}' | grep -q ":${port}\$"; then
                all_listen=0
                break
            fi
        done

        if [[ $all_listen -eq 1 ]]; then
            if [[ $check_game_init -eq 1 ]]; then
                # gameserver tam init isareti: smsservice bridge or PaySystem trace
                if grep -q "smsservice2222\|PaySystem::\|GlobalNPCManager.*Success\|ObjectManager::init.*Complete" /tmp/gameserver.log 2>/dev/null; then
                    echo "[ready] all port's LISTEN + gameserver init complete (${elapsed}s)"
                    return 0
                fi
            else
                echo "[ready] all port's LISTEN (${elapsed}s)"
                return 0
            fi
        fi

        # Progress feedback her 20s'de bir: zone init count, son log satiri
        if (( elapsed - last_progress >= progress_every )) && (( elapsed > 0 )); then
            local zone_count=0
            if [[ -f /tmp/gameserver.log ]]; then
                zone_count=$(grep -c 'ZONE INITIALIZATION SUCCESS' /tmp/gameserver.log 2>/dev/null || echo 0)
            fi
            local listen_state="LISTEN"
            local listen_count=0
            for port in $ports_to_check; do
                if ss -tln 2>/dev/null | awk '{print $4}' | grep -q ":${port}\$"; then
                    listen_count=$((listen_count + 1))
                fi
            done
            echo "[${elapsed}s] port LISTEN=${listen_count}/$(echo $ports_to_check | wc -w) | zone init=${zone_count}"

            # Stuck detection: warning if the zone count isn't increasings
            if [[ $check_game_init -eq 1 ]] && [[ $zone_count -eq $last_zone_count ]]; then
                if [[ $stuck_since -eq 0 ]]; then
                    stuck_since=$elapsed
                elif (( elapsed - stuck_since >= stuck_threshold )); then
                    echo "    WARNING: zone init ${stuck_threshold}s+ ilerlemiyor (count=${zone_count}). Son log:"
                    tail -3 /tmp/gameserver.log 2>/dev/null | sed 's/^/      /'
                fi
            else
                stuck_since=0
                last_zone_count=$zone_count
            fi

            last_progress=$elapsed
        fi

        sleep 3
        elapsed=$((elapsed + 3))
    done

    echo "[timeout] Waited ${max_seconds} seconds for it to be ready; it hasn't fully booted up yet"
    echo "    pgrep:"
    pgrep -a -x 'sharedserver|loginserver|gameserver' 2>/dev/null | sed 's/^/      /'
    echo "    son 10 satir /tmp/gameserver.log:"
    tail -10 /tmp/gameserver.log 2>/dev/null | sed 's/^/      /'
    echo "    Server'lar nohup'la calismakta — manuel takip icin: tail -f /tmp/gameserver.log"
    return 1
}

# ------------------------------------------------------------------
# --short interactive mode: kill all, start each, tail until ready (auto)
# ------------------------------------------------------------------
run_short() {
    kill_all

    start_one sharedserver
    echo "=== Tailing /tmp/sharedserver.log until ready (port 9977) ==="
    tail -f /tmp/sharedserver.log &
    TAIL_PID=$!
    CMD="shared"
    wait_for_ready
    kill $TAIL_PID 2>/dev/null
    wait $TAIL_PID 2>/dev/null

    start_one loginserver
    echo "=== Tailing /tmp/loginserver.log until ready (port 9999) ==="
    tail -f /tmp/loginserver.log &
    TAIL_PID=$!
    CMD="login"
    wait_for_ready
    kill $TAIL_PID 2>/dev/null
    wait $TAIL_PID 2>/dev/null

    start_one gameserver
    echo "=== Tailing /tmp/gameserver.log until ready (port 9998 + init) ==="
    tail -f /tmp/gameserver.log &
    TAIL_PID=$!
    CMD="game"
    wait_for_ready
    kill $TAIL_PID 2>/dev/null
    wait $TAIL_PID 2>/dev/null

    echo
    status
    echo "All servers started successfully. Now tailing all logs (Ctrl-C to exit)..."
    exec tail -f /tmp/sharedserver.log /tmp/loginserver.log /tmp/gameserver.log
}

# Argument parser: The first positional argument, followed by the remaining --flags
CMD="all"
DO_TAIL=0
DO_WAIT=1
DO_SHORT=0
SAW_CMD=0
for arg in "$@"; do
    case "$arg" in
        --tail)    DO_TAIL=1 ;;
        --no-wait) DO_WAIT=0 ;;
        --short)   DO_SHORT=1 ;;
        --as-root) ;;   # Processed in user switch; noop here
        --help|-h)
            sed -n '2,35p' "$0" | sed 's/^# \{0,1\}//'
            exit 0
            ;;
        shared|login|game|all|kill|status)
            if [[ $SAW_CMD -eq 0 ]]; then
                CMD="$arg"
                SAW_CMD=1
            else
                echo "[err] More than one command was specified: '$CMD' ve '$arg'"
                exit 2
            fi
            ;;
        *)
            echo "[err] unknown arg: '$arg'"
            echo "       usage: $0 {shared|login|game|all|kill|status} [--tail] [--no-wait] [--as-root] [--short] [--help]"
            exit 2
            ;;
    esac
done

# If --short is given, run interactive mode and exit
if [[ $DO_SHORT -eq 1 ]]; then
    run_short
fi

# Normal execution (without --short)
case "$CMD" in
    shared)
        kill_one sharedserver
        start_one sharedserver
        ;;
    login)
        kill_one loginserver
        start_one loginserver
        ;;
    game)
        kill_one gameserver
        start_one gameserver
        ;;
    all)
        kill_all
        start_one sharedserver
        sleep 4
        start_one loginserver
        sleep 4
        start_one gameserver
        ;;
    kill)
        kill_all
        echo
        status
        exit 0
        ;;
    status)
        status
        exit 0
        ;;
esac

sleep 2
echo
status

if [[ $DO_WAIT -eq 1 ]]; then
    wait_for_ready
fi

if [[ $DO_TAIL -eq 1 ]]; then
    echo
    echo "--- tailing logs (--tail flag, Exit with Ctrl-C):"
    tail -f /tmp/loginserver.log /tmp/sharedserver.log /tmp/gameserver.log
fi