//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectLoaderManager.cpp
// Written By  : elca, excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectLoaderManager.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Zone.h"

#include "EffectBloodDrain.h"
#include "EffectLight.h"
#include "EffectAftermath.h"
#include "EffectEnemyErase.h"
#include "EffectRestore.h"
#include "EffectKillAftermath.h"
#include "EffectMute.h"
#include "EffectOnBridge.h"

#include "EffectGreenPoison.h"
#include "EffectYellowPoison.h"
#include "EffectDarkness.h"
#include "EffectAcidSwamp.h"
#include "EffectContinualBloodyWall.h"
#include "EffectIceField.h"
#include "EffectProminence.h"

#include "EffectCanEnterGDRLair.h"
#include "EffectBehemothForceScroll.h"
#include "EffectSafeForceScroll.h"
#include "EffectCarnelianForceScroll.h"

#include "EffectGreyfellForceScroll.h"
#include "EffectSoviusForceScroll.h"
#include "EffectMirForceScroll.h"

#include "EffectHoodlumStigma.h"

#include "EffectBleeding.h"

#include "EffectBonusExp.h"

#include "EffectMysteryPotion.h"

//20070724
#include "EffectOdinForceScroll.h"
#include "EffectTituosForceScroll.h"
//20070907
#include "EffectPetBonusExp.h"
//20080226 DSG by Kim Sung Yong
#include "EffectDSG.h"
// 20080310 계급 모래시계 by Kim sung yong
#include "EffectRankBonusExpRed.h"
#include "EffectRankBonusExpBlue.h"
// 20081028 wlzzi - Extreme Force Scroll
#include "EffectExtremeForceScroll.h"
#include "EffectPromotionAngelFlutter.h"
#include "EffectPromotionBatWing.h"
#include "EffectPromotionButterflyWing.h"
#include "EffectChaoticExp.h"

EffectLoaderManager::EffectLoaderManager () 
	throw ()
{
	__BEGIN_TRY
	__END_CATCH
}

EffectLoaderManager::~EffectLoaderManager () 
	throw ()
{
	__BEGIN_TRY

	SAFE_DELETE(g_pEffectBloodDrainLoader);
	SAFE_DELETE(g_pEffectLightLoader);
	SAFE_DELETE(g_pEffectAftermathLoader);
	SAFE_DELETE(g_pEffectEnemyEraseLoader);
	SAFE_DELETE(g_pEffectRestoreLoader);
	SAFE_DELETE(g_pEffectKillAftermathLoader);
	SAFE_DELETE(g_pEffectMuteLoader);

	SAFE_DELETE(g_pEffectGreenPoisonLoader);
	SAFE_DELETE(g_pEffectYellowPoisonLoader);
	SAFE_DELETE(g_pEffectDarknessLoader);
	SAFE_DELETE(g_pEffectAcidSwampLoader);
	SAFE_DELETE(g_pEffectContinualBloodyWallLoader);
	SAFE_DELETE(g_pEffectIceFieldLoader);
	SAFE_DELETE(g_pEffectProminenceLoader);

	SAFE_DELETE(g_pEffectOnBridgeLoader);
	SAFE_DELETE(g_pEffectCanEnterGDRLairLoader);
	SAFE_DELETE(g_pEffectBehemothForceScrollLoader);
	SAFE_DELETE(g_pEffectSafeForceScrollLoader);
	SAFE_DELETE(g_pEffectCarnelianForceScrollLoader);

	SAFE_DELETE(g_pEffectHoodlumStigmaLoader);
	SAFE_DELETE(g_pEffectMysteryPotionLoader);

	SAFE_DELETE(g_pEffectGreyfellForceScrollLoader);
	SAFE_DELETE(g_pEffectSoviusForceScrollLoader);
	SAFE_DELETE(g_pEffectMirForceScrollLoader);

	SAFE_DELETE(g_pEffectBonusExpLoader);

	//20070724
	SAFE_DELETE(g_pEffectOdinForceScrollLoader);
	SAFE_DELETE(g_pEffectTituosForceScrollLoader);

	//20070907 펫 경험치 모래시계
	SAFE_DELETE(g_pEffectPetBonusExpLoader);
	//20080226 DSG by Kim Sung Yong
	SAFE_DELETE(g_pEffectDSGLoader);
	//20080310 계급경험 모래시계 by Kim Sung Yong
	SAFE_DELETE(g_pEffectRankBonusExpRedLoader);
	SAFE_DELETE(g_pEffectRankBonusExpBlueLoader);
	//20081028 wlzzi - Extreme Force Scroll
	SAFE_DELETE(g_pEffectExtremeForceScrollLoader);
	SAFE_DELETE(g_pEffectPromotionAngelFlutterLoader);	
	SAFE_DELETE(g_pEffectPromotionBatWingLoader);	
	SAFE_DELETE(g_pEffectPromotionButterflyWingLoader);
	SAFE_DELETE(g_pEffectChaoticExpLoader);	
	__END_CATCH
}

