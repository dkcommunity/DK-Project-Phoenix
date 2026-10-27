//----------------------------------------------------------------------
// PhoenixAuctionHouseHUD.cpp
//----------------------------------------------------------------------

#include "PhoenixAuctionHouseHUD.h"

#include "imgui.h"

#include <windows.h>
#include <d3d9.h>

#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../ClientDef.h"
#include "../MItem.h"
#include "../Packet/ClientPlayer.h"
#include "../Packet/Cpackets/CGAuctionBuy.h"
#include "../Packet/Cpackets/CGAuctionCancel.h"
#include "../Packet/Cpackets/CGAuctionListItem.h"
#include "../Packet/Cpackets/CGAuctionRequestList.h"
#include "../Packet/Gpackets/GCAuctionList.h"
#include "../Packet/Gpackets/GCAuctionResult.h"

extern ClientPlayer* g_pSocket;
extern bool PhoenixGetItemSpritePreviewSize(WORD id, int* pWidth, int* pHeight);
extern bool PhoenixCopyItemSpritePreviewArgb(WORD id, DWORD* pDst, int dstPitchPixels, int width, int height);
extern void PhoenixSetAuctionHouseItemDescriptor(int x, int y, MItem* pItem);

namespace PhoenixImGui {

namespace {

struct AuctionEntry
{
    unsigned int listingID;
    unsigned int price;
    unsigned char sellerRace;
    unsigned char itemRace;
    std::string sellerName;
    unsigned char itemClass;
    unsigned short itemType;
    unsigned int objectID;
    unsigned int durability;
    int grade;
    int enchantLevel;
    unsigned int itemNum;
    int inventoryFrameID;
    int gridW;
    int gridH;
    std::string itemName;
    std::unique_ptr<MItem> previewItem;

    AuctionEntry(const AuctionEntry&) = delete;
    AuctionEntry& operator=(const AuctionEntry&) = delete;
    AuctionEntry(AuctionEntry&&) = default;
    AuctionEntry& operator=(AuctionEntry&&) = default;

    AuctionEntry()
        : listingID(0)
        , price(0)
        , sellerRace(0)
        , itemRace(0)
        , itemClass(0)
        , itemType(0)
        , objectID(0)
        , durability(0)
        , grade(0)
        , enchantLevel(0)
        , itemNum(0)
        , inventoryFrameID(0)
        , gridW(1)
        , gridH(1)
    {
    }
};

struct ItemTexture
{
    IDirect3DTexture9* texture;
    int width;
    int height;

    ItemTexture()
        : texture(NULL)
        , width(0)
        , height(0)
    {
    }
};

struct SelectedAuctionItem
{
    bool valid;
    unsigned int objectID;
    unsigned char itemClass;
    unsigned short itemType;
    unsigned int durability;
    int grade;
    int enchantLevel;
    unsigned int itemNum;
    int inventoryFrameID;
    int gridW;
    int gridH;
    std::string itemName;
    std::unique_ptr<MItem> previewItem;

    SelectedAuctionItem(const SelectedAuctionItem&) = delete;
    SelectedAuctionItem& operator=(const SelectedAuctionItem&) = delete;
    SelectedAuctionItem(SelectedAuctionItem&&) = default;
    SelectedAuctionItem& operator=(SelectedAuctionItem&&) = default;

