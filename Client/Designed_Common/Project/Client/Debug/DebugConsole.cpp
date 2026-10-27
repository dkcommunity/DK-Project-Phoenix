//----------------------------------------------------------------------
// DebugConsole.cpp — ImGui Hibrit Admin Menu (Phoenix Admin Tools)
//----------------------------------------------------------------------
// Tab 1: Item     — ItemClass + ItemType + Count + Limited/Unlimited
// Tab 2: Creature — MonsterType + SpawnCount (1..30, server clamp)
// Tab 3: Zone     — ZoneID + X + Y (0..255)
// Tab 4: Commands — KATEGORI TREE + DINAMIK FORM + FREE-FORM + HISTORY
//                   (40+ komut, 9 kategori, Dev_Log/admin_commands_reference.md kaynak)
//
// Authentication PhoenixImGui::Render() icinde (attrOperator gate).
// Defense-in-depth: Admin Quick icinde de auth mask kontrolu korunur.
//
// 2026-05-18 SERVER PACKET: Phoenix'te dedicated CGAdmin* packet YOK.
// Tum admin islemleri CGSay chat command (`*create N M`, `*summon {ID} N`,
// `*warp Z X Y`). Server CGSayHandler `*` ile baslayan mesajlari admin
// parser'lara yonlendiriyor (opcreate/opsummon/opwarp).
//----------------------------------------------------------------------

#include "PhoenixImGui.h"
#include "imgui.h"

// Phoenix admin chat command send (CGSay packet via g_pSocket).
// 2026-05-18 FIX: g_pSocket aslinda Socket* DEGIL, ClientPlayer* (Phoenix
// garip naming). ClientPlayer Player'dan derive ediyor, Player'da
// virtual void sendPacket(Packet*) var (Player.h). Bu yuzden
// g_pSocket->sendPacket(&CGSay) calisiyor.
#include "../Packet/Cpackets/CGSay.h"
#include "../Packet/ClientPlayer.h"
#include "../ItemClassDef.h"
#include "../MItemTable.h"
#include "../MCreatureTable.h"
#include "../MZoneTable.h"
#include "../UserInformation.h"

#include <ctype.h>
#include <string.h>
#include <string>
#include <vector>

// Phoenix global "socket" — actually ClientPlayer*. Tanim: GameMain.cpp:
//   ClientPlayer *g_pSocket = NULL;
extern ClientPlayer* g_pSocket;

extern "C" bool Phoenix_TextPerf_IsEnabled();
extern "C" void Phoenix_TextPerf_SetEnabled(bool enabled);
extern "C" void Phoenix_TextPerf_FormatOverlay(char* buffer, int bufferSize);