void EffectLoaderManager::init () 
	throw (Error)
{
	__BEGIN_TRY

	m_pEffectLoaders[Effect::EFFECT_CLASS_BLOOD_DRAIN] = g_pEffectBloodDrainLoader = new EffectBloodDrainLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_LIGHT] = g_pEffectLightLoader = new EffectLightLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_AFTERMATH] = g_pEffectAftermathLoader = new EffectAftermathLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_ENEMY_ERASE] = g_pEffectEnemyEraseLoader = new EffectEnemyEraseLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_RESTORE] = g_pEffectRestoreLoader = new EffectRestoreLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_KILL_AFTERMATH] = g_pEffectKillAftermathLoader = new EffectKillAftermathLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_MUTE] = g_pEffectMuteLoader = new EffectMuteLoader();

	m_pEffectLoaders[Effect::EFFECT_CLASS_GREEN_POISON] = g_pEffectGreenPoisonLoader = new EffectGreenPoisonLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_YELLOW_POISON] = g_pEffectYellowPoisonLoader = new EffectYellowPoisonLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_DARKNESS] = g_pEffectDarknessLoader = new EffectDarknessLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_ACID_SWAMP] = g_pEffectAcidSwampLoader = new EffectAcidSwampLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_CONTINUAL_BLOODY_WALL] = g_pEffectContinualBloodyWallLoader = new EffectContinualBloodyWallLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_ICE_FIELD] = g_pEffectIceFieldLoader = new EffectIceFieldLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_PROMINENCE] = g_pEffectProminenceLoader = new EffectProminenceLoader();

	m_pEffectLoaders[Effect::EFFECT_CLASS_ON_BRIDGE] = g_pEffectOnBridgeLoader = new EffectOnBridgeLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_CAN_ENTER_GDR_LAIR] = g_pEffectCanEnterGDRLairLoader = new EffectCanEnterGDRLairLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_BEHEMOTH_FORCE_SCROLL] = g_pEffectBehemothForceScrollLoader = new EffectBehemothForceScrollLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_SAFE_FORCE_SCROLL] = g_pEffectSafeForceScrollLoader = new EffectSafeForceScrollLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_CARNELIAN_FORCE_SCROLL] = g_pEffectCarnelianForceScrollLoader = new EffectCarnelianForceScrollLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_HOODLUM_STIGMA] = g_pEffectHoodlumStigmaLoader = new EffectHoodlumStigmaLoader();

	m_pEffectLoaders[Effect::EFFECT_CLASS_GREYFELL_FORCE_SCROLL] = g_pEffectGreyfellForceScrollLoader = new EffectGreyfellForceScrollLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_SOVIUS_FORCE_SCROLL] = g_pEffectSoviusForceScrollLoader = new EffectSoviusForceScrollLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_MIR_FORCE_SCROLL] = g_pEffectMirForceScrollLoader = new EffectMirForceScrollLoader();

	m_pEffectLoaders[Effect::EFFECT_CLASS_BONUS_EXP] = g_pEffectBonusExpLoader = new EffectBonusExpLoader();

	m_pEffectLoaders[Effect::EFFECT_CLASS_MYSTERY_POTION] = g_pEffectMysteryPotionLoader = new EffectMysteryPotionLoader();
	//20070724
	m_pEffectLoaders[Effect::EFFECT_CLASS_ODIN_FORCE_SCROLL] = g_pEffectOdinForceScrollLoader = new EffectOdinForceScrollLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_TITUOS_FORCE_SCROLL] = g_pEffectTituosForceScrollLoader = new EffectTituosForceScrollLoader();
	//20070907 펫경험치 모래시계 추가	
	m_pEffectLoaders[Effect::EFFECT_CLASS_PET_BONUS_EXP] = g_pEffectPetBonusExpLoader = new EffectPetBonusExpLoader();
	//20080226 DSG by Kim Sung Yong	
	m_pEffectLoaders[Effect::EFFECT_CLASS_DSG] = g_pEffectDSGLoader = new EffectDSGLoader();
	//20080310 계급경험치 by Kim Sung Yong	
	m_pEffectLoaders[Effect::EFFECT_CLASS_RANK_BONUS_EXP_RED] = g_pEffectRankBonusExpRedLoader = new EffectRankBonusExpRedLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_RANK_BONUS_EXP_BLUE] = g_pEffectRankBonusExpBlueLoader = new EffectRankBonusExpBlueLoader();
	//20081028 wlzzi - Extreme Force Scroll
	m_pEffectLoaders[Effect::EFFECT_CLASS_EXTREME_FORCE_SCROLL] = g_pEffectExtremeForceScrollLoader = new EffectExtremeForceScrollLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_PROMOTION_ANGEL_FLUTTER] = g_pEffectPromotionAngelFlutterLoader = new EffectPromotionAngelFlutterLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_PROMOTION_BAT_WING] = g_pEffectPromotionBatWingLoader = new EffectPromotionBatWingLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_PROMOTION_BUTTERFLY_WING] = g_pEffectPromotionButterflyWingLoader = new EffectPromotionButterflyWingLoader();
	m_pEffectLoaders[Effect::EFFECT_CLASS_CHAOTIC_STONE_EXP] = g_pEffectChaoticExpLoader = new EffectChaoticExpLoader();
	__END_CATCH
}