    SelectedAuctionItem()
        : valid(false)
        , objectID(0)
        , itemClass(0)
        , itemType(0)
        , durability(0)
        , grade(0)
        , enchantLevel(0)
        , itemNum(0)
        , inventoryFrameID(0)
        , gridW(1)
        , gridH(1)
    {
    }
};

bool g_visible = false;
unsigned char g_lastResultCode = 0;
std::string g_lastResultMessage;
std::vector<AuctionEntry> g_entries;
std::map<int, ItemTexture*> g_textureCache;
int g_listPrice = 100000;
bool g_pickMode = false;
SelectedAuctionItem g_selectedItem;
MItem* g_legacyTooltipItem = NULL;
int g_legacyTooltipX = 0;
int g_legacyTooltipY = 0;

ItemTexture* buildItemTexture(IDirect3DDevice9* device, int frameID)
{
    if (device == NULL || frameID < 0)
        return NULL;

    int width = 0;
    int height = 0;
    if (!PhoenixGetItemSpritePreviewSize((WORD)frameID, &width, &height))
        return NULL;

    if (width <= 0 || height <= 0 || width > 512 || height > 512)
        return NULL;

    IDirect3DTexture9* texture = NULL;
    HRESULT hr = device->CreateTexture(
        (UINT)width,
        (UINT)height,
        1,
        0,
        D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED,
        &texture,
        NULL);
    if (FAILED(hr) || texture == NULL)
        return NULL;

    D3DLOCKED_RECT locked;
    hr = texture->LockRect(0, &locked, NULL, 0);
    if (FAILED(hr))
    {
        texture->Release();
        return NULL;
    }

    DWORD* dst = (DWORD*)locked.pBits;
    int dstPitchPixels = locked.Pitch / (int)sizeof(DWORD);
    if (!PhoenixCopyItemSpritePreviewArgb((WORD)frameID, dst, dstPitchPixels, width, height))
    {
        texture->UnlockRect(0);
        texture->Release();
        return NULL;
    }

    texture->UnlockRect(0);

    ItemTexture* itemTexture = new ItemTexture();
    itemTexture->texture = texture;
    itemTexture->width = width;
    itemTexture->height = height;
    return itemTexture;
}

ItemTexture* getItemTexture(IDirect3DDevice9* device, int frameID)
{
    std::map<int, ItemTexture*>::iterator itr = g_textureCache.find(frameID);
    if (itr != g_textureCache.end())
        return itr->second;

    ItemTexture* texture = buildItemTexture(device, frameID);
    if (texture != NULL)
        g_textureCache[frameID] = texture;
    return texture;
}

void clearSelectedAuctionItem()
{
    g_selectedItem = SelectedAuctionItem();
    g_pickMode = false;
}

std::string raceName(unsigned char race)
{
    switch (race)
    {
        case 0: return "Slayer";
        case 1: return "Vampire";
        case 2: return "Ousters";
        default: return "Unknown";
    }
}

void sendRequestList()
{
    if (g_pSocket == NULL)
        return;
    CGAuctionRequestList packet;
    g_pSocket->sendPacket(&packet);
}

void sendBuy(unsigned int listingID)
{
    if (g_pSocket == NULL)
        return;
    CGAuctionBuy packet;
    packet.setListingID(listingID);
    g_pSocket->sendPacket(&packet);
}

void sendCancel(unsigned int listingID)
{
    if (g_pSocket == NULL)
        return;
    CGAuctionCancel packet;
    packet.setListingID(listingID);
    g_pSocket->sendPacket(&packet);
}

void sendListItem()
{
    if (g_pSocket == NULL)
        return;
    if (!g_selectedItem.valid || g_selectedItem.objectID == 0)
    {
        g_lastResultCode = AUCTION_RESULT_ERROR;
        g_lastResultMessage = "Select an inventory item first.";
        return;
    }
    if (g_listPrice <= 0)
    {
        g_lastResultCode = AUCTION_RESULT_ERROR;
        g_lastResultMessage = "Enter a valid price.";
        return;
    }

    CGAuctionListItem packet;
    packet.setObjectID((ObjectID_t)g_selectedItem.objectID);
    packet.setPrice((Gold_t)g_listPrice);
    g_pSocket->sendPacket(&packet);
}

void clearLegacyTooltip()
{
    g_legacyTooltipItem = NULL;
    g_legacyTooltipX = 0;
    g_legacyTooltipY = 0;
}

void queueLegacyTooltip(MItem* pItem, const ImVec2& itemMin, const ImVec2& itemSize)
{
    if (pItem == NULL)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    float x = windowPos.x + windowSize.x + 10.0f;
    if (x > io.DisplaySize.x - 260.0f)
        x = windowPos.x - 270.0f;
    if (x < 4.0f)
        x = itemMin.x + itemSize.x + 8.0f;

    float y = itemMin.y;
    if (y > io.DisplaySize.y - 120.0f)
        y = io.DisplaySize.y - 120.0f;
    if (y < 4.0f)
        y = 4.0f;

    g_legacyTooltipItem = pItem;
    g_legacyTooltipX = (int)x;
    g_legacyTooltipY = (int)y;
}

bool setGunMagazineFromCount(MItem* pItem, TYPE_ITEM_NUMBER itemNum)
{
    if (pItem == NULL || !pItem->IsGunItem())
        return false;

    MMagazine* pMagazine = dynamic_cast<MMagazine*>(MItem::NewItem((ITEM_CLASS)ITEM_CLASS_MAGAZINE));
    if (pMagazine == NULL)
    {
        OutputDebugStringA("[PhoenixAuctionHouseHUD] failed to create magazine preview item.\n");
        return false;
    }

    pMagazine->SetID(0);
    for (int j = 0; j < (*g_pItemTable)[ITEM_CLASS_MAGAZINE].GetSize(); ++j)
    {
        pMagazine->SetItemType(j);
        if (pMagazine->IsInsertToItem(pItem))
            break;
    }
    pMagazine->ClearItemOption();
    pMagazine->SetNumber(itemNum);

    MGunItem* pGunItem = dynamic_cast<MGunItem*>(pItem);
    if (pGunItem == NULL)
    {
        delete pMagazine;
        OutputDebugStringA("[PhoenixAuctionHouseHUD] gun preview cast failed.\n");
        return false;
    }

    pGunItem->SetMagazine(pMagazine);
    return true;
}

void applySubItemsForPreview(MItem* pItem, PCItemInfo& itemInfo)
{
    if (pItem == NULL || itemInfo.getListNum() == 0)
        return;

    if (pItem->GetItemClass() == ITEM_CLASS_BELT)
    {
        MBelt* pBelt = dynamic_cast<MBelt*>(pItem);
        if (pBelt == NULL)
            return;

        int size = itemInfo.getListNum();
        for (int i = 0; i < size; ++i)
        {
            SubItemInfo* pSubItemInfo = itemInfo.popFrontListElement();
            if (pSubItemInfo == NULL)
                continue;

            MItem* pSubItem = MItem::NewItem((enum ITEM_CLASS)pSubItemInfo->getItemClass());
            if (pSubItem != NULL)
            {
                pSubItem->SetItemType(pSubItemInfo->getItemType());
                pSubItem->SetID(pSubItemInfo->getObjectID());
                pSubItem->SetNumber(pSubItemInfo->getItemNum());
                pBelt->AddItem(pSubItem, pSubItemInfo->getSlotID());
            }
            delete pSubItemInfo;
        }
    }
    else if (pItem->GetItemClass() == ITEM_CLASS_OUSTERS_ARMSBAND)
    {
        MOustersArmsBand* pArmsBand = dynamic_cast<MOustersArmsBand*>(pItem);
        if (pArmsBand == NULL)
            return;

        int size = itemInfo.getListNum();
        for (int i = 0; i < size; ++i)
        {
            SubItemInfo* pSubItemInfo = itemInfo.popFrontListElement();
            if (pSubItemInfo == NULL)
                continue;

            MItem* pSubItem = MItem::NewItem((enum ITEM_CLASS)pSubItemInfo->getItemClass());
            if (pSubItem != NULL)
            {
                pSubItem->SetItemType(pSubItemInfo->getItemType());
                pSubItem->SetID(pSubItemInfo->getObjectID());
                pSubItem->SetNumber(pSubItemInfo->getItemNum());
                pArmsBand->AddItem(pSubItem, pSubItemInfo->getSlotID());
            }
            delete pSubItemInfo;
        }
    }
}

std::unique_ptr<MItem> buildAuctionPreviewItemFromPCItemInfo(PCItemInfo& itemInfo)
{
    BYTE itemClass = itemInfo.getItemClass();
    if (itemClass >= MAX_ITEM_CLASS)
        return std::unique_ptr<MItem>();

    std::unique_ptr<MItem> pItem(MItem::NewItem((ITEM_CLASS)itemClass));
    if (!pItem)
        return std::unique_ptr<MItem>();

    pItem->SetItemType(itemInfo.getItemType());
    pItem->SetID(itemInfo.getObjectID());
    pItem->SetItemOptionList(itemInfo.getOptionType());
    pItem->SetItemGTwoOptionList(itemInfo.getGTwoOptionType());
#if __CONTENTS(__PET_MIXINGFORGE)
    pItem->SetPetMixingOptionList(itemInfo.getMixOptionType());
#endif
    pItem->SetCurrentDurability(itemInfo.getDurability());
    pItem->SetSilver(itemInfo.getSilver());
    pItem->SetGrade(itemInfo.getGrade());
    pItem->SetItemThirdOptionList(itemInfo.getThirdOptionType());
    pItem->SetItemThirdOptionType(itemInfo.getThirdEnchantType());
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
    pItem->SetCashItem(itemInfo.getCashItem());
#endif

    if (pItem->IsGunItem())
    {
        if (!setGunMagazineFromCount(pItem.get(), itemInfo.getItemNum()))
            return std::unique_ptr<MItem>();
    }
    else
    {
        pItem->SetNumber(itemInfo.getItemNum());
    }

    pItem->SetEnchantLevel(itemInfo.getEnchantLevel());
    applySubItemsForPreview(pItem.get(), itemInfo);
    return pItem;
}

std::unique_ptr<MItem> cloneAuctionPreviewItem(const MItem* pSource)
{
    if (pSource == NULL || pSource->GetItemClass() >= MAX_ITEM_CLASS)
        return std::unique_ptr<MItem>();

    std::unique_ptr<MItem> pItem(MItem::NewItem(pSource->GetItemClass()));
    if (!pItem)
        return std::unique_ptr<MItem>();

    MItem* pMutableSource = const_cast<MItem*>(pSource);

    pItem->SetItemType(pSource->GetItemType());
    pItem->SetID(pSource->GetID());
    pItem->SetItemOptionList(pSource->GetItemOptionList());
    pItem->SetItemGTwoOptionList(pSource->GetItemGTwoOptionList());
#if __CONTENTS(__PET_MIXINGFORGE)
    pItem->SetPetMixingOptionList(pSource->GetPetMixingOptionList());
#endif
    pItem->SetCurrentDurability(pSource->GetCurrentDurability());
    pItem->SetSilver(pSource->GetSilver());
    pItem->SetGrade(pSource->GetGrade());
    pItem->SetItemThirdOptionList(pSource->GetItemThirdOptionList());
    pItem->SetItemThirdOptionType(pMutableSource->GetItemThirdOptionType());
#if __CONTENTS(__INTERNATIONAL_PREMIUM_SYSTEM)
    pItem->SetCashItem(pSource->GetCashItem());
#endif

    if (pSource->IsGunItem())
    {
        const MGunItem* pSourceGun = dynamic_cast<const MGunItem*>(pSource);
        const MMagazine* pSourceMagazine = (pSourceGun != NULL) ? pSourceGun->GetMagazine() : NULL;
        if (pSourceMagazine != NULL)
            setGunMagazineFromCount(pItem.get(), pSourceMagazine->GetNumber());
        else
            setGunMagazineFromCount(pItem.get(), pSource->GetNumber());
    }
    else
    {
        pItem->SetNumber(pSource->GetNumber());
    }

    pItem->SetEnchantLevel(pSource->GetEnchantLevel());
    return pItem;
}

bool drawFrameIconOrFallback(IDirect3DDevice9* device, int frameID, const ImVec2& slotSize, const char* fallbackText = NULL)
{
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(pos, ImVec2(pos.x + slotSize.x, pos.y + slotSize.y), IM_COL32(9, 19, 22, 230), 3.0f);
    drawList->AddRect(pos, ImVec2(pos.x + slotSize.x, pos.y + slotSize.y), IM_COL32(40, 170, 210, 220), 3.0f, 0, 1.0f);

    ItemTexture* texture = getItemTexture(device, frameID);
    if (texture != NULL && texture->texture != NULL)
    {
        float maxW = slotSize.x - 8.0f;
        float maxH = slotSize.y - 8.0f;
        float scale = maxW / (float)texture->width;
        float scaleH = maxH / (float)texture->height;
        if (scaleH < scale)
            scale = scaleH;
        if (scale > 1.0f)
            scale = 1.0f;
        float w = texture->width * scale;
        float h = texture->height * scale;
        ImVec2 imgMin(pos.x + (slotSize.x - w) * 0.5f, pos.y + (slotSize.y - h) * 0.5f);
        ImVec2 imgMax(imgMin.x + w, imgMin.y + h);
        drawList->AddImage((ImTextureID)(uintptr_t)texture->texture, imgMin, imgMax);
    }
    else
    {
        char text[48];
        if (fallbackText != NULL)
        {
            std::snprintf(text, sizeof(text), "%s", fallbackText);
            ImVec2 textSize = ImGui::CalcTextSize(text);
            drawList->AddText(
                ImVec2(pos.x + (slotSize.x - textSize.x) * 0.5f, pos.y + (slotSize.y - textSize.y) * 0.5f),
                IM_COL32(170, 210, 220, 255),
                text);
        }
    }

    ImGui::InvisibleButton("##auction_icon", slotSize);
    return ImGui::IsItemHovered();
}

bool drawIconOrFallback(IDirect3DDevice9* device, const AuctionEntry& entry, const ImVec2& slotSize)
{
    return drawFrameIconOrFallback(device, entry.inventoryFrameID, slotSize);
}

void drawEntry(IDirect3DDevice9* device, const AuctionEntry& entry)
{
    ImGui::PushID((int)entry.listingID);
    ImGui::BeginGroup();

    bool iconHovered = drawIconOrFallback(device, entry, ImVec2(72.0f, 72.0f));
    ImVec2 iconMin = ImGui::GetItemRectMin();
    ImVec2 iconSize = ImGui::GetItemRectSize();

    ImGui::TextWrapped("%s", entry.itemName.c_str());
    ImGui::TextColored(ImVec4(0.90f, 0.78f, 0.24f, 1.0f), "%u Gold", entry.price);

    if (ImGui::Button("Buy", ImVec2(54.0f, 0.0f)))
        sendBuy(entry.listingID);
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(64.0f, 0.0f)))
        sendCancel(entry.listingID);