namespace PhoenixImGui {

static bool IsAdminAuthorized()
{
    return (g_pUserInformation != NULL
        && (g_pUserInformation->attrOperator.GetValue() & OPERATORTYPE_AUTHMASK) != 0);
}

//----------------------------------------------------------------------
// Tab state
//----------------------------------------------------------------------

struct ItemTabState {
    int  itemClass      = ITEM_CLASS_RING;
    int  itemType       = 0;
    int  itemCount      = 1;      // non-stackable tekrar sayisi
    int  stackCount     = 1;      // stackable [num]
    bool limitedDuration = false;
    int  durationSeconds = 3600;
    bool useOption1     = false;
    bool useOption2     = false;
    bool useThirdOption1 = false;
    bool useThirdOption2 = false;
    int  option1        = 228;    // Vision +10 ornegi
    int  option2        = 0;
    int  thirdOption1   = 0;
    int  thirdOption2   = 0;
    char classFilter[64] = "";
    char itemFilter[64] = "";
    std::vector<std::string> history;
};

struct CreatureTabState {
    int  creatureType   = 0;
    int  spawnCount     = 1;
    char filter[64]     = "";
    std::vector<std::string> history;
};

struct ZoneTabState {
    int  zoneId         = 1;
    int  x              = 50;
    int  y              = 50;
    char filter[64]     = "";
    std::vector<std::string> history;
};

// Commands tab — kategori tree (sol) + dinamik form (sag) + free-form (alt)
struct CommandsTabState {
    char  commandBuf[256] = "";     // alt panel free-form input
    int   selectedCmdId   = -1;     // sol panel tree'den secili komut index
    char  formBuf[256]    = "";     // sag panel edit input (template'ten kopya)
    std::vector<std::string> history;
};

struct AdminOnlinePlayerRow {
    std::string name;
    std::string race;
    std::string playerClass;
    int level;
    std::string rank;
    std::string map;
    int zoneId;
    int x;
    int y;
};

static ItemTabState     g_item;
static CreatureTabState g_creature;
static ZoneTabState     g_zone;
static CommandsTabState g_commands;
static std::vector<AdminOnlinePlayerRow> g_onlinePlayers;
static bool g_onlinePlayersLoading = false;
static bool g_onlinePlayersEverRequested = false;
static DWORD g_onlinePlayersLastRequestTick = 0;
static DWORD g_onlinePlayersLastUpdateTick = 0;
static std::string g_onlinePlayersStatus = "Not loaded.";

// History buffer cap
static const size_t kHistoryMax = 32;

struct AdminListEntry {
    int id;
    int subId;
    std::string label;
};

static std::vector<AdminListEntry> g_itemClassEntries;
static std::vector<AdminListEntry> g_itemTypeEntries;
static std::vector<AdminListEntry> g_creatureEntries;
static std::vector<AdminListEntry> g_zoneEntries;
static int g_cachedItemTableSize = -1;
static int g_cachedItemTypeClass = -1;
static int g_cachedItemTypeSize = -1;
static int g_cachedCreatureTableSize = -1;
static int g_cachedZoneTableSize = -1;

static void PushHistory(std::vector<std::string>& hist, const std::string& line)
{
    hist.push_back(line);
    if (hist.size() > kHistoryMax)
        hist.erase(hist.begin());
}

static bool HasAlphaNum(const std::string& text)
{
    for (size_t i = 0; i < text.size(); ++i) {
        if (isalnum((unsigned char)text[i]))
            return true;
    }
    return false;
}

static std::string CleanAsciiName(const char* text, const char* fallback)
{
    if (text == NULL || text[0] == '\0')
        return fallback ? fallback : "";

    std::string out;
    bool lastSpace = false;
    for (const unsigned char* p = (const unsigned char*)text; *p != '\0'; ++p) {
        unsigned char c = *p;
        if (c >= 32 && c <= 126) {
            if (c == ' ' || c == '\t') {
                if (!lastSpace && !out.empty()) {
                    out.push_back(' ');
                    lastSpace = true;
                }
            } else {
                out.push_back((char)c);
                lastSpace = false;
            }
        }
    }

    while (!out.empty() && out[out.size() - 1] == ' ')
        out.erase(out.size() - 1);

    if (!HasAlphaNum(out))
        return fallback ? fallback : "";
    return out;
}

static std::string ExtractParenthesizedAscii(const char* text)
{
    if (text == NULL) return "";
    const char* left = strchr(text, '(');
    const char* right = (left != NULL) ? strchr(left + 1, ')') : NULL;
    if (left == NULL || right == NULL || right <= left + 1)
        return "";

    std::string value(left + 1, right);
    value = CleanAsciiName(value.c_str(), "");
    return value;
}

static bool TextMatchesFilter(const std::string& text, const char* filter)
{
    if (filter == NULL || filter[0] == '\0')
        return true;

    std::string a = text;
    std::string b = filter;
    for (size_t i = 0; i < a.size(); ++i) a[i] = (char)tolower((unsigned char)a[i]);
    for (size_t i = 0; i < b.size(); ++i) b[i] = (char)tolower((unsigned char)b[i]);
    return a.find(b) != std::string::npos;
}

static std::string GetItemClassDisplayName(int itemClass)
{
    char fallback[64];
    _snprintf(fallback, sizeof(fallback) - 1, "Class %d", itemClass);
    fallback[sizeof(fallback) - 1] = '\0';

#if __CONTENTS(__ITEM_SHOW)
    if (itemClass >= 0 && itemClass < MAX_ITEM_CLASS) {
        std::string english = ExtractParenthesizedAscii(ITEM_SHOW_NAME[itemClass]);
        if (!english.empty())
            return english;
        return CleanAsciiName(ITEM_SHOW_NAME[itemClass], fallback);
    }
#endif

    return fallback;
}

static std::string GetItemTypeDisplayName(int itemClass, int itemType)
{
    char fallback[64];
    _snprintf(fallback, sizeof(fallback) - 1, "Item %d-%d", itemClass, itemType);
    fallback[sizeof(fallback) - 1] = '\0';

    if (g_pItemTable == NULL) return fallback;
    if (itemClass < 0 || itemClass >= (int)g_pItemTable->GetSize()) return fallback;
    if (itemType < 0 || itemType >= (int)(*g_pItemTable)[itemClass].GetSize()) return fallback;

    const ITEMTABLE_INFO& info = (*g_pItemTable)[itemClass][itemType];
    std::string name = CleanAsciiName(info.EName.GetString(), "");
    if (name.empty())
        name = CleanAsciiName(info.HName.GetString(), "");
    if (name.empty())
        name = fallback;
    return name;
}

static std::string FormatListLabel(int id, const std::string& name)
{
    char prefix[32];
    _snprintf(prefix, sizeof(prefix) - 1, "%d - ", id);
    prefix[sizeof(prefix) - 1] = '\0';
    return std::string(prefix) + name;
}

static bool ItemClassSupportsOptions(int itemClass)
{
    switch (itemClass) {
    case ITEM_CLASS_RING:
    case ITEM_CLASS_BRACELET:
    case ITEM_CLASS_NECKLACE:
    case ITEM_CLASS_COAT:
    case ITEM_CLASS_TROUSER:
    case ITEM_CLASS_SHOES:
    case ITEM_CLASS_SWORD:
    case ITEM_CLASS_BLADE:
    case ITEM_CLASS_SHIELD:
    case ITEM_CLASS_CROSS:
    case ITEM_CLASS_GLOVE:
    case ITEM_CLASS_HELM:
    case ITEM_CLASS_SG:
    case ITEM_CLASS_SMG:
    case ITEM_CLASS_AR:
    case ITEM_CLASS_SR:
    case ITEM_CLASS_BELT:
    case ITEM_CLASS_VAMPIRE_RING:
    case ITEM_CLASS_VAMPIRE_BRACELET:
    case ITEM_CLASS_VAMPIRE_NECKLACE:
    case ITEM_CLASS_VAMPIRE_COAT:
    case ITEM_CLASS_SKULL:
    case ITEM_CLASS_MACE:
    case ITEM_CLASS_SERUM:
    case ITEM_CLASS_VAMPIRE_EARRING:
    case ITEM_CLASS_VAMPIRE_WEAPON:
    case ITEM_CLASS_VAMPIRE_AMULET:
    case ITEM_CLASS_COUPLE_RING:
    case ITEM_CLASS_VAMPIRE_COUPLE_RING:
    case ITEM_CLASS_OUSTERS_ARMSBAND:
    case ITEM_CLASS_OUSTERS_BOOTS:
    case ITEM_CLASS_OUSTERS_CHAKRAM:
    case ITEM_CLASS_OUSTERS_CIRCLET:
    case ITEM_CLASS_OUSTERS_COAT:
    case ITEM_CLASS_OUSTERS_PENDENT:
    case ITEM_CLASS_OUSTERS_RING:
    case ITEM_CLASS_OUSTERS_STONE:
    case ITEM_CLASS_OUSTERS_WRISTLET:
    case ITEM_CLASS_CARRYING_RECEIVER:
    case ITEM_CLASS_SHOULDER_ARMOR:
    case ITEM_CLASS_DERMIS:
    case ITEM_CLASS_PERSONA:
    case ITEM_CLASS_FASCIA:
    case ITEM_CLASS_MITTEN:
    case ITEM_CLASS_ETHEREAL_CHAIN:
    case ITEM_CLASS_OUSTERS_HARMONIC_PENDENT:
    case ITEM_CLASS_CUE_OF_ADAM:
        return true;
    default:
        return false;
    }
}

static bool ItemClassSupportsStackCount(int itemClass)
{
    switch (itemClass) {
    case ITEM_CLASS_POTION:
    case ITEM_CLASS_WATER:
    case ITEM_CLASS_HOLYWATER:
    case ITEM_CLASS_MAGAZINE:
    case ITEM_CLASS_BOMB_MATERIAL:
    case ITEM_CLASS_ETC:
    case ITEM_CLASS_KEY:
    case ITEM_CLASS_BOMB:
    case ITEM_CLASS_MINE:
    case ITEM_CLASS_EVENT_STAR:
    case ITEM_CLASS_EVENT_ETC:
    case ITEM_CLASS_DYE_POTION:
    case ITEM_CLASS_MIXING_ITEM:
    case ITEM_CLASS_PET_FOOD:
    case ITEM_CLASS_LUCKY_BAG:
        return true;
    default:
        return false;
    }
}

static void RebuildItemClassEntries()
{
    int tableSize = (g_pItemTable != NULL) ? (int)g_pItemTable->GetSize() : 0;
    if (tableSize == g_cachedItemTableSize && !g_itemClassEntries.empty())
        return;

    g_itemClassEntries.clear();
    g_cachedItemTableSize = tableSize;

    int limit = tableSize;
    if (limit > MAX_ITEM_CLASS) limit = MAX_ITEM_CLASS;
    for (int i = 0; i < limit; ++i) {
        AdminListEntry e;
        e.id = i;
        e.subId = -1;
        e.label = FormatListLabel(i, GetItemClassDisplayName(i));
        g_itemClassEntries.push_back(e);
    }
}

static void RebuildItemTypeEntries(int itemClass)
{
    int typeSize = 0;
    if (g_pItemTable != NULL && itemClass >= 0 && itemClass < (int)g_pItemTable->GetSize())
        typeSize = (int)(*g_pItemTable)[itemClass].GetSize();

    if (itemClass == g_cachedItemTypeClass &&
        typeSize == g_cachedItemTypeSize &&
        !g_itemTypeEntries.empty())
        return;

    g_itemTypeEntries.clear();
    g_cachedItemTypeClass = itemClass;
    g_cachedItemTypeSize = typeSize;

    for (int i = 0; i < typeSize; ++i) {
        AdminListEntry e;
        e.id = itemClass;
        e.subId = i;
        e.label = FormatListLabel(i, GetItemTypeDisplayName(itemClass, i));
        g_itemTypeEntries.push_back(e);
    }
}

static void RebuildCreatureEntries()
{
    int tableSize = (g_pCreatureTable != NULL) ? (int)g_pCreatureTable->GetSize() : 0;
    if (tableSize == g_cachedCreatureTableSize && !g_creatureEntries.empty())
        return;

    g_creatureEntries.clear();
    g_cachedCreatureTableSize = tableSize;

    for (int i = 0; i < tableSize; ++i) {
        const CREATURETABLE_INFO& info = (*g_pCreatureTable)[i];
        std::string name = CleanAsciiName(info.Name.GetString(), "");
        char fallback[64];
        _snprintf(fallback, sizeof(fallback) - 1, "Creature %d", i);
        fallback[sizeof(fallback) - 1] = '\0';
        if (name.empty())
            name = fallback;

        AdminListEntry e;
        e.id = i;
        e.subId = -1;
        e.label = FormatListLabel(i, name);
        g_creatureEntries.push_back(e);
    }
}

static void RebuildZoneEntries()
{
    int tableSize = 0;
    if (g_pZoneTable != NULL)
        tableSize = (int)g_pZoneTable->GetZoneMap().size();

    if (tableSize == g_cachedZoneTableSize && !g_zoneEntries.empty())
        return;

    g_zoneEntries.clear();
    g_cachedZoneTableSize = tableSize;
    if (g_pZoneTable == NULL)
        return;

    const CZoneTable::ZONEINFO_MAP& zoneMap = g_pZoneTable->GetZoneMap();
    for (CZoneTable::ZONEINFO_MAP::const_iterator it = zoneMap.begin();
         it != zoneMap.end(); ++it) {
        if (it->second == NULL) continue;

        const ZONETABLE_INFO* info = it->second;
        std::string name = CleanAsciiName(info->Name.GetString(), "");
        if (name.empty())
            name = CleanAsciiName(info->Filename.GetString(), "");

        char fallback[64];
        _snprintf(fallback, sizeof(fallback) - 1, "Zone %d", (int)info->ID);
        fallback[sizeof(fallback) - 1] = '\0';
        if (name.empty())
            name = fallback;

        AdminListEntry e;
        e.id = (int)info->ID;
        e.subId = -1;
        e.label = FormatListLabel((int)info->ID, name);
        g_zoneEntries.push_back(e);
    }
}

//----------------------------------------------------------------------
// Cooldown helper — network spam onleme (200ms button cooldown)
//----------------------------------------------------------------------
static DWORD g_lastSendTick = 0;
static const DWORD kSendCooldownMs = 200;

static bool CanSendPacket()
{
    DWORD now = GetTickCount();
    if (now - g_lastSendTick < kSendCooldownMs)
        return false;
    g_lastSendTick = now;
    return true;
}

static bool SendChatCommand(const char* msg);

static void SplitProtocolFields(const char* message, std::vector<std::string>& out)
{
    out.clear();
    if (message == NULL)
        return;

    std::string text(message);
    size_t start = 0;
    while (start <= text.size()) {
        size_t pos = text.find('|', start);
        if (pos == std::string::npos) {
            out.push_back(text.substr(start));
            break;
        }

        out.push_back(text.substr(start, pos - start));
        start = pos + 1;
    }
}

static void RequestOnlinePlayers(bool force)
{
    if (!IsAdminAuthorized())
        return;

    DWORD now = GetTickCount();
    if (!force && g_onlinePlayersEverRequested &&
        now - g_onlinePlayersLastRequestTick < 1000)
        return;

    if (!CanSendPacket())
        return;

    if (SendChatCommand("*command gmplayers refresh")) {
        g_onlinePlayersLoading = true;
        g_onlinePlayersEverRequested = true;
        g_onlinePlayersLastRequestTick = now;
        g_onlinePlayersStatus = "Request sent.";
    } else {
        g_onlinePlayersLoading = false;
        g_onlinePlayersStatus = "Request failed: client socket is not ready.";
    }
}

static void TeleportToOnlinePlayer(const AdminOnlinePlayerRow& row)
{
    if (!IsAdminAuthorized() || row.name.empty())
        return;

    if (!CanSendPacket())
        return;

    char msg[256];
    _snprintf(msg, sizeof(msg) - 1, "*command gmgoto %s", row.name.c_str());
    msg[sizeof(msg) - 1] = '\0';

    if (SendChatCommand(msg)) {
        g_onlinePlayersStatus = "Teleport request sent.";
    } else {
        g_onlinePlayersStatus = "Teleport failed: client socket is not ready.";
    }
}

// GM Recall: secili oyuncuyu GM'nin yanina ceker.
// Server yolu: CGSay "*recall <Name>" -> CGSayHandler::oprecall (yeni packet yok).
// Referans/Phoenix: komut string'i birebir "*recall", hedef isimle secilir.
static void RecallOnlinePlayer(const AdminOnlinePlayerRow& row)
{
    if (!IsAdminAuthorized() || row.name.empty())
        return;

    if (!CanSendPacket())
        return;

    char msg[256];
    _snprintf(msg, sizeof(msg) - 1, "*recall %s", row.name.c_str());
    msg[sizeof(msg) - 1] = '\0';

    if (SendChatCommand(msg)) {
        g_onlinePlayersStatus = "Recall request sent.";
    } else {
        g_onlinePlayersStatus = "Recall failed: client socket is not ready.";
    }
}

bool HandleGMOnlinePlayersProtocolMessage(const char* message)
{
    static const char* kPrefix = "@PHX_GMPLAYERS|";
    if (message == NULL || strncmp(message, kPrefix, strlen(kPrefix)) != 0)
        return false;

    std::vector<std::string> fields;
    SplitProtocolFields(message, fields);
    if (fields.size() < 2) {
        g_onlinePlayersLoading = false;
        g_onlinePlayersStatus = "Invalid GM player protocol message.";
        return true;
    }

    const std::string& action = fields[1];
    if (action == "BEGIN") {
        g_onlinePlayers.clear();
        g_onlinePlayersLoading = true;
        g_onlinePlayersStatus = "Loading online players...";
        return true;
    }

    if (action == "ROW") {
        if (fields.size() < 11) {
            g_onlinePlayersStatus = "Invalid GM player row.";
            return true;
        }

        AdminOnlinePlayerRow row;
        row.name = fields[2];
        row.race = fields[3];
        row.playerClass = fields[4];
        row.level = atoi(fields[5].c_str());
        row.rank = fields[6];
        row.map = fields[7];
        row.zoneId = atoi(fields[8].c_str());
        row.x = atoi(fields[9].c_str());
        row.y = atoi(fields[10].c_str());
        g_onlinePlayers.push_back(row);
        return true;
    }

    if (action == "END") {
        g_onlinePlayersLoading = false;
        g_onlinePlayersLastUpdateTick = GetTickCount();

        char status[96];
        _snprintf(status, sizeof(status) - 1, "Loaded %d online player(s).",
                  (int)g_onlinePlayers.size());
        status[sizeof(status) - 1] = '\0';
        g_onlinePlayersStatus = status;
        return true;
    }

    if (action == "ERROR") {
        g_onlinePlayersLoading = false;
        g_onlinePlayersStatus = fields.size() >= 3 ? fields[2] : "Server rejected the request.";
        return true;
    }

    if (action == "TELEPORT") {
        g_onlinePlayersStatus = fields.size() >= 3 ? fields[2] : "Teleport response received.";
        return true;
    }

    g_onlinePlayersLoading = false;
    g_onlinePlayersStatus = "Unknown GM player protocol action.";
    return true;
}

static void DrawOnlinePlayerList()
{
    ImGui::SeparatorText("GM Online Players");
    if (ImGui::Button("Refresh", ImVec2(86, 26))) {
        RequestOnlinePlayers(true);
    }
    ImGui::SameLine();
    if (g_onlinePlayersLoading)
        ImGui::TextDisabled("Loading...");
    else
        ImGui::TextDisabled("%s", g_onlinePlayersStatus.c_str());

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_NoSavedSettings |
        ImGuiTableFlags_SizingStretchSame |
        ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("##gm_online_players_live", 7, tableFlags, ImVec2(720.0f, 180.0f))) {
        // Go + Pull SmallButton ayni hucrede; Go boyu korunur, sutun genisletilir.
        ImGui::TableSetupColumn("Teleport", ImGuiTableColumnFlags_WidthFixed, 118.0f);
        ImGui::TableSetupColumn("PLAYER NAME", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("RACE", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("CLASS", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("LEVEL", ImGuiTableColumnFlags_WidthFixed, 54.0f);
        ImGui::TableSetupColumn("RANK", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("MAP", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < g_onlinePlayers.size(); ++i) {
            const AdminOnlinePlayerRow& row = g_onlinePlayers[i];

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID((int)i);
            if (ImGui::SmallButton("Go"))
                TeleportToOnlinePlayer(row);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Teleport to %s (%s %d,%d)",
                                  row.name.c_str(), row.map.c_str(), row.x, row.y);
            ImGui::SameLine(0.0f, 4.0f);
            if (ImGui::SmallButton("Pull"))
                RecallOnlinePlayer(row);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Recall %s to your position", row.name.c_str());
            ImGui::PopID();

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(row.name.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(row.race.c_str());
            ImGui::TableSetColumnIndex(3);
            ImGui::TextUnformatted(row.playerClass.c_str());
            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%d", row.level);
            ImGui::TableSetColumnIndex(5);
            ImGui::TextUnformatted(row.rank.c_str());
            ImGui::TableSetColumnIndex(6);
            ImGui::TextUnformatted(row.map.c_str());
            // 2026-09-02: harita adinin sagina zone ID (protokol field 8,
            // row.zoneId — zaten parse ediliyor, yalniz gosterim eklendi)
            ImGui::SameLine(0.0f, 6.0f);
            ImGui::TextDisabled("(%d)", row.zoneId);
        }

        ImGui::EndTable();
    }

    if (g_onlinePlayersLastUpdateTick != 0) {
        DWORD age = (GetTickCount() - g_onlinePlayersLastUpdateTick) / 1000;
        ImGui::TextDisabled("Last update: %lu sec ago", (unsigned long)age);
    }
}

static void DrawOnlinePlayerWindow(bool* p_open)
{
    if (p_open == NULL || !*p_open)
        return;

    ImGui::SetNextWindowSize(ImVec2(760, 260), ImGuiCond_FirstUseEver);
    if (!g_onlinePlayersEverRequested)
        RequestOnlinePlayers(true);

    if (ImGui::Begin("GM Online Players", p_open, ImGuiWindowFlags_NoCollapse)) {
        DrawOnlinePlayerList();
    }
    ImGui::End();
}

//----------------------------------------------------------------------
// CGSay chat command send (Phoenix kanonik admin pattern)
//
// 2026-05-18 FIX: Onceki versiyonda `#ifdef CONNECT_SERVER` guard vardi
// (yanlislikla Plan agent'in tek bir ornegine bakarak konuldu). Phoenix
// production code'da 50+ `g_pSocket->sendPacket()` cagri yeri var ve
// COGU GUARD YOK — `#ifdef CONNECT_SERVER` Phoenix'in eski conditional'i.
// Modern Phoenix Release build socket send direkt yapiyor. Guard kaldirildi.
//----------------------------------------------------------------------
static bool SendChatCommand(const char* msg)
{
    if (msg == NULL) return false;
    if (g_pSocket == NULL) return false;
    CGSay packet;
    std::string m(msg);
    packet.setMessage(m);
    g_pSocket->sendPacket(&packet);
    return true;
}

//----------------------------------------------------------------------
// Spawn / Teleport / Raw implementations
//----------------------------------------------------------------------

// *create ClassN TypeM [option] [#thirdOption] (time) [stack]
static void DoSpawnItem()
{
    int itemClass = g_item.itemClass;
    int itemType = g_item.itemType;
    int count = g_item.itemCount;
    int stackCount = g_item.stackCount;
    int durationSeconds = g_item.durationSeconds;
    const bool supportsOptions = ItemClassSupportsOptions(itemClass);
    const bool supportsStack = ItemClassSupportsStackCount(itemClass);

    if (itemClass < 0) itemClass = 0;
    if (itemType < 0) itemType = 0;
    if (count < 1) count = 1;
    if (count > 30) count = 30;
    if (stackCount < 1) stackCount = 1;
    if (stackCount > 999) stackCount = 999;
    if (g_item.limitedDuration && durationSeconds < 1) durationSeconds = 1;

    std::string command = "*create ";
    command += std::to_string(itemClass);
    command += " ";
    command += std::to_string(itemType);

    if (supportsOptions) {
        if (g_item.useOption1) {
            command += " ";
            command += std::to_string(g_item.option1);
        }
        if (g_item.useOption2) {
            command += " ";
            command += std::to_string(g_item.option2);
        }
        if (g_item.useThirdOption1) {
            command += " #";
            command += std::to_string(g_item.thirdOption1);
        }
        if (g_item.useThirdOption2) {
            command += " #";
            command += std::to_string(g_item.thirdOption2);
        }
    }

    command += " (";
    command += g_item.limitedDuration ? std::to_string(durationSeconds) : "0";
    command += ")";

    if (supportsStack && stackCount > 1) {
        command += " [";
        command += std::to_string(stackCount);
        command += "]";
    }

    char msg[256];
    _snprintf(msg, sizeof(msg) - 1, "%s", command.c_str());
    msg[sizeof(msg) - 1] = '\0';

    for (int i = 0; i < count; ++i) {
        if (i > 0) {
            Sleep(kSendCooldownMs);
            g_lastSendTick = GetTickCount();
        } else {
            CanSendPacket();
        }
        SendChatCommand(msg);
    }

    char hist[256];
    _snprintf(hist, sizeof(hist) - 1, "CREATE x%d: %s", count, msg);
    hist[sizeof(hist) - 1] = '\0';
    PushHistory(g_item.history, hist);
}

// *summon {MonsterType} count
static void DoSpawnCreature(int creatureType, int count)
{
    if (count < 1) count = 1;
    if (count > 30) count = 30;
    if (!CanSendPacket()) return;

    char msg[256];
    _snprintf(msg, sizeof(msg) - 1, "*summon {%d} %d", creatureType, count);
    msg[sizeof(msg) - 1] = '\0';
    SendChatCommand(msg);

    char hist[256];
    _snprintf(hist, sizeof(hist) - 1,
              "SpawnCreature type=%d count=%d", creatureType, count);
    hist[sizeof(hist) - 1] = '\0';
    PushHistory(g_creature.history, hist);
}

// Free-form admin command (Commands tab) — direct CGSay send
static void DoRawCommand(const char* msg)
{
    if (msg == NULL || msg[0] == '\0') return;
    if (!CanSendPacket()) return;

    char safebuf[256];
    _snprintf(safebuf, sizeof(safebuf) - 1, "%s", msg);
    safebuf[sizeof(safebuf) - 1] = '\0';

    SendChatCommand(safebuf);

    char hist[280];
    _snprintf(hist, sizeof(hist) - 1, "SEND: %s", safebuf);
    hist[sizeof(hist) - 1] = '\0';
    PushHistory(g_commands.history, hist);
}

// *warp ZoneID X Y
static void DoTeleport(int zoneId, int x, int y)
{
    if (x < 0 || x > 255 || y < 0 || y > 255) {
        char hist[256];
        _snprintf(hist, sizeof(hist) - 1,
                  "Teleport REJECTED zone=%d xy=(%d,%d) — X/Y must be 0..255",
                  zoneId, x, y);
        hist[sizeof(hist) - 1] = '\0';
        PushHistory(g_zone.history, hist);
        return;
    }
    if (!CanSendPacket()) return;

    char msg[256];
    _snprintf(msg, sizeof(msg) - 1, "*warp %d %d %d", zoneId, x, y);
    msg[sizeof(msg) - 1] = '\0';
    SendChatCommand(msg);

    char hist[256];
    _snprintf(hist, sizeof(hist) - 1,
              "Teleport zone=%d xy=(%d,%d)", zoneId, x, y);
    hist[sizeof(hist) - 1] = '\0';
    PushHistory(g_zone.history, hist);
}

//----------------------------------------------------------------------
// Commands tab — Kategori tree + dinamik form (KAPSAMLI UI)
//----------------------------------------------------------------------

// Komut tanim struct'i
struct AdminCommand {
    const char* category;
    const char* label;
    const char* syntax;
    const char* example;
    const char* auth;
    const char* description;
};

// Komut tablosu (Dev_Log/admin_commands_reference.md kaynak)
// DISABLED'lar dahil yok: *notice, *combat, *7862295/6/7
static const AdminCommand kCommandDefs[] = {
    // Item & Monster
    { "Item & Monster", "*create",         "*create <Class> <Type> [(hours)]",                       "*create 11 0",            "[GOD]",     "Inventory'e item yaratir. Limited sure: (hours) ekle. CORPSE/MOTORCYCLE/RELIC blocked. MAX 2 option/list." },
    { "Item & Monster", "*summon",         "*summon <Name|{Type}|SpriteID> <count> [mad] [group]",   "*summon {10} 1",          "[GOD/DM]",  "Monster spawn yaninda. Count 1..30 clamp. SAFE_ZONE blocked." },
    { "Item & Monster", "*fun mine",       "*fun mine <ItemType> <count>",                           "*fun mine 0 10",          "[!PLAYER]", "Caller konumuna count adet mayin koyar. Damage=ItemInfo, SkillLevel=100. Count 1..36." },

    // Movement
    { "Movement",       "*warp",           "*warp <zone> <x> <y>",                                   "*warp 1820 100 100",      "[!PLAYER]", "Karakteri zone+xy'a isinlar. Zone name veya ID. X/Y 0..255 zorunlu." },
    { "Movement",       "*recall",         "*recall <name1> [name2...]",                             "*recall LostPlayer",      "[!PLAYER]", "Oyuncuyu caller yanina cagir. Coklu isim destegi. Siege zone'da effect-aware (defender 172,38 / attacker 20,232)." },
    // NOTE: *mrecall dispatch /* */ comment'inde (CGSayHandler.cpp:484-507), opmrecall body BOSS — no-op. Preset listesinden cikarildi.

    // Inspection
    { "Inspection",     "*info",           "*info <player>",                                         "*info PlayerName",        "[!PLAYER]", "Hedef PC full stat dump (STR/DEX/INT/HP/MP/Fame/Gold + IP host)." },
    { "Inspection",     "*find",           "*find <player>",                                         "*find PlayerName",        "[ALL]",     "Player'in hangi GameServerGroup'ta logged oldugunu soyler. UYARI: auth check yok, PLAYER bile calistirir (locate privacy)." },
    { "Inspection",     "*trace",          "*trace <player>",                                        "*trace PlayerName",       "[!PLAYER]", "Hedef creature'a teleport et (transportCreature)." },
    { "Inspection",     "*trace NPC",      "*trace NPC <name>",                                      "*trace NPC Matthew",      "[!PLAYER]", "NPC'ye teleport et." },
    { "Inspection",     "*view",           "*view <variable>",                                       "*view exp_ratio",         "[GOD/DM]",  "Server-side g_pVariableManager 30+ degisken oku (event/exp_ratio/item_prob_ratio/premium/master_lair vs.)." },
    { "Inspection",     "*user",           "*user",                                                  "*user",                   "[!PLAYER]", "DB'de LogOn IN ('GAME','LOGON') olan oyuncu sayisi (online count). NO arg." },

    // Moderation
    { "Moderation",     "*kick",           "*kick <player>",                                         "*kick BadPlayer",         "[!PLAYER]", "Oyuncuyu PENALTY_TYPE_KICKED ile disconnect et." },
    { "Moderation",     "*mute",           "*mute <player> [minutes]",                               "*mute SpamBot 60",        "[!PLAYER]", "Chat susturma. Minutes verilmezse PENALTY_TYPE_MUTE kalici. Verilirse EffectMute time*600turn=time saniye*60=time dakika." },
    { "Moderation",     "*denychat",       "*denychat <1-5> <player>",                               "*denychat 3 SpamBot",     "[!PLAYER]", "Time-based mute. Level->sure: 1=1h, 2=6h, 3=12h, 4=24h, 5=168h(1hafta)." },
    { "Moderation",     "*deny",           "*deny <player>",                                         "*deny BannedAccount",     "[GOD/DM]",  "DB'de Player.Access='DENY' set. Sonraki login'de engellenir." },
    { "Moderation",     "*freezing",       "*freezing <player>",                                     "*freezing PlayerName",    "[!PLAYER]", "PENALTY_TYPE_FREEZING flag (hareket engeli)." },
    { "Moderation",     "*DSG",            "*DSG <player> <iStat>",                                  "*DSG PlayerName 1",       "[!PLAYER]", "Dungeon Special Group flag — DSGMemberList tablosunda DsgStat=<iStat> integer guncelle (organizasyon flag, NOT stat reduce)." },

    // Self / Save / Log
    { "Self",           "*save",           "*save",                                                  "*save",                   "[DM/GOD]",  "Tum aktif zone PC'lerini DB'ye yaz. Tum oyunculara STRID_SAVE_YOUR_DATA broadcast." },
    { "Self",           "*log",            "*log <player> [seconds]",                                "*log Cheater 3600",       "[GOD/DM]",  "Hedef oyuncuda startPacketLog(sec) — tum paketleri dosyaya kaydet (cheat investigation). Default 600s." },
    { "Self",           "*soulchain",      "*soulchain <player>",                                    "*soulchain TargetPlayer", "[!PLAYER]", "CGSkillToNamed ile SKILL_SOUL_CHAIN execute (admin path skill bypass)." },
    { "Self",           "*bug_report",     "*bug_report <msg>",                                      "*bug_report Stuck in wall","[ALL]",    "BugReportLog DB tablosuna insert (escape: ' ve \\ -> _). PLAYER de cagirir." },

    // Server Var
    { "Server Var",     "*get",            "*get <variable>",                                        "*get rare_item_ratio",    "[GOD]",     "opview alt-kumesi: item_prob_ratio, premium_item_probe_percent, rare_item_ratio, chief_monster_rare_item_percent. Sayisal donus." },
    { "Server Var",     "*set",            "*set <variable> <value>",                                "*set exp_ratio 200",      "[GOD]",     "opview'deki 44 set_type degiskeni yaz. Bkz. admin_commands_reference.md section 3 *view tam liste." },
    { "Server Var",     "*load",           "*load <type> [value]",                                   "*load monster_info",      "[GOD]",     "EventReloadInfo runtime reload. Tipler: master_lair_info, monster_info, monster_ai, zone_info, zone <ID>, log_user, item_info <Class>, option_info, rank_bonus_info, string_pool, war_schedule_info, blood_bible_owner, sweeper_owner <lvl>, race_war_pc_limit, npc <ZoneID>, goods_list_info." },
    { "Server Var",     "*credit",         "*credit <devName>",                                      "*credit Kim",             "[ALL]",     "Hardcoded developer credit lookup (easter egg). Korean dev isim -> rol bilgisi. NOT credit/gold ekleme!" },
    { "Server Var",     "*pay",            "*pay",                                                   "*pay",                    "[!PLAYER]", "Caller'in billing/PayPlay durumunu ozet (Metrotech eski abonelik). NO arg." },

    // Server State
    { "Server",         "*shutdown",       "*shutdown <turns>",                                      "*shutdown 18000",         "[GOD]",     "Server shutdown. 10 turn=1sn (Effect.cpp:83). 3000=5dk, 18000=30dk, 36000=1saat." },
    { "Server",         "*wall",           "*wall <msg>",                                            "*wall Server bakim 5 dk", "[GOD]",     "Tum oyunculara global broadcast." },
    { "Server",         "*world",          "*world <command>",                                       "*world *save",            "[!PLAYER]", "Komutu yerel calistir + ayni WorldID'deki tum GameServer'lara UDP ile yay (GGCommand packet). 'subcmd' degil tam komut: ornek *world *save / *world *wall msg." },
    { "Server",         "*allworld",       "*allworld <command>",                                    "*allworld *wall Bakim",   "[GOD/DM]",  "*world ama TUM World'lere yay (cross-world broadcast)." },

    // Account
    { "Account",        "*grant",          "*grant <competence> <player>",                           "*grant 0 PlayerName",     "[GOD]",     "Admin yetki ata. Competence: 0=GOD, 1=DM, 2=HELPER, 3=PLAYER (CreatureTypes.h:50)." },
    { "Account",        "*billing disconnect","*billing disconnect",                                 "*billing disconnect",     "[GOD]",     "g_pBillingPlayerManager->setForceDisconnect(). __CONNECT_BILLING_SYSTEM__ build flag tanimliysa calisir; Phoenix server'da muhtemelen no-op." },

    // Guild
    { "Guild",          "*guild",          "*guild changeGuildMaster <guildID> <name>",              "*guild changeGuildMaster 7 NewMaster", "[GOD]", "Guild master degistir. Race-aware (Slayer fame+domain level, Vampire/Ousters level threshold)." },

    // *command subcommands (KANIT: ServerSRC grep, 87 gercek subcommand)
    // ---- Self state ----
    { "*command",       "invincible",      "*command invincible",                                    "*command invincible",     "[GOD/DM]",  "GOD Mode On/Off" },
    { "*command",       "ghost",           "*command ghost",                                         "*command ghost",          "[GOD/DM]",  "Ghost Mode On/Off" },
    { "*command",       "heal",            "*command heal",                                          "*command heal",           "[GOD/DM]",  "Self full HP/MP restore." },
    { "*command",       "suicide",         "*command suicide",                                       "*command suicide",        "[GOD/DM]",  "Self HP 0 (test death flow)." },
    { "*command",       "advanceclass",    "*command advanceclass",                                  "*command advanceclass",   "[GOD/DM]",  "Advancement class exp +1." },
    { "*command",       "changeSex",       "*command changeSex",                                     "*command changeSex",      "[GOD/DM]",  "Change the character's gender." },
    { "*command",       "ResetAttr",       "*command ResetAttr",                                     "*command ResetAttr",      "[GOD/DM]",  "Attribute (str/dex/int) reset." },
    { "*command",       "SetHP",           "*command SetHP <value>",                                 "*command SetHP 9999",     "[GOD/DM]",  "Caller HP set." },
    { "*command",       "setGold",         "*command setGold <amount>",                              "*command setGold 100000", "[GOD/DM]",  "Caller gold set." },
    // ---- Inventory / Stash ----
    { "*command",       "clearInventory",  "*command clearInventory",                                "*command clearInventory", "[GOD/DM]",  "Clear Inventory." },
    { "*command",       "clearRankBonus",  "*command clearRankBonus",                                "*command clearRankBonus", "[GOD/DM]",  "Clears Rank Bonus" },
    { "*command",       "PetStash",        "*command PetStash",                                      "*command PetStash",       "[GOD/DM]",  "Pet Stash" },
    // ---- Effects ----
    { "*command",       "BloodBurst",      "*command BloodBurst",                                    "*command BloodBurst",     "[GOD/DM]",  "Attack/Defense BloodBurst point MAX." },
    { "*command",       "ViewDamage",      "*command ViewDamage <on/off>",                           "*command ViewDamage on",  "[GOD/DM]",  "Damage number gorunurluk." },
    { "*command",       "Loud",            "*command Loud <name> <time>",                            "*command Loud Bell 10",   "[GOD/DM]",  "EffectLoud, zone broadcast (time*10 deadline)." },
    { "*command",       "Horn",            "*command Horn",                                          "*command Horn",           "[GOD/DM]",  "RunHorn notice event (zone-wide)." },
    // ---- Zone / Effect bulk ----
    { "*command",       "twiceMoney",      "*command twiceMoney",                                    "*command twiceMoney",     "[GOD/DM]",  "Money 2x debug toggle." },
    { "*command",       "showpcstat",      "*command showpcstat",                                    "*command showpcstat",     "[GOD/DM]",  "Zone: Slayer/Vampire/Ousters count." },
    { "*command",       "showZonePCNum",   "*command showZonePCNum",                                 "*command showZonePCNum",  "[GOD/DM]",  "Zone: total number of PCs." },
    { "*command",       "setTimeOutAllZoneEffect", "*command setTimeOutAllZoneEffect",               "*command setTimeOutAllZoneEffect", "[GOD/DM]", "Zone: All effect timeout." },
    // ---- Bulk kill / heal ----
    { "*command",       "killAllPC",       "*command killAllPC",                                     "*command killAllPC",      "[GOD/DM]",  "Zone: Kill All PC's" },
    { "*command",       "killAllMonster",  "*command killAllMonster",                                "*command killAllMonster", "[GOD/DM]",  "Zone: Kill All Monsters" },
    { "*command",       "killAllSlayer",   "*command killAllSlayer",                                 "*command killAllSlayer",  "[GOD/DM]",  "Zone: Kill All Slayer Race" },
    { "*command",       "killAllVampire",  "*command killAllVampire",                                "*command killAllVampire", "[GOD/DM]",  "Zone: Kill All Vampire Race" },
    { "*command",       "killAllOusters",  "*command killAllOusters",                                "*command killAllOusters", "[GOD/DM]",  "Zone: Kill All Ousters Race" },
    { "*command",       "healAllPC",       "*command healAllPC",                                     "*command healAllPC",      "[GOD/DM]",  "Zone: Heal All Players" },
    { "*command",       "healAllSlayer",   "*command healAllSlayer",                                 "*command healAllSlayer",  "[GOD/DM]",  "Zone: Heal All Slayers" },
    { "*command",       "healAllVampire",  "*command healAllVampire",                                "*command healAllVampire", "[GOD/DM]",  "Zone: Heal All Vampires" },
    { "*command",       "healAllOusters",  "*command healAllOusters",                                "*command healAllOusters", "[GOD/DM]",  "Zone: Heal All Ousters" },
    { "*command",       "KickOutAll",      "*command KickOutAll",                                    "*command KickOutAll",     "[GOD/DM]",  "Zone: DC's All Players" },
    // ---- Events / Bulletin / Firecraker ----
    { "*command",       "Bulletin",        "*command Bulletin <1-3> <msg>",                          "*command Bulletin 1 Test","[GOD/DM]",  "Bulletin (MonsterType 650). Level 1=6h, 2=12h, 3=24h." },
    { "*command",       "createBulletin",  "*command createBulletin <level>",                        "*command createBulletin 1","[GOD/DM]", "Create a bulletin (Bulletin alternative, opcreate proxy))." },
    { "*command",       "Firecraker",      "*command Firecraker <0-13>",                             "*command Firecraker 0",   "[GOD/DM]",  "Fireworks effects (14 types)." },
    { "*command",       "createFirecraker","*command createFirecraker <ItemType>",                   "*command createFirecraker 1","[GOD/DM]","Create Firecraker" },
    { "*command",       "Quest",           "*command Quest Complete",                                "*command Quest Complete", "[GOD/DM]",  "Complete the active MONSTER_KILL quest." },
    { "*command",       "QuestEnding",     "*command QuestEnding",                                   "*command QuestEnding",    "[GOD/DM]",  "QuestEnding notice event." },
    // ---- Castle / Siege / Wars ----
    { "*command",       "setCastleOwner",  "*command setCastleOwner <name>",                         "*command setCastleOwner SlayerCommon", "[GOD/DM]", "Determine the castle owner. SlayerCommon=99, VampireCommon=0, OustersCommon=66." },
    { "*command",       "setCastleOwnerGuild", "*command setCastleOwnerGuild <zoneID> <guildID>",    "*command setCastleOwnerGuild 1131 99","[GOD/DM]","Castle owner explicit guildID." },
    { "*command",       "showWarList",     "*command showWarList",                                   "*command showWarList",    "[GOD/DM]",  "Broadcast active battles." },
    { "*command",       "startRaceWar",    "*command startRaceWar",                                  "*command startRaceWar",   "[GOD/DM]",  "Start RaceWar." },
    { "*command",       "removeRaceWar",   "*command removeRaceWar",                                 "*command removeRaceWar",  "[GOD/DM]",  "Remove RaceWar." },
    { "*command",       "startWar",        "*command startWar",                                      "*command startWar",       "[GOD/DM]",  "Set the nearest 'war schedule' in now" },
    { "*command",       "removeWar",       "*command removeWar [zoneID]",                            "*command removeWar 1131", "[GOD/DM]",  "Remove 'War' (or 'caller zone')." },
    { "*command",       "LevelWar",        "*command LevelWar <zoneID>",                             "*command LevelWar 1131",  "[GOD/DM]",  "LevelWar manualStart (zone 1131-1134)." },
    { "*command",       "ResetSiege",      "*command ResetSiege",                                    "*command ResetSiege",     "[GOD/DM]",  "Zone: SiegeWar Reset." },
    { "*command",       "InitSiege",       "*command InitSiege",                                     "*command InitSiege",      "[GOD/DM]",  "Zone: SiegeWar Start." },
    { "*command",       "IAmAttacker",     "*command IAmAttacker",                                   "*command IAmAttacker",    "[GOD/DM]",  "Caller SIEGE_ATTACKER_1 effect 600 (test)." },
    { "*command",       "IAmDefender",     "*command IAmDefender",                                   "*command IAmDefender",    "[GOD/DM]",  "Caller SIEGE_DEFENDER effect 600." },
    { "*command",       "IAmReinforce",    "*command IAmReinforce",                                  "*command IAmReinforce",   "[GOD/DM]",  "Caller SIEGE_REINFORCE effect 600." },
    { "*command",       "saveBloodBibleOwner", "*command saveBloodBibleOwner",                       "*command saveBloodBibleOwner","[GOD/DM]","Shrine BB owner save the to database." },
    // ---- CTF / FlagWar ----
    { "*command",       "CTF",             "*command CTF",                                           "*command CTF",            "[GOD/DM]",  "CTF (Capture The Flag) start/control." },
    { "*command",       "startFlagWar",    "*command startFlagWar",                                  "*command startFlagWar",   "[GOD/DM]",  "Start FlagWar." },
    { "*command",       "endFlagWar",      "*command endFlagWar",                                    "*command endFlagWar",     "[GOD/DM]",  "Finish FlagWar." },
    // ---- Survival / GDR ----
    { "*command",       "startSurvival",   "*command startSurvival",                                 "*command startSurvival",  "[GOD/DM]",  "Start Survival event" },
    { "*command",       "startRaceSurvival", "*command startRaceSurvival",                           "*command startRaceSurvival","[GOD/DM]","Start Race-based Survival" },
    { "*command",       "StartGDRLair",    "*command StartGDRLair",                                  "*command StartGDRLair",   "[GOD/DM]",  "Start the GDR Lair event." },
    { "*command",       "ResetGDRLair",    "*command ResetGDRLair",                                  "*command ResetGDRLair",   "[GOD/DM]",  "GDR Lair reset." },
    { "*command",       "regenMasterLair", "*command regenMasterLair",                               "*command regenMasterLair","[GOD/DM]",  "Master Lair regen Trigger." },
    { "*command",       "showMasterLairStatus", "*command showMasterLairStatus",                     "*command showMasterLairStatus","[GOD/DM]","Master Lair status update." },
    // ---- Dynamic Zone ----
    { "*command",       "addDynamicZone",  "*command addDynamicZone <params>",                       "*command addDynamicZone", "[GOD/DM]",  "Create Dynamic zone instance" },
    { "*command",       "clearDynamicZone","*command clearDynamicZone",                              "*command clearDynamicZone","[GOD/DM]", "Clear the dynamic zones." },
    { "*command",       "enterDynamicZone","*command enterDynamicZone <id>",                         "*command enterDynamicZone 1","[GOD/DM]","Enter Dynamic zone" },
    // ---- Misc ----
    { "*command",       "balanceZoneGroup","*command balanceZoneGroup",                              "*command balanceZoneGroup","[GOD/DM]", "Bölge grubu yük dengeleme tetikleyicisi." },
    { "*command",       "printTile",       "*command printTile",                                     "*command printTile",      "[GOD/DM]",  "Write the caller's location tile information to the log." },
    { "*command",       "GoodsReload",     "*command GoodsReload",                                   "*command GoodsReload",    "[GOD/DM]",  "Shop goods reload." },
    { "*command",       "GuildRecall",     "*command GuildRecall",                                   "*command GuildRecall",    "[GOD/DM]",  "Lonca üyelerini geri çağır." },
    { "*command",       "ForceNick",       "*command ForceNick <player> <name>",                     "*command ForceNick Old New","[GOD/DM]","Force a name change for the character." },
    { "*command",       "RemoveNick",      "*command RemoveNick <player>",                           "*command RemoveNick Bad", "[GOD/DM]",  "Clear character name." },
    { "*command",       "SMSTest",         "*command SMSTest",                                       "*command SMSTest",        "[GOD/DM]",  "SMS notification test." },
    { "*command",       "NotifyWin",       "*command NotifyWin",                                     "*command NotifyWin",      "[GOD/DM]",  "Send a Win notification." },
};
static const int kCommandDefsCount =
    (int)(sizeof(kCommandDefs) / sizeof(kCommandDefs[0]));

static const char* kCategories[] = {
    "Item & Monster", "Movement", "Inspection", "Moderation",
    "Self", "Server Var", "Server", "Account", "*command"
};
static const int kCategoryCount = (int)(sizeof(kCategories) / sizeof(kCategories[0]));

//----------------------------------------------------------------------
// Tab cizicileri
//----------------------------------------------------------------------

static void DrawItemTab()
{
    RebuildItemClassEntries();
    RebuildItemTypeEntries(g_item.itemClass);

    const bool supportsOptions = ItemClassSupportsOptions(g_item.itemClass);
    const bool supportsStack = ItemClassSupportsStackCount(g_item.itemClass);

    ImGui::BeginChild("##item_controls", ImVec2(0, 118), true);
    ImGui::Text("Selected: %d / %d  %s",
                g_item.itemClass,
                g_item.itemType,
                GetItemTypeDisplayName(g_item.itemClass, g_item.itemType).c_str());

    ImGui::SetNextItemWidth(85.0f);
    ImGui::InputInt("Repeat", &g_item.itemCount);
    if (g_item.itemCount < 1) g_item.itemCount = 1;
    if (g_item.itemCount > 30) g_item.itemCount = 30;

    ImGui::SameLine();
    if (ImGui::RadioButton("Unlimited", !g_item.limitedDuration))
        g_item.limitedDuration = false;
    ImGui::SameLine();
    if (ImGui::RadioButton("Limited", g_item.limitedDuration))
        g_item.limitedDuration = true;
    if (g_item.limitedDuration) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(105.0f);
        ImGui::InputInt("Seconds", &g_item.durationSeconds);
        if (g_item.durationSeconds < 1) g_item.durationSeconds = 1;
    }

    if (supportsStack) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(85.0f);
        ImGui::InputInt("Stack", &g_item.stackCount);
        if (g_item.stackCount < 1) g_item.stackCount = 1;
        if (g_item.stackCount > 999) g_item.stackCount = 999;
    }

    ImGui::BeginDisabled(!supportsOptions);
    ImGui::Checkbox("Opt 1", &g_item.useOption1);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(82.0f);
    ImGui::InputInt("##opt1", &g_item.option1);
    ImGui::SameLine();
    ImGui::Checkbox("Opt 2", &g_item.useOption2);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(82.0f);
    ImGui::InputInt("##opt2", &g_item.option2);
    ImGui::SameLine();
    ImGui::Checkbox("Third 1", &g_item.useThirdOption1);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(82.0f);
    ImGui::InputInt("##third1", &g_item.thirdOption1);
    ImGui::SameLine();
    ImGui::Checkbox("Third 2", &g_item.useThirdOption2);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(82.0f);
    ImGui::InputInt("##third2", &g_item.thirdOption2);
    ImGui::EndDisabled();

    if (!supportsOptions)
        ImGui::TextDisabled("Options unavailable for this item class.");
    ImGui::EndChild();

    const float leftWidth = 245.0f;
    ImGui::BeginChild("##item_classes_panel", ImVec2(leftWidth, -150.0f), true);
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputText("##item_class_filter", g_item.classFilter, sizeof(g_item.classFilter));
    for (size_t i = 0; i < g_itemClassEntries.size(); ++i) {
        const AdminListEntry& e = g_itemClassEntries[i];
        if (!TextMatchesFilter(e.label, g_item.classFilter)) continue;
        bool selected = (g_item.itemClass == e.id);
        if (ImGui::Selectable(e.label.c_str(), selected)) {
            g_item.itemClass = e.id;
            g_item.itemType = 0;
            g_cachedItemTypeClass = -1;
            RebuildItemTypeEntries(g_item.itemClass);
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("##item_types_panel", ImVec2(0, -150.0f), true);
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputText("##item_type_filter", g_item.itemFilter, sizeof(g_item.itemFilter));
    for (size_t i = 0; i < g_itemTypeEntries.size(); ++i) {
        const AdminListEntry& e = g_itemTypeEntries[i];
        if (!TextMatchesFilter(e.label, g_item.itemFilter)) continue;
        bool selected = (g_item.itemType == e.subId);
        if (ImGui::Selectable(e.label.c_str(), selected)) {
            g_item.itemClass = e.id;
            g_item.itemType = e.subId;
            DoSpawnItem();
        }
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Recent");
    if (ImGui::BeginChild("ItemHistory", ImVec2(0, 118), true)) {
        for (size_t i = g_item.history.size(); i > 0; --i)
            ImGui::TextUnformatted(g_item.history[i - 1].c_str());
    }
    ImGui::EndChild();
}

static void DrawCreatureTab()
{
    RebuildCreatureEntries();

    ImGui::SetNextItemWidth(120.0f);
    ImGui::InputInt("Count", &g_creature.spawnCount);
    if (g_creature.spawnCount < 1) g_creature.spawnCount = 1;
    if (g_creature.spawnCount > 30) g_creature.spawnCount = 30;
    ImGui::SameLine();
    ImGui::SetNextItemWidth(260.0f);
    ImGui::InputText("Filter", g_creature.filter, sizeof(g_creature.filter));

    if (ImGui::Button("Spawn Selected", ImVec2(145, 28))) {
        DoSpawnCreature(g_creature.creatureType, g_creature.spawnCount);
    }

    ImGui::BeginChild("##creature_list", ImVec2(0, -150.0f), true);
    for (size_t i = 0; i < g_creatureEntries.size(); ++i) {
        const AdminListEntry& e = g_creatureEntries[i];
        if (!TextMatchesFilter(e.label, g_creature.filter)) continue;
        bool selected = (g_creature.creatureType == e.id);
        if (ImGui::Selectable(e.label.c_str(), selected)) {
            g_creature.creatureType = e.id;
            DoSpawnCreature(g_creature.creatureType, g_creature.spawnCount);
        }
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Recent");
    if (ImGui::BeginChild("CreatureHistory", ImVec2(0, 118), true)) {
        for (size_t i = g_creature.history.size(); i > 0; --i)
            ImGui::TextUnformatted(g_creature.history[i - 1].c_str());
    }
    ImGui::EndChild();
}

static void DrawZoneTab()
{
    RebuildZoneEntries();

    ImGui::SetNextItemWidth(85.0f);
    ImGui::InputInt("X", &g_zone.x);
    if (g_zone.x < 0) g_zone.x = 0;
    if (g_zone.x > 255) g_zone.x = 255;

    ImGui::SameLine();
    ImGui::SetNextItemWidth(85.0f);
    ImGui::InputInt("Y", &g_zone.y);
    if (g_zone.y < 0) g_zone.y = 0;
    if (g_zone.y > 255) g_zone.y = 255;

    ImGui::SameLine();
    ImGui::SetNextItemWidth(260.0f);
    ImGui::InputText("Filter", g_zone.filter, sizeof(g_zone.filter));

    if (ImGui::Button("Warp Selected", ImVec2(145, 28))) {
        DoTeleport(g_zone.zoneId, g_zone.x, g_zone.y);
    }

    ImGui::BeginChild("##zone_list", ImVec2(0, -150.0f), true);
    for (size_t i = 0; i < g_zoneEntries.size(); ++i) {
        const AdminListEntry& e = g_zoneEntries[i];
        if (!TextMatchesFilter(e.label, g_zone.filter)) continue;
        bool selected = (g_zone.zoneId == e.id);
        if (ImGui::Selectable(e.label.c_str(), selected)) {
            g_zone.zoneId = e.id;
            DoTeleport(g_zone.zoneId, g_zone.x, g_zone.y);
        }
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Recent");
    if (ImGui::BeginChild("ZoneHistory", ImVec2(0, 118), true)) {
        for (size_t i = g_zone.history.size(); i > 0; --i)
            ImGui::TextUnformatted(g_zone.history[i - 1].c_str());
    }
    ImGui::EndChild();
}

static void DrawCommandsTab()
{
    ImGui::Text("Admin commands  ( CGSay -> server CGSayHandler )");
    ImGui::TextDisabled("Left: category tree. Right: selected command details. Bottom: free-form + history.");
    ImGui::Separator();

    const float kLeftPanelWidth = ImGui::GetContentRegionAvail().x * 0.40f;
    const float kBottomReserve  = 260.0f;

    // ---- Sol panel (40%): Kategori tree ----
    if (ImGui::BeginChild("##cmdleft", ImVec2(kLeftPanelWidth, -kBottomReserve), true))
    {
        for (int cat = 0; cat < kCategoryCount; ++cat) {
            const char* catName = kCategories[cat];
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
            if (cat == 0) flags |= ImGuiTreeNodeFlags_DefaultOpen;

            if (ImGui::TreeNodeEx(catName, flags)) {
                for (int i = 0; i < kCommandDefsCount; ++i) {
                    if (strcmp(kCommandDefs[i].category, catName) != 0) continue;
                    const bool isSelected = (g_commands.selectedCmdId == i);
                    if (ImGui::Selectable(kCommandDefs[i].label, isSelected)) {
                        g_commands.selectedCmdId = i;
                        _snprintf(g_commands.formBuf, sizeof(g_commands.formBuf) - 1,
                                  "%s", kCommandDefs[i].example);
                        g_commands.formBuf[sizeof(g_commands.formBuf) - 1] = '\0';
                    }
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // ---- Sag panel: Secili komut detay + form ----
    if (ImGui::BeginChild("##cmdright", ImVec2(0, -kBottomReserve), true))
    {
        if (g_commands.selectedCmdId < 0 || g_commands.selectedCmdId >= kCommandDefsCount) {
            ImGui::TextDisabled("Select a command from the left panel...");
        } else {
            const AdminCommand& c = kCommandDefs[g_commands.selectedCmdId];

            // Auth badge (renkli)
            ImVec4 authColor(0.4f, 0.8f, 0.4f, 1.0f);                       // green default
            if (strstr(c.auth, "GOD/DM"))      authColor = ImVec4(1.0f, 0.7f, 0.3f, 1.0f);   // amber
            else if (strstr(c.auth, "[GOD]"))  authColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);   // red GOD-only
            ImGui::TextColored(authColor, "%s", c.auth);
            ImGui::SameLine();
            ImGui::Text("%s", c.label);

            ImGui::Separator();

            ImGui::Text("Syntax:");
            ImGui::TextWrapped("%s", c.syntax);

            ImGui::Spacing();
            ImGui::Text("Description:");
            ImGui::TextWrapped("%s", c.description);

            ImGui::Spacing();
            ImGui::Text("Command (edit + Send):");
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::InputText("##formbuf", g_commands.formBuf, sizeof(g_commands.formBuf));

            ImGui::Spacing();
            if (ImGui::Button("Send", ImVec2(120, 28))) {
                DoRawCommand(g_commands.formBuf);
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset", ImVec2(80, 28))) {
                _snprintf(g_commands.formBuf, sizeof(g_commands.formBuf) - 1,
                          "%s", c.example);
                g_commands.formBuf[sizeof(g_commands.formBuf) - 1] = '\0';
            }
        }
    }
    ImGui::EndChild();

    // ---- Alt panel: Free-form input + history ----
    ImGui::SeparatorText("Free-form (type each command directly)");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 200.0f);
    ImGui::InputText("##rawcmd", g_commands.commandBuf, sizeof(g_commands.commandBuf));
    ImGui::SameLine();
    if (ImGui::Button("Send##raw", ImVec2(90, 0))) {
        DoRawCommand(g_commands.commandBuf);
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear##raw", ImVec2(90, 0))) {
        g_commands.commandBuf[0] = '\0';
    }

    ImGui::SeparatorText("Recent");
    if (ImGui::BeginChild("CmdHistory", ImVec2(0, 110), true)) {
        for (size_t i = g_commands.history.size(); i > 0; --i)
            ImGui::TextUnformatted(g_commands.history[i - 1].c_str());
    }
    ImGui::EndChild();
}

static void DrawTextPerfTab()
{
    bool enabled = Phoenix_TextPerf_IsEnabled();
    if (ImGui::Checkbox("TextPerf Enabled", &enabled))
        Phoenix_TextPerf_SetEnabled(enabled);

    ImGui::SameLine();
    ImGui::TextDisabled("memory-only, no file output");

    ImGui::Separator();

    static char textPerfBuffer[8192];
    Phoenix_TextPerf_FormatOverlay(textPerfBuffer, (int)sizeof(textPerfBuffer));

    if (ImGui::BeginChild("TextPerfSnapshot", ImVec2(0, 0), true,
                          ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGui::TextUnformatted(textPerfBuffer);
    }
    ImGui::EndChild();
}

void DrawAdminQuickButtons()
{
    if (!IsAdminAuthorized())
        return;

    static bool ghostEnabled = false;
    static bool invincibleEnabled = false;
    static bool showOnlinePlayers = false;

    ImGui::SetNextWindowSize(ImVec2(252, 50), ImGuiCond_FirstUseEver);
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize;

    if (!ImGui::Begin("Admin Menu", NULL, flags)) {
        ImGui::End();
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.48f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.62f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.08f, 0.38f, 0.16f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3.0f, 3.0f));

    if (ImGui::Button(ghostEnabled ? "Ghost ON" : "Ghost OFF", ImVec2(84, 21))) {
        ghostEnabled = !ghostEnabled;
        DoRawCommand(ghostEnabled ? "*command ghost on" : "*command ghost off");
    }
    ImGui::SameLine();
    if (ImGui::Button(invincibleEnabled ? "God ON" : "Invincible OFF", ImVec2(93, 21))) {
        invincibleEnabled = !invincibleEnabled;
        DoRawCommand(invincibleEnabled ? "*command invincible on" : "*command invincible off");
    }
    ImGui::SameLine();
    if (ImGui::Button("Players", ImVec2(62, 21))) {
        showOnlinePlayers = true;
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::End();

    DrawOnlinePlayerWindow(&showOnlinePlayers);
}

//----------------------------------------------------------------------
// Top-level — PhoenixImGui::Render() taraf cagrir
//----------------------------------------------------------------------
void DrawDebugConsole(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(860, 640), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Phoenix Debug Console", p_open)) {
        ImGui::End();
        return;
    }
    ImGui::SetWindowFontScale(1.04f);

    if (ImGui::BeginTabBar("##phoenix_debug_tabs")) {
        if (ImGui::BeginTabItem("Items"))          { DrawItemTab();     ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Creature Spawn")) { DrawCreatureTab(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Maps"))           { DrawZoneTab();     ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Commands")) { DrawCommandsTab(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("TextPerf")) { DrawTextPerfTab(); ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

} // namespace PhoenixImGui

bool PhoenixAdminOnlinePlayers_HandleProtocolMessage(const char* message)
{
    return PhoenixImGui::HandleGMOnlinePlayersProtocolMessage(message);
}
