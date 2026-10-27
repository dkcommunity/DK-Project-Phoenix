//----------------------------------------------------------------------
// PhoenixAuctionHouseHUD.h
//----------------------------------------------------------------------

#ifndef _PHOENIX_AUCTION_HOUSE_HUD_H_
#define _PHOENIX_AUCTION_HOUSE_HUD_H_

#include <string>

struct IDirect3DDevice9;
class GCAuctionList;
class MItem;

namespace PhoenixImGui {

void UpdateAuctionHouseList(const GCAuctionList& packet);
void UpdateAuctionHouseResult(unsigned char resultCode, const std::string& message);
void DrawAuctionHouseHUD(IDirect3DDevice9* device);
void ShutdownAuctionHouseHUD();
void PrepareAuctionHouseLegacyTooltip();

bool IsAuctionItemPickModeActive();
bool HandleAuctionInventoryItemClick(const MItem* pItem);

} // namespace PhoenixImGui

#endif // _PHOENIX_AUCTION_HOUSE_HUD_H_