    if (iconHovered && entry.previewItem.get() != NULL)
        queueLegacyTooltip(entry.previewItem.get(), iconMin, iconSize);

    ImGui::EndGroup();
    ImGui::PopID();
}

AuctionEntry makeEntry(const GCAuctionList::Entry* pPacketEntry)
{
    AuctionEntry entry;
    if (pPacketEntry == NULL)
        return entry;

    PCItemInfo& itemInfo = const_cast<PCItemInfo&>(pPacketEntry->ItemInfo);
    entry.listingID = pPacketEntry->ListingID;
    entry.price = pPacketEntry->Price;
    entry.sellerRace = pPacketEntry->SellerRace;
    entry.itemRace = pPacketEntry->ItemRace;
    entry.sellerName = pPacketEntry->SellerName;
    entry.itemClass = itemInfo.getItemClass();
    entry.itemType = itemInfo.getItemType();
    entry.objectID = itemInfo.getObjectID();
    entry.durability = itemInfo.getDurability();
    entry.grade = itemInfo.getGrade();
    entry.enchantLevel = itemInfo.getEnchantLevel();
    entry.itemNum = itemInfo.getItemNum();
    entry.previewItem = buildAuctionPreviewItemFromPCItemInfo(itemInfo);

    if (entry.previewItem.get() != NULL)
    {
        entry.itemName = entry.previewItem->GetName();
        entry.gridW = entry.previewItem->GetGridWidth();
        entry.gridH = entry.previewItem->GetGridHeight();
        entry.inventoryFrameID = entry.previewItem->GetInventoryFrameID();
    }

    if (entry.itemName.empty())
    {
        char fallback[64];
        std::snprintf(fallback, sizeof(fallback), "Item %u/%u", (unsigned int)entry.itemClass, (unsigned int)entry.itemType);
        entry.itemName = fallback;
    }

    return entry;
}

} // namespace