void EffectLoaderManager::load (Slayer* pSlayer) 
	throw (Error)
{
	__BEGIN_TRY

	g_pEffectBloodDrainLoader->load(pSlayer);
	g_pEffectLightLoader->load(pSlayer);
	g_pEffectAftermathLoader->load(pSlayer);
	g_pEffectEnemyEraseLoader->load(pSlayer);
	g_pEffectRestoreLoader->load(pSlayer);
	g_pEffectKillAftermathLoader->load(pSlayer);
	g_pEffectMuteLoader->load(pSlayer);
	g_pEffectCanEnterGDRLairLoader->load(pSlayer);
	g_pEffectBehemothForceScrollLoader->load(pSlayer);
	g_pEffectSafeForceScrollLoader->load(pSlayer);
	g_pEffectCarnelianForceScrollLoader->load(pSlayer);
	g_pEffectHoodlumStigmaLoader->load(pSlayer);

	g_pEffectGreyfellForceScrollLoader->load(pSlayer);
	g_pEffectSoviusForceScrollLoader->load(pSlayer);
	g_pEffectMirForceScrollLoader->load(pSlayer);
	
	g_pEffectBonusExpLoader->load(pSlayer);

	g_pEffectMysteryPotionLoader->load(pSlayer);
	//20070724
	g_pEffectOdinForceScrollLoader->load(pSlayer);
	g_pEffectTituosForceScrollLoader->load(pSlayer);
	//20070907
	g_pEffectPetBonusExpLoader->load(pSlayer);
	//20080226 DSG by Kim Sung Yong
	g_pEffectDSGLoader->load(pSlayer);
	//20080310 계급경험치 by Kim Sung Yong
	g_pEffectRankBonusExpRedLoader->load(pSlayer);
	g_pEffectRankBonusExpBlueLoader->load(pSlayer);
	//20081028 wlzzi - Extreme Force Scroll
	g_pEffectExtremeForceScrollLoader->load(pSlayer);
	g_pEffectPromotionAngelFlutterLoader->load(pSlayer);
	g_pEffectChaoticExpLoader->load(pSlayer);
	__END_CATCH
}

