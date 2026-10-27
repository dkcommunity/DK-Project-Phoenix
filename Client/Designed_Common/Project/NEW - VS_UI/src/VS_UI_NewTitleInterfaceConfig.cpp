#include "client_PCH.h"
#include "VS_UI_NewTitleInterfaceConfig.h"

#include <stdlib.h>
#include <string.h>

namespace
{
	bool ReadProfileIntIfExists(const char* section, const char* key, const char* fileName, int& outValue)
	{
		char value[64] = {0,};
		DWORD length = GetPrivateProfileStringA(section, key, "", value, sizeof(value), fileName);
		if(length == 0)
			return false;

		outValue = atoi(value);
		return true;
	}

	int ReadProfileIntAlias(const char* section, const char* primaryKey, const char* legacyKey, int defaultValue, const char* fileName)
	{
		int value = 0;
		if(ReadProfileIntIfExists(section, primaryKey, fileName, value))
			return value;
		if(legacyKey != NULL && ReadProfileIntIfExists(section, legacyKey, fileName, value))
			return value;
		return defaultValue;
	}

	WORD ReadWord(const char* section, const char* key, int defaultValue, const char* fileName)
	{
		return (WORD)GetPrivateProfileIntA(section, key, defaultValue, fileName);
	}

	WORD ReadWordAlias(const char* section, const char* primaryKey, const char* legacyKey, int defaultValue, const char* fileName)
	{
		return (WORD)ReadProfileIntAlias(section, primaryKey, legacyKey, defaultValue, fileName);
	}

	void ReadPoint(POINT& point, const char* section, const char* xKey, const char* yKey, int defaultX, int defaultY, const char* fileName)
	{
		point.x = ReadWord(section, xKey, defaultX, fileName);
		point.y = ReadWord(section, yKey, defaultY, fileName);
	}

}