bool IsAuctionItemPickModeActive()
{
    return g_visible && g_pickMode;
}

bool HandleAuctionInventoryItemClick(const MItem* pItem)
{
    if (!IsAuctionItemPickModeActive())
        return false;

    if (pItem == NULL)
    {
        g_lastResultCode = AUCTION_RESULT_ERROR;
        g_lastResultMessage = "Click an inventory item to select it.";
        return true;
    }

    const MItem* pConstItem = pItem;
    MItem* pMutableItem = const_cast<MItem*>(pConstItem);
    std::unique_ptr<MItem> previewItem = cloneAuctionPreviewItem(pConstItem);
    if (!previewItem)
    {
        g_lastResultCode = AUCTION_RESULT_ERROR;
        g_lastResultMessage = "Selected item cannot be previewed.";
        return true;
    }

    g_selectedItem.valid = true;
    g_selectedItem.objectID = pConstItem->GetID();
    g_selectedItem.itemClass = (unsigned char)pConstItem->GetItemClass();
    g_selectedItem.itemType = pConstItem->GetItemType();
    g_selectedItem.durability = pConstItem->GetCurrentDurability();
    g_selectedItem.grade = pConstItem->GetGrade();
    g_selectedItem.enchantLevel = pConstItem->GetEnchantLevel();
    g_selectedItem.itemNum = pConstItem->GetNumber();
    g_selectedItem.inventoryFrameID = pConstItem->GetInventoryFrameID();
    g_selectedItem.gridW = pConstItem->GetGridWidth();
    g_selectedItem.gridH = pConstItem->GetGridHeight();
    g_selectedItem.itemName = pMutableItem->GetName();
    g_selectedItem.previewItem = std::move(previewItem);
    if (g_selectedItem.itemName.empty())
    {
        char fallback[64];
        std::snprintf(fallback, sizeof(fallback), "Item %u/%u",
            (unsigned int)g_selectedItem.itemClass,
            (unsigned int)g_selectedItem.itemType);
        g_selectedItem.itemName = fallback;
    }

    g_pickMode = false;
    g_lastResultCode = AUCTION_RESULT_OK;
    g_lastResultMessage = "Item selected. Enter price and list it.";
    return true;
}