void EffectLoaderManager::load (Vampire* pVampire) 
	throw (Error)
{
	__BEGIN_TRY

	g_pEffectBloodDrainLoader->load(pVampire);
	g_pEffectLightLoader->load(pVampire);
	g_pEffectAftermathLoader->load(pVampire);
	g_pEffectEnemyEraseLoader->load(pVampire);
	g_pEffectRestoreLoader->load(pVampire);
	g_pEffectKillAftermathLoader->load(pVampire);
	g_pEffectMuteLoader->load(pVampire);
	g_pEffectCanEnterGDRLairLoader->load(pVampire);
	g_pEffectBehemothForceScrollLoader->load(pVampire);
	g_pEffectSafeForceScrollLoader->load(pVampire);
	g_pEffectCarnelianForceScrollLoader->load(pVampire);
	g_pEffectHoodlumStigmaLoader->load(pVampire);

	g_pEffectGreyfellForceScrollLoader->load(pVampire);
	g_pEffectSoviusForceScrollLoader->load(pVampire);
	g_pEffectMirForceScrollLoader->load(pVampire);

	g_pEffectBonusExpLoader->load(pVampire);

	g_pEffectMysteryPotionLoader->load(pVampire);
	//20070724
	g_pEffectOdinForceScrollLoader->load(pVampire);
	g_pEffectTituosForceScrollLoader->load(pVampire);
	//20070907
	g_pEffectPetBonusExpLoader->load(pVampire);
	//20080226 DSG by Kim Sung Yong
	g_pEffectDSGLoader->load(pVampire);
	//20080310 계급경험치 by Kim Sung Yong
	g_pEffectRankBonusExpRedLoader->load(pVampire);
	g_pEffectRankBonusExpBlueLoader->load(pVampire);

	//20081028 wlzzi - Extreme Force Scroll
	g_pEffectExtremeForceScrollLoader->load(pVampire);

	g_pEffectPromotionBatWingLoader->load(pVampire);
	g_pEffectChaoticExpLoader->load(pVampire);	
	__END_CATCH
}

void EffectLoaderManager::load (Ousters* pOusters) 
	throw (Error)
{
	__BEGIN_TRY

	g_pEffectBloodDrainLoader->load(pOusters);
	g_pEffectAftermathLoader->load(pOusters);
	g_pEffectKillAftermathLoader->load(pOusters);
	g_pEffectMuteLoader->load(pOusters);
	g_pEffectCanEnterGDRLairLoader->load(pOusters);
	g_pEffectBehemothForceScrollLoader->load(pOusters);
	g_pEffectSafeForceScrollLoader->load(pOusters);
	g_pEffectCarnelianForceScrollLoader->load(pOusters);
	g_pEffectHoodlumStigmaLoader->load(pOusters);

	g_pEffectGreyfellForceScrollLoader->load(pOusters);
	g_pEffectSoviusForceScrollLoader->load(pOusters);
	g_pEffectMirForceScrollLoader->load(pOusters);

	g_pEffectBonusExpLoader->load(pOusters);

	g_pEffectMysteryPotionLoader->load(pOusters);
	//20070724
	g_pEffectOdinForceScrollLoader->load(pOusters);
	g_pEffectTituosForceScrollLoader->load(pOusters);
	//20070907
	g_pEffectPetBonusExpLoader->load(pOusters);
	//20080226 DSG by Kim Sung Yong
	g_pEffectDSGLoader->load(pOusters);
	//20080310 계급경험치 by Kim Sung Yong
	g_pEffectRankBonusExpRedLoader->load(pOusters);
	g_pEffectRankBonusExpBlueLoader->load(pOusters);
	//20081028 wlzzi - Extreme Force Scroll
	g_pEffectExtremeForceScrollLoader->load(pOusters);

	g_pEffectPromotionButterflyWingLoader->load(pOusters);
	g_pEffectChaoticExpLoader->load(pOusters);
	__END_CATCH
}

void EffectLoaderManager::load (Zone* pZone)
	throw (Error)
{
	__BEGIN_TRY

	g_pEffectGreenPoisonLoader->load(pZone);
	g_pEffectYellowPoisonLoader->load(pZone);
	g_pEffectDarknessLoader->load(pZone);
	g_pEffectAcidSwampLoader->load(pZone);
	g_pEffectContinualBloodyWallLoader->load(pZone);
	g_pEffectIceFieldLoader->load(pZone);
	g_pEffectProminenceLoader->load(pZone);
	g_pEffectOnBridgeLoader->load(pZone);

	__END_CATCH
}

string EffectLoaderManager::toString () const
	throw ()
{
	__BEGIN_TRY

	return "";

	__END_CATCH
}

// global variable definition
EffectLoaderManager* g_pEffectLoaderManager = NULL;
