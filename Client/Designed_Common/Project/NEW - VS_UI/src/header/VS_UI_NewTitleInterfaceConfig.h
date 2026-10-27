#ifndef __VS_UI_NEW_TITLE_INTERFACE_CONFIG_H__
#define __VS_UI_NEW_TITLE_INTERFACE_CONFIG_H__

#include <windows.h>

enum PhoenixTitleSlotIndex
{
	PHOENIX_TITLE_SLOT_LEFT = 0,
	PHOENIX_TITLE_SLOT_CENTER,
	PHOENIX_TITLE_SLOT_RIGHT,
	PHOENIX_TITLE_SLOT_COUNT,
};

enum PhoenixTitleTextLineIndex
{
	PHOENIX_TITLE_LINE1 = 0,
	PHOENIX_TITLE_LINE2,
	PHOENIX_TITLE_LINE3,
	PHOENIX_TITLE_LINE_COUNT,
};

const int PHOENIX_CHAR_MANAGER_LOCKED_SLOT_COUNT = 2;
const int PHOENIX_CHAR_MANAGER_SLOT_BUTTON_X_OFFSET = 24;
const int PHOENIX_CHAR_MANAGER_SLOT_LEFT_Y_OFFSET = 84;
const int PHOENIX_CHAR_MANAGER_SLOT_CENTER_Y_OFFSET = 164;
const int PHOENIX_CHAR_MANAGER_SLOT_RIGHT_Y_OFFSET = 244;
const int PHOENIX_CHAR_MANAGER_CREATE_BUTTON_X_OFFSET = 46;
const int PHOENIX_CHAR_MANAGER_CREATE_BUTTON_Y_OFFSET = 491;
const int PHOENIX_CHAR_MANAGER_DELETE_BUTTON_X_OFFSET = 191;
const int PHOENIX_CHAR_MANAGER_DELETE_BUTTON_Y_OFFSET = 491;
const int PHOENIX_CHAR_MANAGER_LOCKED_SLOT_X_OFFSET = 24;
const int PHOENIX_CHAR_MANAGER_LOCKED_SLOT1_Y_OFFSET = 80;
const int PHOENIX_CHAR_MANAGER_LOCKED_SLOT2_Y_OFFSET = 160;

struct PhoenixTitleCharManagerConfig
{
	char section[255];

	WORD charInfoAngleX;
	WORD charInfoAngleY;
	WORD slayerCharInfoAngleX;
	WORD slayerCharInfoAngleY;

	POINT charPosition[PHOENIX_TITLE_SLOT_COUNT];
	POINT charSelectButton[PHOENIX_TITLE_SLOT_COUNT];
	POINT lockedSlot[PHOENIX_CHAR_MANAGER_LOCKED_SLOT_COUNT];
	WORD charSelectButtonWidth;
	WORD charSelectButtonHeight;

	POINT charNameBar[PHOENIX_TITLE_SLOT_COUNT];
	POINT charName[PHOENIX_TITLE_SLOT_COUNT];
	WORD charNameBarWidth;

	WORD prevButtonX;
	WORD prevButtonY;
	WORD gameStartButtonX;
	WORD gameStartButtonY;
	WORD charCreateButtonX;
	WORD charCreateButtonY;
	WORD charDeleteButtonX;
	WORD charDeleteButtonY;

	WORD effectFireLeft1X;
	WORD effectFireLeft1Y;
	WORD effectFireLeft2X;
	WORD effectFireLeft2Y;
	WORD effectFireRight1X;
	WORD effectFireRight1Y;
	WORD effectFireRight2X;
	WORD effectFireRight2Y;
	WORD effectFireCenterX;
	WORD effectFireCenterY;

	POINT effectCharAttachUp[PHOENIX_TITLE_SLOT_COUNT];
	POINT effectCharAttachDown[PHOENIX_TITLE_SLOT_COUNT];
	POINT effectCharSelect[PHOENIX_TITLE_SLOT_COUNT];

	WORD childX;
	WORD childY;
	WORD adultX;
	WORD adultY;
	WORD violenceX;
	WORD violenceY;
	WORD fearX;
	WORD fearY;
	WORD textBarX;
	WORD textBarY;
	WORD aniMarkX;
	WORD aniMarkY;
	POINT textLine[PHOENIX_TITLE_LINE_COUNT];
	char textLineString[PHOENIX_TITLE_LINE_COUNT][256];

	WORD premiumUseDayTextX;
	WORD premiumUseDayTextY;
	WORD warningStringX;
	WORD warningStringY;

	WORD panelX;
	WORD panelY;
	WORD listInfoX;
	WORD listInfoY;
	WORD charInfoX;
	WORD charInfoY;
	WORD slayerCharInfoX;
	WORD slayerCharInfoY;
};

void PhoenixReadCharManagerInterfaceConfig(
	PhoenixTitleCharManagerConfig& outConfig,
	const char* section,
	const char* fileName);

POINT PhoenixCharManagerLockedSlotPoint(
	WORD panelX,
	WORD rightSlotButtonY,
	int lockedSlotIndex);

#endif // __VS_UI_NEW_TITLE_INTERFACE_CONFIG_H__