void PhoenixReadCharManagerInterfaceConfig(
	PhoenixTitleCharManagerConfig& outConfig,
	const char* section,
	const char* fileName)
{
	memset(&outConfig, 0, sizeof(outConfig));
	if(section == NULL || fileName == NULL)
		return;

	strncpy(outConfig.section, section, sizeof(outConfig.section) - 1);
	outConfig.section[sizeof(outConfig.section) - 1] = 0;

	outConfig.charInfoAngleX = ReadWord(section, "CHAR_INFO_ANGLE_X", 0, fileName);
	outConfig.charInfoAngleY = ReadWord(section, "CHAR_INFO_ANGLE_Y", 0, fileName);
	outConfig.slayerCharInfoAngleX = ReadWord(section, "SLAYER_CHAR_INFO_ANGLE_X", 0, fileName);
	outConfig.slayerCharInfoAngleY = ReadWord(section, "SLAYER_CHAR_INFO_ANGLE_Y", 0, fileName);

	ReadPoint(outConfig.charPosition[PHOENIX_TITLE_SLOT_LEFT], section, "CHAR_LEFT_X", "CHAR_LEFT_Y", 0, 0, fileName);
	ReadPoint(outConfig.charPosition[PHOENIX_TITLE_SLOT_CENTER], section, "CHAR_CENTER_X", "CHAR_CENTER_Y", 0, 0, fileName);
	ReadPoint(outConfig.charPosition[PHOENIX_TITLE_SLOT_RIGHT], section, "CHAR_RIGHT_X", "CHAR_RIGHT_Y", 0, 0, fileName);

	outConfig.charSelectButtonWidth = ReadWordAlias(section, "CHAR_SELECT_BUTTON_WIDTH", "CHAR_SELECT_BUTTON_WIDHT", 0, fileName);
	outConfig.charSelectButtonHeight = ReadWord(section, "CHAR_SELECT_BUTTON_HEIGHT", 0, fileName);

	ReadPoint(outConfig.charNameBar[PHOENIX_TITLE_SLOT_LEFT], section, "CHAR_NAME_BAR_LEFT_X", "CHAR_NAME_BAR_LEFT_Y", 0, 0, fileName);
	ReadPoint(outConfig.charNameBar[PHOENIX_TITLE_SLOT_CENTER], section, "CHAR_NAME_BAR_CENTER_X", "CHAR_NAME_BAR_CENTER_Y", 0, 0, fileName);
	ReadPoint(outConfig.charNameBar[PHOENIX_TITLE_SLOT_RIGHT], section, "CHAR_NAME_BAR_RIGHT_X", "CHAR_NAME_BAR_RIGHT_Y", 0, 0, fileName);

	ReadPoint(outConfig.charName[PHOENIX_TITLE_SLOT_LEFT], section, "CHAR_NAME_LEFT_X", "CHAR_NAME_LEFT_Y",
		outConfig.charNameBar[PHOENIX_TITLE_SLOT_LEFT].x, outConfig.charNameBar[PHOENIX_TITLE_SLOT_LEFT].y, fileName);
	ReadPoint(outConfig.charName[PHOENIX_TITLE_SLOT_CENTER], section, "CHAR_NAME_CENTER_X", "CHAR_NAME_CENTER_Y",
		outConfig.charNameBar[PHOENIX_TITLE_SLOT_CENTER].x, outConfig.charNameBar[PHOENIX_TITLE_SLOT_CENTER].y, fileName);
	ReadPoint(outConfig.charName[PHOENIX_TITLE_SLOT_RIGHT], section, "CHAR_NAME_RIGHT_X", "CHAR_NAME_RIGHT_Y",
		outConfig.charNameBar[PHOENIX_TITLE_SLOT_RIGHT].x, outConfig.charNameBar[PHOENIX_TITLE_SLOT_RIGHT].y, fileName);
	outConfig.charNameBarWidth = ReadWordAlias(section, "CHAR_NAME_BAR_WIDTH", "CHAR_NAME_BAR_WIDHT", 0, fileName);

	outConfig.prevButtonX = ReadWordAlias(section, "PREV_BUTTON_X", "PREVE_BUTTON_X", 0, fileName);
	outConfig.prevButtonY = ReadWordAlias(section, "PREV_BUTTON_Y", "PREVE_BUTTON_Y", 0, fileName);
	outConfig.gameStartButtonX = ReadWord(section, "GAME_START_BUTTON_X", 0, fileName);
	outConfig.gameStartButtonY = ReadWord(section, "GAME_START_BUTTON_Y", 0, fileName);

	outConfig.panelX = ReadWord(section, "m_wPanel_X", 0, fileName);
	outConfig.panelY = ReadWord(section, "m_wPanel_Y", 0, fileName);
	outConfig.listInfoX = ReadWord(section, "m_wListInfo_X", 0, fileName);
	outConfig.listInfoY = ReadWord(section, "m_wListInfo_Y", 0, fileName);

	ReadPoint(outConfig.charSelectButton[PHOENIX_TITLE_SLOT_LEFT], section, "CHAR_SELECT_LEFT_BUTTON_X", "CHAR_SELECT_LEFT_BUTTON_Y",
		outConfig.panelX + PHOENIX_CHAR_MANAGER_SLOT_BUTTON_X_OFFSET,
		outConfig.panelY + PHOENIX_CHAR_MANAGER_SLOT_LEFT_Y_OFFSET, fileName);
	ReadPoint(outConfig.charSelectButton[PHOENIX_TITLE_SLOT_CENTER], section, "CHAR_SELECT_CENTER_BUTTON_X", "CHAR_SELECT_CENTER_BUTTON_Y",
		outConfig.panelX + PHOENIX_CHAR_MANAGER_SLOT_BUTTON_X_OFFSET,
		outConfig.panelY + PHOENIX_CHAR_MANAGER_SLOT_CENTER_Y_OFFSET, fileName);
	ReadPoint(outConfig.charSelectButton[PHOENIX_TITLE_SLOT_RIGHT], section, "CHAR_SELECT_RIGHT_BUTTON_X", "CHAR_SELECT_RIGHT_BUTTON_Y",
		outConfig.panelX + PHOENIX_CHAR_MANAGER_SLOT_BUTTON_X_OFFSET,
		outConfig.panelY + PHOENIX_CHAR_MANAGER_SLOT_RIGHT_Y_OFFSET, fileName);

	POINT defaultLockedSlot1 = PhoenixCharManagerLockedSlotPoint(
		outConfig.panelX,
		(WORD)outConfig.charSelectButton[PHOENIX_TITLE_SLOT_RIGHT].y,
		0);
	POINT defaultLockedSlot2 = PhoenixCharManagerLockedSlotPoint(
		outConfig.panelX,
		(WORD)outConfig.charSelectButton[PHOENIX_TITLE_SLOT_RIGHT].y,
		1);
	ReadPoint(outConfig.lockedSlot[0], section, "LOCKED_SLOT_1_X", "LOCKED_SLOT_1_Y",
		defaultLockedSlot1.x, defaultLockedSlot1.y, fileName);
	ReadPoint(outConfig.lockedSlot[1], section, "LOCKED_SLOT_2_X", "LOCKED_SLOT_2_Y",
		defaultLockedSlot2.x, defaultLockedSlot2.y, fileName);

	outConfig.charCreateButtonX = ReadWord(section, "CHAR_CREATE_BUTTON_X", outConfig.panelX + PHOENIX_CHAR_MANAGER_CREATE_BUTTON_X_OFFSET, fileName);
	outConfig.charCreateButtonY = ReadWord(section, "CHAR_CREATE_BUTTON_Y", outConfig.panelY + PHOENIX_CHAR_MANAGER_CREATE_BUTTON_Y_OFFSET, fileName);
	outConfig.charDeleteButtonX = ReadWord(section, "CHAR_DELETE_BUTTON_X", outConfig.panelX + PHOENIX_CHAR_MANAGER_DELETE_BUTTON_X_OFFSET, fileName);
	outConfig.charDeleteButtonY = ReadWord(section, "CHAR_DELETE_BUTTON_Y", outConfig.panelY + PHOENIX_CHAR_MANAGER_DELETE_BUTTON_Y_OFFSET, fileName);

	outConfig.effectFireLeft1X = ReadWord(section, "EFFECT_FIRE_LEFT1_X", 0, fileName);
	outConfig.effectFireLeft1Y = ReadWord(section, "EFFECT_FIRE_LEFT1_Y", 0, fileName);
	outConfig.effectFireLeft2X = ReadWord(section, "EFFECT_FIRE_LEFT2_X", 0, fileName);
	outConfig.effectFireLeft2Y = ReadWord(section, "EFFECT_FIRE_LEFT2_Y", 0, fileName);
	outConfig.effectFireRight1X = ReadWord(section, "EFFECT_FIRE_RIGHT1_X", 0, fileName);
	outConfig.effectFireRight1Y = ReadWord(section, "EFFECT_FIRE_RIGHT1_Y", 0, fileName);
	outConfig.effectFireRight2X = ReadWord(section, "EFFECT_FIRE_RIGHT2_X", 0, fileName);
	outConfig.effectFireRight2Y = ReadWord(section, "EFFECT_FIRE_RIGHT2_Y", 0, fileName);
	outConfig.effectFireCenterX = ReadWord(section, "EFFECT_FIRE_FIRE_CENTER_X", 0, fileName);
	outConfig.effectFireCenterY = ReadWord(section, "EFFECT_FIRE_FIRE_CENTER_Y", 0, fileName);

	ReadPoint(outConfig.effectCharAttachUp[PHOENIX_TITLE_SLOT_LEFT], section, "EFFECT_CHAR_ATTACH_LEFT_UP_X", "EFFECT_CHAR_ATTACH_LEFT_UP_Y", 0, 0, fileName);
	ReadPoint(outConfig.effectCharAttachUp[PHOENIX_TITLE_SLOT_CENTER], section, "EFFECT_CHAR_ATTACH_CENTER_UP_X", "EFFECT_CHAR_ATTACH_CENTER_UP_Y", 0, 0, fileName);
	ReadPoint(outConfig.effectCharAttachUp[PHOENIX_TITLE_SLOT_RIGHT], section, "EFFECT_CHAR_ATTACH_RIGHT_UP_X", "EFFECT_CHAR_ATTACH_RIGHT_UP_Y", 0, 0, fileName);

	ReadPoint(outConfig.effectCharAttachDown[PHOENIX_TITLE_SLOT_LEFT], section, "EFFECT_CHAR_ATTACH_LEFT_DOWN_X", "EFFECT_CHAR_ATTACH_LEFT_DOWN_Y", 0, 0, fileName);
	ReadPoint(outConfig.effectCharAttachDown[PHOENIX_TITLE_SLOT_CENTER], section, "EFFECT_CHAR_ATTACH_CENTER_DOWN_X", "EFFECT_CHAR_ATTACH_CENTER_DOWN_Y", 0, 0, fileName);
	ReadPoint(outConfig.effectCharAttachDown[PHOENIX_TITLE_SLOT_RIGHT], section, "EFFECT_CHAR_ATTACH_RIGHT_DOWN_X", "EFFECT_CHAR_ATTACH_RIGHT_DOWN_Y", 0, 0, fileName);

	ReadPoint(outConfig.effectCharSelect[PHOENIX_TITLE_SLOT_LEFT], section, "EFFECT_CHAR_SELECT_LEFT_X", "EFFECT_CHAR_SELECT_LEFT_Y", 0, 0, fileName);
	ReadPoint(outConfig.effectCharSelect[PHOENIX_TITLE_SLOT_CENTER], section, "EFFECT_CHAR_SELECT_CENTER_X", "EFFECT_CHAR_SELECT_CENTER_Y", 0, 0, fileName);
	ReadPoint(outConfig.effectCharSelect[PHOENIX_TITLE_SLOT_RIGHT], section, "EFFECT_CHAR_SELECT_RIGHT_X", "EFFECT_CHAR_SELECT_RIGHT_Y", 0, 0, fileName);

	outConfig.childX = ReadWord(section, "CHILD_X", 0, fileName);
	outConfig.childY = ReadWord(section, "CHILD_Y", 0, fileName);
	outConfig.adultX = ReadWord(section, "ADULT_X", 0, fileName);
	outConfig.adultY = ReadWord(section, "ADULT_Y", 0, fileName);
	outConfig.violenceX = ReadWord(section, "VIOLENCE_X", 0, fileName);
	outConfig.violenceY = ReadWord(section, "VIOLENCE_Y", 0, fileName);
	outConfig.fearX = ReadWord(section, "FEAR_X", 0, fileName);
	outConfig.fearY = ReadWord(section, "FEAR_Y", 0, fileName);
	outConfig.textBarX = ReadWord(section, "TEXT_BAR_X", 0, fileName);
	outConfig.textBarY = ReadWord(section, "TEXT_BAR_Y", 0, fileName);
	outConfig.aniMarkX = ReadWord(section, "ANI_MARK_X", 0, fileName);
	outConfig.aniMarkY = ReadWord(section, "ANI_MARK_Y", 0, fileName);

	ReadPoint(outConfig.textLine[PHOENIX_TITLE_LINE1], section, "TEXT_LINE1_X", "TEXT_LINE1_Y", outConfig.textBarX, outConfig.textBarY, fileName);
	ReadPoint(outConfig.textLine[PHOENIX_TITLE_LINE2], section, "TEXT_LINE2_X", "TEXT_LINE2_Y", outConfig.textBarX, outConfig.textBarY, fileName);
	ReadPoint(outConfig.textLine[PHOENIX_TITLE_LINE3], section, "TEXT_LINE3_X", "TEXT_LINE3_Y", outConfig.textBarX, outConfig.textBarY, fileName);

	outConfig.premiumUseDayTextX = ReadWordAlias(section, "PREMIUM_USE_DAY_TEXT_X", "PRIMEUP_USE_DAY_TEXT_X", 0, fileName);
	outConfig.premiumUseDayTextY = ReadWordAlias(section, "PREMIUM_USE_DAY_TEXT_Y", "PRIMEUP_USE_DAY_TEXT_Y", 0, fileName);
	outConfig.warningStringX = ReadWordAlias(section, "WARNING_STRING_X", "WORNIN_STRING_X", 0, fileName);
	outConfig.warningStringY = ReadWordAlias(section, "WARNING_STRING_Y", "WORNIN_STRING_Y", 0, fileName);

	GetPrivateProfileStringA(section, "TEXT_LINE1", "", outConfig.textLineString[PHOENIX_TITLE_LINE1], sizeof(outConfig.textLineString[PHOENIX_TITLE_LINE1]), fileName);
	GetPrivateProfileStringA(section, "TEXT_LINE2", "", outConfig.textLineString[PHOENIX_TITLE_LINE2], sizeof(outConfig.textLineString[PHOENIX_TITLE_LINE2]), fileName);
	GetPrivateProfileStringA(section, "TEXT_LINE3", "", outConfig.textLineString[PHOENIX_TITLE_LINE3], sizeof(outConfig.textLineString[PHOENIX_TITLE_LINE3]), fileName);

	outConfig.charInfoX = ReadWord(section, "CHAR_INFO_X", outConfig.listInfoX, fileName);
	outConfig.charInfoY = ReadWord(section, "CHAR_INFO_Y", outConfig.listInfoY, fileName);
	outConfig.slayerCharInfoX = ReadWord(section, "SLAYER_CHAR_INFO_X", outConfig.listInfoX, fileName);
	outConfig.slayerCharInfoY = ReadWord(section, "SLAYER_CHAR_INFO_Y", outConfig.listInfoY, fileName);
}

POINT PhoenixCharManagerLockedSlotPoint(
	WORD panelX,
	WORD rightSlotButtonY,
	int lockedSlotIndex)
{
	POINT point;
	point.x = panelX + PHOENIX_CHAR_MANAGER_LOCKED_SLOT_X_OFFSET;
	point.y = rightSlotButtonY +
		(lockedSlotIndex == 0 ? PHOENIX_CHAR_MANAGER_LOCKED_SLOT1_Y_OFFSET : PHOENIX_CHAR_MANAGER_LOCKED_SLOT2_Y_OFFSET);
	return point;
}