void UpdateAuctionHouseList(const GCAuctionList& packet)
{
    clearLegacyTooltip();
    g_entries.clear();

    const std::list<GCAuctionList::Entry*>& entries = packet.getEntries();
    for (std::list<GCAuctionList::Entry*>::const_iterator itr = entries.begin(); itr != entries.end(); ++itr)
        g_entries.push_back(makeEntry(*itr));

    g_visible = true;
}

void UpdateAuctionHouseResult(unsigned char resultCode, const std::string& message)
{
    g_lastResultCode = resultCode;
    g_lastResultMessage = message;
    g_visible = true;

    if (resultCode == AUCTION_RESULT_LISTED ||
        (resultCode == AUCTION_RESULT_ERROR &&
            (message.find("not in inventory") != std::string::npos ||
             message.find("already listed") != std::string::npos)))
    {
        clearSelectedAuctionItem();
    }

    if (resultCode == AUCTION_RESULT_LISTED ||
        resultCode == AUCTION_RESULT_BOUGHT ||
        resultCode == AUCTION_RESULT_CANCELLED)
    {
        sendRequestList();
    }
}

void DrawAuctionHouseHUD(IDirect3DDevice9* device)
{
    clearLegacyTooltip();

    if (!g_visible)
    {
        g_pickMode = false;
        return;
    }
    if (g_Mode != MODE_GAME)
    {
        g_pickMode = false;
        return;
    }

    if (g_pickMode && ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        g_pickMode = false;
        g_lastResultCode = AUCTION_RESULT_OK;
        g_lastResultMessage = "Auction item selection cancelled.";
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 310.0f, 110.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(620.0f, 520.0f), ImGuiCond_FirstUseEver);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.02f, 0.04f, 0.05f, 0.92f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.18f, 0.70f, 0.82f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.02f, 0.14f, 0.18f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.04f, 0.22f, 0.28f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    if (ImGui::Begin("Auction House", &g_visible, ImGuiWindowFlags_NoCollapse))
    {
        if (!g_lastResultMessage.empty())
        {
            ImVec4 color = (g_lastResultCode == AUCTION_RESULT_ERROR)
                ? ImVec4(1.0f, 0.32f, 0.28f, 1.0f)
                : ImVec4(0.48f, 0.92f, 0.72f, 1.0f);
            ImGui::TextColored(color, "%s", g_lastResultMessage.c_str());
            ImGui::Separator();
        }

        if (ImGui::Button("Refresh"))
            sendRequestList();

        ImGui::SameLine();
        ImGui::TextDisabled("Max 20 listed items, 3 columns");

        ImGui::Separator();

        ImGui::BeginChild("##auction_list", ImVec2(0.0f, 270.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        const int columns = 3;
        const float cellW = 180.0f;
        for (size_t i = 0; i < g_entries.size(); ++i)
        {
            if (i > 0 && (i % columns) != 0)
                ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::PushItemWidth(cellW);
            drawEntry(device, g_entries[i]);
            ImGui::PopItemWidth();
            ImGui::EndGroup();
        }

        if (g_entries.empty())
            ImGui::TextDisabled("No auction listing available for your race.");

        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Text("Sell inventory item");

        ImGui::BeginGroup();
        ImGui::PushID("selected_sell_item");
        bool selectedIconHovered = drawFrameIconOrFallback(
            device,
            g_selectedItem.valid ? g_selectedItem.inventoryFrameID : -1,
            ImVec2(56.0f, 56.0f),
            g_selectedItem.valid ? NULL : "Empty");
        ImVec2 selectedIconMin = ImGui::GetItemRectMin();
        ImVec2 selectedIconSize = ImGui::GetItemRectSize();
        if (selectedIconHovered && g_selectedItem.previewItem.get() != NULL)
            queueLegacyTooltip(g_selectedItem.previewItem.get(), selectedIconMin, selectedIconSize);
        ImGui::PopID();
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::BeginGroup();
        if (g_selectedItem.valid)
        {
            ImGui::TextWrapped("%s", g_selectedItem.itemName.c_str());
            ImGui::TextDisabled("Grid %dx%d | Grade %d | Enchant %d",
                g_selectedItem.gridW,
                g_selectedItem.gridH,
                g_selectedItem.grade,
                g_selectedItem.enchantLevel);
        }
        else if (g_pickMode)
        {
            ImGui::TextColored(ImVec4(0.94f, 0.78f, 0.28f, 1.0f), "Click an item in your inventory.");
        }
        else
        {
            ImGui::TextDisabled("No item selected.");
        }

        if (ImGui::Button(g_pickMode ? "Cancel Selection" : "Select Item", ImVec2(118.0f, 0.0f)))
        {
            if (g_pickMode)
            {
                g_pickMode = false;
                g_lastResultCode = AUCTION_RESULT_OK;
                g_lastResultMessage = "Auction item selection cancelled.";
            }
            else
            {
                g_pickMode = true;
                g_lastResultCode = AUCTION_RESULT_OK;
                g_lastResultMessage = "Click an inventory item to select it.";
            }
        }
        ImGui::SameLine();
        if (g_selectedItem.valid && ImGui::Button("Clear", ImVec2(64.0f, 0.0f)))
            clearSelectedAuctionItem();

        ImGui::PushItemWidth(120.0f);
        ImGui::InputInt("Price", &g_listPrice);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::BeginDisabled(!g_selectedItem.valid);
        if (ImGui::Button("List Item", ImVec2(92.0f, 0.0f)))
            sendListItem();
        ImGui::EndDisabled();
        ImGui::EndGroup();
    }
    ImGui::End();

    if (!g_visible)
        g_pickMode = false;

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
}

void PrepareAuctionHouseLegacyTooltip()
{
    if (!g_visible || g_legacyTooltipItem == NULL)
        return;

    PhoenixSetAuctionHouseItemDescriptor(g_legacyTooltipX, g_legacyTooltipY, g_legacyTooltipItem);
}

void ShutdownAuctionHouseHUD()
{
    clearLegacyTooltip();
    clearSelectedAuctionItem();

    for (std::map<int, ItemTexture*>::iterator itr = g_textureCache.begin(); itr != g_textureCache.end(); ++itr)
    {
        if (itr->second != NULL)
        {
            if (itr->second->texture != NULL)
                itr->second->texture->Release();
            delete itr->second;
        }
    }
    g_textureCache.clear();
}

} // namespace PhoenixImGui
