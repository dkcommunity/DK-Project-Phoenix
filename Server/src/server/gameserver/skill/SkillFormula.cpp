//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillFormula.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SkillHandler.h"
#include "Assert.h"
#include "Vampire.h"
#include "Monster.h"

// 검계열
#include "DoubleImpact.h"
#include "TripleSlasher.h"
#include "RainbowSlasher.h"
#include "ThunderSpark.h"
#include "DancingSword.h"
#include "CrossCounter.h"
#include "FlashSliding.h"
#include "LightningHand.h"
#include "SwordWave.h"
#include "SnakeCombo.h"
#include "WindDivider.h"
#include "ThunderBolt.h"
#include "Expansion.h"
#include "MiracleShield.h"
#include "ThunderFlash.h"
#include "ThunderStorm.h"
#include "MentalSword.h"
#include "SharpShield.h"
#include "WideLightning.h"
#include "SwordRay.h"
#include "HitConvert.h"

// 도계열
#include "SingleBlow.h"
#include "SpiralSlay.h"
#include "TripleBreak.h"
#include "WildSmash.h"
#include "GhostBlade.h"
#include "PotentialExplosion.h"
#include "ShadowWalk.h"
#include "ChargingPower.h"
#include "HurricaneCombo.h"
#include "TornadoSever.h"
#include "Earthquake.h"
#include "Berserker.h"
#include "MoonlightSever.h"
#include "ShadowDancing.h"
#include "Typhoon.h"
#include "AirShield.h"
#include "PowerOfLand.h"
#include "MultiAmputate.h"
#include "WildTyphoon.h"

// 군인 계열
#include "QuickFire.h"
#include "DoubleShot.h"
#include "TripleShot.h"
#include "MultiShot.h"
#include "HeadShot.h"
#include "Piercing.h"
#include "Sniping.h"
#include "MindControl.h"
//#include "DetectMine.h"
#include "CreateBomb.h"
#include "CreateMine.h"
#include "InstallMine.h"
#include "DisarmMine.h"
#include "Revealer.h"
#include "ObservingEye.h"
#include "GunShotGuidance.h"
#include "BulletOfLight.h"
#include "Concealment.h"
#include "UltimateBlow.h"

// 인챈 계열
#include "CreateHolyWater.h"
#include "Light.h"
#include "DetectHidden.h"
#include "AuraBall.h"
#include "Bless.h"
#include "ContinualLight.h"
#include "Flare.h"
#include "Purify.h"
#include "AuraRing.h"
#include "Striking.h"
#include "DetectInvisibility.h"
//#include "Identify.h"
#include "AuraShield.h"
//#include "Enchant.h"
#include "Visible.h"
#include "VigorDrop.h"
#include "Sanctuary.h"
#include "Reflection.h"
#include "Hymn.h"
#include "LightBall.h"
#include "Rebuke.h"
#include "SpiritGuard.h"
#include "Requital.h"
#include "Lightness.h"

// 힐링 계열
#include "CureLightWounds.h"
#include "CurePoison.h"
#include "ProtectionFromPoison.h"
#include "CauseLightWounds.h"
#include "CureSeriousWounds.h"
#include "RemoveCurse.h"
#include "ProtectionFromCurse.h"
#include "CauseSeriousWounds.h"
#include "CureCriticalWounds.h"
#include "ProtectionFromAcid.h"
#include "Sacrifice.h"
#include "CauseCriticalWounds.h"
#include "CureAll.h"
#include "RegenerationSkill.h"
#include "Resurrect.h"
#include "EnergyDrop.h"
#include "Activation.h"
#include "HolyBlast.h"
//#include "MassCure.h"
//#include "MassHeal.h"
#include "HolyArrow.h"
#include "Regeneration.h"
#include "TurnUndead.h"
#include "DenialMagic.h"
#include "Illendue.h"


// 뱀프 계열
#include "PoisonousHands.h"
#include "GreenPoison.h"
#include "YellowPoison.h"
#include "DarkBluePoison.h"
#include "GreenStalker.h"

#include "AcidTouch.h"
#include "AcidBolt.h"
#include "AcidBall.h"
#include "AcidSwamp.h"

#include "Paralyze.h"
#include "Doom.h"
#include "Seduction.h"
//#include "Blind.h"
#include "Death.h"
#include "Mephisto.h"

#include "BloodyNail.h"
#include "BloodyKnife.h"
#include "BloodyBall.h"
#include "BloodyWave.h"
#include "BloodyBreaker.h"
#include "BloodyMasterWave.h"
#include "BloodyWarp.h"
//#include "BloodyWall.h"
#include "BloodySpear.h"
#include "BloodyWall.h"
#include "BloodySnake.h"
#include "RapidGliding.h"

#include "Hide.h"
#include "Darkness.h"
#include "Invisibility.h"
#include "TransformToWolf.h"
#include "TransformToBat.h"

//#include "SummonWolf.h"
#include "SummonCasket.h"
#include "OpenCasket.h"
//#include "RaisingDead.h"
//#include "SummonServant.h"

#include "BloodyMarker.h"
#include "BloodyTunnel.h"

#include "PoisonStrike.h"
#include "AcidStrike.h"
#include "BloodyStrike.h"
#include "PoisonStorm.h"
#include "AcidStorm.h"
#include "BloodyStorm.h"
#include "Extreme.h"
#include "Transfusion.h"
#include "HandsOfWisdom.h"
#include "Armageddon.h"

// 기타
#include "CriticalGround.h"
#include "SummonMonsters.h"

#include "Restore.h"

#include "Peace.h"
#include "GroundAttack.h"
#include "MeteorStrike.h"
#include "Hallucination.h"
#include "DuplicateSelf.h"

#include "SoulChain.h"

// 성지스킬
#include "MagicElusion.h"
#include "PoisonMesh.h"
#include "IllusionOfAvenge.h"
#include "WillOfLife.h"

// 아우스터즈 스킬
#include "Flourish.h"
#include "Evade.h"
#include "SharpRound.h"
#include "BackStab.h"
#include "Blunting.h"
#include "GammaChop.h"
#include "CrossGuard.h"
#include "KasasArrow.h"
#include "HandsOfFire.h"
#include "Prominence.h"
#include "RingOfFlare.h"
#include "BlazeBolt.h"
#include "IceField.h"
#include "WaterBarrier.h"
#include "NymphRecovery.h"
#include "Liberty.h"
#include "Tendril.h"

#include "StoneAuger.h"

#include "EarthsTeeth.h"
#include "GnomesWhisper.h"
#include "HandsOfNizie.h"
#include "RefusalEther.h"

#include "EmissionWater.h"
#include "BeatHead.h"
#include "DivineSpirits.h"
#include "BlitzSliding.h"
#include "JabbingVein.h"
#include "GreatHeal.h"
#include "DivineGuidance.h"
#include "BlazeWalk.h"
#include "BloodyZenith.h"

#include "Rediance.h"
#include "LarSlash.h"
#include "Trident.h"

#include "HeartCatalyst.h"
#include "ProtectionFromBlood.h"
#include "MoleShot.h"
#include "Eternity.h"

#include "InstallTrap.h"
#include "HolyArmor.h"
#include "MercyGround.h"
#include "CreateHolyPotion.h"

#include "TransformToWerwolf.h"
#include "GrayDarkness.h"
#include "StoneSkin.h"
#include "TalonOfCrow.h"
#include "Howl.h"

#include "AcidEruption.h"
#include "Teleport.h"

#include "FirePiercing.h"
#include "SoulRebirth.h"
#include "IceLance.h"
#include "ExplosionWater.h"

#include "FrozenArmor.h"
#include "ReactiveArmor.h"
#include "MagnumSpear.h"
#include "HellFire.h"
#include "GroundBless.h"

#include "SharpChakram.h"
#include "DestructionSpear.h"
#include "ShiftBreak.h"
#include "FatalSnick.h"
#include "ChargingAttack.h"
#include "DuckingWallop.h"

#include "DistanceBlitz.h"
#include "SummonGroundElemental.h"
#include "SummonFireElemental.h"
#include "SummonWaterElemental.h"
#include "MeteorStorm.h"

#include "WideIceField.h"
#include "Glacier1.h"
#include "Glacier2.h"
#include "IceAuger.h"
#include "IceHail.h"
#include "WideIceHail.h"
#include "IceWave.h"
#include "LandMineExplosion.h"
#include "ClaymoreExplosion.h"
#include "PleasureExplosion.h"
#include "DeleoEfficio.h"
#include "ReputoFactum.h"

#include "SwordOfThor.h"
#include "BurningSolCharging.h"
#include "BurningSolLaunch.h"
#include "SweepVice.h"
#include "Whitsuntide.h"
#include "ViolentPhantom.h"
#include "InstallTurret.h"
#include "TurretFire.h"
#include "SummonGoreGland.h"
#include "GoreGlandFire.h"
#include "ThrowingAxe.h"
#include "ChoppingFirewood.h"
#include "ChainThrowingAxe.h"
#include "MultiThrowingAxe.h"
#include "PlayingWithFire.h"

#include "InfinityThunderbolt.h"
#include "SpitStream.h"
#include "PlasmaRocketLauncher.h"
#include "BombingStar.h"
#include "IntimateGrail.h"
#include "NooseOfWraith.h"
#include "SetAfire.h"
#include "Cannonade.h"

// 아우스터즈 140레벨 스킬
#include "SharpHail.h"
#include "IceHorizon.h"
#include "FuryOfGnome.h"
#include "SummonMiga.h"
#include "SummonMigaAttack.h"
#include "SelfDestruction.h"
#include "ARAttack.h"
#include "SMGAttack.h"
#include "GrenadeAttack.h"

#include "Halo.h"
#include "Destinies.h"
#include "FierceFlame.h"
#include "ShadowOfStorm.h"
#include "WildWolf.h"
#include "Aberration.h"
#include "DragonTornado.h"
#include "BikeCrash.h"
#include "HarpoonBomb.h"
#include "PassingHeal.h"
#include "RottenApple.h"
#include "FeebleVirus.h"
#include "SwordCombo.h"
#include "BladeCombo.h"
#include "VampireCombo.h"
#include "OustersCombo.h"

#include "Brandish.h"
#include "Parasite.h"
#include "SoundWave.h"
#include "FireWave.h"
#include "TargetSummonMonsters.h"

#include "BloodyBurst.h"
#include "TraceCreature.h"
// 2차 승직 스킬
#include "ShineSword.h"
#include "BombCrashWalk.h"
#include "SatelliteBomb.h"
#include "IllusionInversion.h"
#include "HeavenGround.h"
#include "BloodyScarify.h"
#include "BloodCurse.h"
#include "BloodsSymposionAttack.h"
#include "DummyDrake.h"
#include "HydroConvergence.h"
#include "SummonClay.h"
#include "HeterChakram.h"

#include "RequestResurrect.h"

// 2007.01.22
#include "Tanning.h"

// 20070820
#include "MagicShield1.h"
// 20070828
#include "MagicShield2.h"
#include "EaglesEye1.h"
#include "EaglesEye2.h"
#include "FreezeRing1.h"
#include "FreezeRing2.h"
#include "BatStorm1.h"
#include "BatStorm2.h"
#include "CurseOfBlood1.h"
#include "CurseOfBlood2.h"
#include "BloodyShout1.h"
#include "BloodyShout2.h"

#include "BloodyWings.h"
#include "MistOfSoul1.h"
#include "MistOfSoul2.h"

#include "AttackCerberus.h"
#include "AttackManticoret.h"
#include "AttackBogletH.h"
#include "AttackBogletB.h"
#include "AttackMassacre.h"
#include "AttackPlumpyBoar.h"
#include "AttackCabracam.h"

#include "MenegrothTrap1.h"
#include "MenegrothTrap2.h"

// 승직 레어스킬
#include "HeavenlySpark.h" // 20080401
#include "BladeStorm.h" // 20080403
#include "ExplosionRocketLauncher.h" // 20080407
#include "Genesis.h" // 20080407
#include "Thorns.h" // 20080408
#include "LethalClaw.h" // 20080410
#include "VenomCircle.h" // 20080414
#include "Brionac.h" // 20080415
#include "AsteroidBomber.h" // 20080415
#include "FrozenGlacier.h" // 20080415
#include "Incinerate.h" // 20080415

#include "OniblaRaiseBone.h"
#include "OniblaDropSkull.h"
#include "AgonyChildDropHead.h"
#include "AgonyChildShadowFour.h"
#include "VillainLadyGhostShadow.h"
#include "VillainLadySelfDestruction.h"
#include "OniSpanielThrowingAxe.h"
#include "BlackAstralMagicCrystal.h"
#include "WhiteAstralSpellCrystal.h"

#include "MagmaDetonation.h"
#include "MagmaDetonation2.h"
#include "SquallyBarrier1.h"
#include "SquallyBarrier2.h"
#include "SiusCrash.h"
#include "LycanHowl.h"
#include "BifronzeMagic.h"
#include "PalusMiddleRangeMagic.h"
#include "PalusLongRangeMagic.h"
#include "GarumMagic.h"
#include "ClavieMagic.h"
#include "WideWildWolf.h"
#include "SmileofDeath.h"
#include "FuryShock.h"
#include "RailGunAttack.h"
#include "BlessofLycan.h"
#include "BLAZEWALK2.h"
#include "GLADIATOR.h"
#include "CRUSHINGSTORM.h"
#include "SATELLITEBOMB2.h"
#include "INTIMATEGRAIL2.h"
#include "HOLYARMOR2.h"
#include "STRIKING2.h"
#include "BLESS2.h"
#include "GLORYGROUND.h"
#include "PASSINGHEALK2.h"
#include "GREATHEAL2.h"
#include "CHAINOFDEMON.h"
#include "BLOODYSKULL.h"
#include "RAGEOFBLOOD.h"
#include "ALLYSTERWIND.h"
#include "FLAMESIGHT.h"
#include "SPIRALMEGALITH.h"
#include "RADCHIA.h"
#include "HEALPASS2.h"
#include "BREATHOFDRYAD.h"
#include "CUREEFFECT2.h"
#include "LIVINGDEADBECHOESPER.h"
#include "MIHAIBATSTREAM.h"
#include "TSARBLUEROSEDEVIL.h"
#include "FOLLETPOISONFRAME.h"
#include "FUEFOLLETREDROSEDEVIL.h"
#include "DACULETALONOFDDRACULE.h"
#include "DRACULEBLOOKDYSCARIFY2.h"
#include "DRACULEBLOODYCARPE.h"
#include "DRACULEDRACULECASKET.h"
#include "DRAGONHURRICANE.h"
#include "LARSTROKE.h"
#include "GAEBULGA.h"
#include "VICIOUSGUIDANCE.h"
#include "SPECTORINVERSE.h"
#include "BATBREAKER.h"
#include "SHADYDOUPLE.h"
#include "CHAKRAMHAIL.h"
#include "FLAMESPIKE.h"
#include "BLIZZARD.h"
#include "DEMOLISHER.h"
#include "MAGMATICERUPTION.h"
#include "ADV2LIGHTINGSHOCK.h"
#include "ADV2HALCYONMK1.h"
#include "MK1Fire.h"
#include "ADV2SOLARFLARE.h"
#include "ADV2VIACRUCIS.h"
#include "ADV2CHAOSSPLITTER.h"
#include "ADV2DEADLYGROUND.h"
#include "DEADLYATT.h"
#include "ADV2SUDDENBLAST.h"
#include "ADV2TORRENTIALLIFE.h"
#include "ADV2CYCLINGWARROCK.h"
#include "ADV2TRUECLOLOROFVAMPLOAD.h"
#include "ADV2THEROAROFAWAKENWARRIOR.h"
#include "ADV2THERAGEOFGUARDIANSPRIT.h"
#include "ENRAGECONQUROR.h"
#include "ATTRACTCIRCLE.h"
#include "CLIENTSHADYDOUPLE.h"
#include "CLIENTBATBREAKER.h"
#include "ADV2METEORBLAZE.h"
#include "IGNISINFERNUM.h"
#include "DEMONICRESURRECTIO.h"
#include "WOLFROAR.h"
#include "EVGENEIABELLUM.h"
#include "OUTRAGEBOOSTER.h"
#include "ICEPRESSURE.h"
#include "CRYSTALTHORN.h"
#include "FAVOROFLUNA.h"
#include "TEMENOS.h"
#include "DEPENDSOUL.h"
#include "PULSEMAGNETICFIELD.h"
#include "TRANSFORMDARKBATSTORM.h"
#include "ULRAYIRYMELLEATTACKA.h"
#include "ULRAYIRYMELLEATTACKB.h"
#include <math.h>
//2014-07-14 221skill
#include "ADV2ADVENTBLADE.h"
#include "ADV2CHAINLIGHTNINGSWORD.h"
#include "ADV2HALCYONMK1NAPALNBULLET.h"
#include "ADV2CONTRADICTO.h"
#include "ADV2DIVINETRACES.h"
#include "ADV2WILDWOLFSTRIKE.h"
#include "ADV2DEADLYSTORM.h"
#include "ADV2SHININGCHAKRAMSPIN.h"
#include "ADV2INFINITYBLAZE.h"
#include "ADV2GRACEOFELQUINES.h"
#include "ADV2SUMMONDEFENSIVESPINEL.h"
//2014-07-24 monsterskill
#include "CREATUREGOLEMQUAKEATTACK.h"
#include "VOMIT.h"
#include "LAYWASTE.h"
#include "HALFMOONKICK.h"
#include "BLOODPUNISH.h"
#include "BLOODPRAY.h"
#include "DEVILWAVE.h"
#include "PARASITEATTACK.h"
#include "DEVILSCREAM.h"
#include "BONESTRIKE.h"
#include "LIGHTNINGSLASH.h"
#include "GENESISATTACK.h"

#include "BLOODYSKULL2.h"
#include "ADV2CHAOSSPLITTER2.h"
#include "CRYSTALTHORN2.h"
#include "BLIZZARD2.h"
#include "ADV2METEORBLAZE2.h"
#include "CHAKRAMHAIL2.h"
#include "ADV2SOLARFLARE2.h"
#include "ADV2VIACRUCIS2.h"
#include "MK1Fire2.h"
#include "ADV2LIGHTINGSHOCK2.h"
#include "DRAGONHURRICANE2.h"

#include "ADV3BLADEERUPTION1.h"
#include "ADV3ATTRACTSWORDCRASH1.h"
#include "ADV3STRATEGICBOMBER1.h"
#include "ADV3AURAEXPLOSION1.h"
#include "ADV3HOLYACIDALIUM1.h"
#include "ADV3BATSPEAR1.h"
#include "ADV3EXTENSIVEDELUSION1.h"
#include "ADV3ACCELERATECHARGE1.h"
#include "ADV3RAGEBURST1.h"
#include "ADV3FROZENGROUND1.h"
#include "ADV3METEORICFALL1.h"
#include "ADV3WARPPORTAL.h"

#include "ADV3BLADEERUPTION2.h"
#include "ADV3ATTRACTSWORDCRASH2.h"
#include "ADV3STRATEGICBOMBER2.h"
#include "ADV3AURAEXPLOSION2.h"
#include "ADV3HOLYACIDALIUM2.h"
#include "ADV3BATSPEAR2.h"
#include "ADV3EXTENSIVEDELUSION2.h"
#include "ADV3ACCELERATECHARGE2.h"
#include "ADV3RAGEBURST2.h"
#include "ADV3FROZENGROUND2.h"
#include "ADV3METEORICFALL2.h"


#include "SetAfire2.h"
#include "SHADYDOUPLE2.h"
#include "PotentialExplosion2.h"
#include "SharpShield2.h"
#include "Sniping2.h"
#include "Thorns2.h"
#include "CureSeriousWounds2.h"
#include "Halo2.h"
#include "RingOfFlare2.h"
#include "WaterBarrier2.h"
#include "ReactiveArmor2.h"

const int PartyEffectBoost[7] = 
{
	100, // 0
	100, // 1
	120, // 2
	135, // 3
	140, // 4
	145, // 5
	150  // 6
};

const int PartyDurationBoost[7] = 
{
	100, // 0
	100, // 1
	130, // 2
	155, // 3
	175, // 4
	190, // 5
	200  // 6
};

//////////////////////////////////////////////////////////////////////////////
// 검 계열
//////////////////////////////////////////////////////////////////////////////

void DoubleImpact::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 1 + (input.STR/20) + input.SkillLevel/20;
	output.Delay  = 8; // 1초
	//output.Damage = 2 + input.SkillLevel/33;
	//output.Delay  = 10; // 1초
}

void TripleSlasher::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage = 2 + (input.STR/20) + (input.SkillLevel/15);
	output.Damage = 3 + (input.STR/20) + (input.SkillLevel/15);
	output.Delay  = 8; // 0.7초
	//output.Damage = 3 + input.SkillLevel/33;
	//output.Delay  = 7; // 0.7초
}

void RainbowSlasher::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 4 + input.STR/15 + input.SkillLevel/10;
	output.Delay  = 8; // 0.8초
	//output.Damage = 7 + input.SkillLevel/33;
	//output.Delay  = 8; // 0.8초
}

void ThunderSpark::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 3 + input.STR/20 + input.SkillLevel/10;
	output.Delay  = 7; // 0.7초
	//output.Damage = 5 + input.SkillLevel/33;
	//output.Delay  = 7; // 0.7초
}

void DancingSword::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 1 + (input.DEX/10) + (input.SkillLevel/10);
//	output.Duration = (10 + input.STR/10 + input.SkillLevel/2) * 10;
	//output.Duration = (30 + input.STR/10 + input.SkillLevel/2) * 10;
        output.Duration = 6000;
	//output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초
	output.Delay	= 40;		// Delay 와 Duration 이 같다. by bezz 2003.3.5

	//output.Damage   = 5 + input.SkillLevel/10;
	//output.Duration = (30 + input.SkillLevel/4) * 10;
	//output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초
}

void CrossCounter::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Duration = (10 + input.SkillLevel/2) * 10;
	output.Duration = 6000;
	//output.Delay    = max(5 - input.SkillLevel/20, 2) * 10; // 5->2초
	output.Delay	= 40;		// Delay 와 Duration 이 같다. by bezz 2003.3.5

	//output.Duration = (30 + input.SkillLevel/4) * 10;
	//output.Delay    = max(5 - input.SkillLevel/20, 2) * 10; // 5->2초
}

void FlashSliding::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = max(3 - input.SkillLevel/50, 1) * 10; // 3->1초
	output.Duration = 3 - input.SkillLevel/50; 
	//output.Delay = (3 - input.SkillLevel/50) * 10; // 3->1초
}

void LightningHand::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/10 + input.SkillLevel/10;
	// 클라이언트와의 동기화 문제로 딜레이가 없거나 조금 더 짧아야 한다.
	output.Delay  = 4; // 0.2초
	//output.Damage = 7 + input.SkillLevel/20;
	//output.Delay  = 2; // 0.2초
}

void SwordWave::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage = 5 + input.SkillLevel/33;
	output.Damage = 15 + input.SkillLevel/20;
	output.Delay  = 4; // 1초
	//output.Damage = 5 + input.SkillLevel/33;
	//output.Delay  = 10; // 1초
}

void SnakeCombo::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// by sigi. 2002.12.3
	output.Damage = 5 + input.STR/12 + input.SkillLevel/8;
	output.Delay  = 10; // 1초
	//output.Damage = 7 + input.SkillLevel/10;
	//output.Delay  = 10; // 1초
}

void WindDivider::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/14 + input.SkillLevel/20;
	output.Delay  = 13; // 1초
}

void ThunderBolt::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/10 + input.SkillLevel/10; // by bezz. 2002.12.10
	output.Delay  = (3 - input.SkillLevel/50) * 10;	// by sigi. 2002.12.3
}

void Expansion::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Damage   = 10 + input.SkillLevel/2; // 자신의 + 값
        output.Damage   = 2000;
	output.ToHit    = 1000;//5  + input.SkillLevel/3; // 파티의 + 값
//	output.Duration = (30 + input.STR/10 + input.SkillLevel*2/3) * 10;
	//output.Duration = (45 + input.STR/10 + input.SkillLevel) * 10;
        //output.Duration = 3600;
        output.Duration = 12000;
	//output.Delay    = max(5 - input.SkillLevel/33,2) * 10;
	output.Delay	= output.Duration;		// Delay 와 Duration 이 같다. by bezz 2003.3.5

	//output.Damage   = 30 + input.SkillLevel/2; // 자신의 + 값
	//output.ToHit    = 15 + input.SkillLevel/4; // 파티의 + 값
	//output.Duration = (30 + input.SkillLevel/3) * 10;
	//output.Delay    = (5 - input.SkillLevel/33) * 10;
}

void MiracleShield::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = (5 + input.SkillLevel/5); // 5->25
	output.Duration = (30 + input.SkillLevel/5) * 10; // 30~50초
	output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초
}

void ThunderFlash::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 5 + input.SkillLevel/10;
	output.Delay  = 20; // 2초
}

void ThunderStorm::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/10 + input.SkillLevel/10; // by bezz. 2002.12.10
	output.Delay  = (3 - input.SkillLevel/50)*10; // 1초
	//output.Damage = 10 + input.SkillLevel/5;
	//output.Delay  = (5 - input.SkillLevel/33)*10; // 1초
}

void MentalSword::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 30 + ( 15 * input.SkillLevel/50 );
	output.Range =  2 + input.SkillLevel/33;
	output.Delay  = (8 - input.SkillLevel / 20)*10; // 5~3초
	//output.Damage = 30 + ( 15 * input.SkillLev/50 );
	//output.Range = 2 + input.SkillLevel/20;
	//output.Delay  = (5 - input.SkillLevel / 50)*10; // 5~3초
}

//////////////////////////////////////////////////////////////////////////////
// 도 계열
//////////////////////////////////////////////////////////////////////////////

void SingleBlow::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 1 + input.STR/20 + input.SkillLevel/20;	// by sigi. 2002.12.3
	output.Delay  = 7; // 1초
}

void SpiralSlay::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage = 2 + input.STR/20 + input.SkillLevel/15;	// by sigi. 2002.12.3
	output.Damage = 4 + input.STR/20 + input.SkillLevel/15;
	output.Delay  = 7; // 0.7초
}

void TripleBreak::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 3 + input.STR/20 + input.SkillLevel/10;	// by sigi. 2002.12.3
	output.Delay  = 7; // 0.7초
}

void WildSmash::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 4 + input.STR/15 + input.SkillLevel/10;	// by sigi. 2002.12.3
	output.Delay  = 8; // 0.8초
}

void GhostBlade::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 5 + input.SkillLevel/10;
	//output.Duration = (30 + input.SkillLevel/4) * 10;
	//output.Duration = (10 + input.STR/10 + input.SkillLevel/2) * 10;
	//output.Duration = (30 + input.STR/10 + input.SkillLevel/2) * 10;
         output.Duration = 6000;
	//output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초
	output.Delay	= 40;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

void PotentialExplosion::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 1 + input.SkillLevel/15;
//	output.Duration = (10 + input.SkillLevel/2) * 10;
 	//output.Duration = (30 + input.SkillLevel/2) * 10;
        output.Duration = 6000;
	//output.Delay    = (7 - input.SkillLevel/20) * 10; // 7->2초
	output.Delay	= 40;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

void ShadowWalk::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Range = max(2 + input.SkillLevel/25,2);
	output.Delay = (3 - input.SkillLevel/50) * 10; // 3->1초
}

void ChargingPower::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 5 + (input.SkillLevel/20) + (input.DomainLevel/30);
 //	output.Duration = (60 + input.SkillLevel*3/2) * 10;
//	output.Duration = (30 + input.STR/10 + input.SkillLevel*10/12) * 10;
// 	output.Damage   = 1 + (input.STR + input.SkillLevel)/20;
 //	output.Duration = (10 + input.SkillLevel/2) * 10;
	//output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초
        output.Duration = 6000;
	output.Delay	= 40;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

void HurricaneCombo::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 5 + input.STR/12 + input.SkillLevel/8;
	output.Delay  = 10; // 1초
}

void TornadoSever::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage = 8 + input.STR/15 + input.SkillLevel/10;
//	output.Damage = 3 + input.STR/20 + input.SkillLevel/10;
	output.Damage = 10 + input.STR/20 + input.SkillLevel/8;
//	output.Delay  = 20 - (input.SkillLevel/100); // 2초
	output.Delay  = 4;
}

void Earthquake::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 5 + input.STR/20 + input.SkillLevel/10;	// by sigi. 2002.12.3
	output.Range  = 7; // by bezz. 2002.12.10
	output.Delay  = 13; // 1.3초 by bezz. 2002.12.10
}

void Berserker::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// penalty값은 Berserker.cpp에 있다.
	//output.Damage    = (input.STR/20) * (1 + input.SkillLevel/20); //  데미지 보너스
	//output.ToHit    = (input.DEX/10) * (1 + input.SkillLevel/10); //  투힛 보너스

	// 2002.12.06 장홍창
	output.Damage    = (input.STR/20) * (1 + input.SkillLevel/25); //  데미지 보너스
//	output.ToHit    = (input.DEX/10) * (1 + input.SkillLevel/12); //  투힛 보너스
	output.ToHit    = 10 + (1 + input.SkillLevel/12); //  투힛 보너스

//	output.Duration = (30 + input.SkillLevel/5) * 10;
	//output.Duration = (45 + input.SkillLevel/1.5) * 10;
        output.Duration = 6000;
	//output.Delay    = max(5 - input.SkillLevel/33,2) * 10;
	output.Delay	= 40;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

void MoonlightSever::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage   = 8 + input.STR/10 + input.SkillLevel/10;
	output.Damage   = 15 + input.STR/10 + input.SkillLevel/8;
//	output.Delay    = 8; // 0.8초
	output.Delay    = 4; // 0.4초
}

void ShadowDancing::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage = 30 + ( 15 * input.SkillLevel/50 );
	output.Damage = 30 + ( 15 * input.SkillLevel/25 );
	output.ToHit    = 10 + input.SkillLevel/20; // 크리티컬 확률 증가치
	//output.Delay    = (5 - input.SkillLevel / 50)*10; // 2초를 세번으로 나누면 0.66666 = 0.7초
	output.Delay    = (8 - input.SkillLevel / 20)*10; // by sigi. 2002.12.3
}

void Typhoon::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = input.STR/10 + input.SkillLevel/5;
	//output.Duration = 10; // 1초 (타겟이 삥삥 돌아가는 시간) - effect없다 -_-;
	//output.Delay    = 15; // 1.5초
	output.Duration = (1 + input.SkillLevel/100)*10; // 1초 (타겟이 삥삥 돌아가는 시간) - effect없다 -_-;
	output.Delay    = (3 - input.SkillLevel/50)*10; // by bezz. 2002.12.10
}

//////////////////////////////////////////////////////////////////////////////
// 총 계열
//////////////////////////////////////////////////////////////////////////////

void QuickFire::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.ToHit  = -20 + (input.DEX/10) + input.SkillLevel/5;
	//output.Damage = -50 + input.STR/10 + input.SkillLevel/3;
	output.ToHit  = -30 + input.DEX/20 + input.SkillLevel/5;
	output.Damage = -30 + input.STR/20 + input.SkillLevel/5;
	output.Delay  = 2;
}

void DoubleShot::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.ToHit  = -20 + input.SkillLevel/5;
	output.Damage = -50 + input.SkillLevel/3;
	output.Delay  = 2;
}

void TripleShot::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.ToHit  = -20 + input.SkillLevel/5;
	output.Damage = -50 + input.SkillLevel/3;
	output.Delay  = 2;
}

void MultiShot::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.IClass == Item::ITEM_CLASS_SG)
	{
//		output.Damage = 3 + input.SkillLevel/10;
		output.Damage = 8 + input.SkillLevel/10;
	}

	else if(input.IClass == Item::ITEM_CLASS_AR ||
			input.IClass == Item::ITEM_CLASS_SMG)
	{
//		output.Damage = 1 + input.SkillLevel/30;
		output.Damage = 5 + input.SkillLevel/15;
	}

	else if(input.IClass == Item::ITEM_CLASS_SR)
	{
//		output.Damage = input.SkillLevel/50;
		output.Damage = 3 + input.SkillLevel/20;
	}

	output.ToHit  = -20 + input.SkillLevel/5;
	output.Delay  = 8; // 0.8초
}

void HeadShot::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 0;

	// SkillUtil.cpp의 computeArmsWeaponDamageBonus()에서 계산되므로.. 중복 데미지다. - -; by sigi. 2002.12.3
	if (input.IClass == Item::ITEM_CLASS_SG)
	{
		switch (input.Range)
		{
			case 3: output.Damage = 5;
			case 2: output.Damage = 8;
			case 1: output.Damage = 10;
			default:
				break;
		}
	}
	else if (input.IClass == Item::ITEM_CLASS_AR || input.IClass == Item::ITEM_CLASS_SMG)
	{
		switch (input.Range)
		{
			case 3: output.Damage = 5;
			case 2: output.Damage = 6;
			case 1: output.Damage = 8;
			default:
				break;
		}
	}
	else if (input.IClass == Item::ITEM_CLASS_SR)
	{
		switch(input.Range)
		{
			case 3: output.Damage = 3;
			case 2: output.Damage = 6;
			case 1: output.Damage = 8;
			default:
				break;
		}
	}

	output.Delay = 8; // 0.8초
}

void Piercing::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.ToHit  = 0;
	output.Damage = 2;
	output.Delay  = 8; // 0.8초
}

void Sniping::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = (30 + input.SkillLevel/5) * 10;
	output.Delay    = (10 - input.SkillLevel/20) * 10; // 10->5초
}

void MindControl::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 15 + input.SkillLevel/10;
	output.Duration = (30 + input.SkillLevel/5) * 10;
	output.Delay    = 30; // 3초
}

void Revealer::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = (30 + input.SkillLevel/5) * 10; // 30~50초
	output.Delay = output.Duration;
//	output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초

	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void CreateBomb::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 15 + input.SkillLevel/10;
	output.Duration = (30 + input.SkillLevel/5) * 10;
	output.Delay    = 30 - input.SkillLevel / 10; // 3초 ~ 2초
}

void CreateMine::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 15 + input.SkillLevel/10;
	output.Duration = (30 + input.SkillLevel/5) * 10;
	output.Delay    = 30 - input.SkillLevel / 10; // 3초 ~ 2초
}

void InstallMine::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 15 + input.SkillLevel/10;
	output.Duration = (30 + input.SkillLevel/5) * 10;
//	output.Delay    = 30 - input.SkillLevel / 10; // 3초 ~ 2초
	output.Delay    = 90 - input.SkillLevel / 2.5; // 9초 ~ 5초
}

void DisarmMine::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 15 + input.SkillLevel/10;
	output.Duration = (30 + input.SkillLevel/5) * 10;
	output.Delay    = 30; // 3초
}

void ObservingEye::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Duration = (10 + input.SkillLevel/5)*10;
	output.Duration = (30 + input.SkillLevel/3)*10;
	// damage값은 vision으로 넣고
	// 다른 값을 적당히 계산해서 쓴다. -_-;
	output.Damage = 1 + input.SkillLevel/50;
	//output.Delay  = (5 - input.DEX/50) * 10; // 20초
	output.Delay	= output.Duration;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

//////////////////////////////////////////////////////////////////////////////
// 인챈트 계열
//////////////////////////////////////////////////////////////////////////////

void CreateHolyWater::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = (5 - input.SkillLevel/33) * 10; // 5->2초 
}

void Light::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = (60 + input.SkillLevel/20 * 60) * 10; // 60~360초
	output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초

	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void DetectHidden::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = 1200; // 30~50초
	output.Delay    = (5 - min(input.INTE,150)/50) * 10; // 5->2초

	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void AuraBall::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage = 5 + (input.INTE/20) + input.SkillLevel/5;
//	output.Damage = 10 + (input.INTE/10) + input.SkillLevel/5;
	output.Damage = 16 + (input.INTE/10) + input.SkillLevel/4;
	output.Delay  = 10; // 1초
//	output.Range = 2 + (input.SkillLevel/25); // 쓰지도 않는 값 왜 넣어놨을까.. 8/18

	// 공격계 기술에는 파티 보너스가 존재하지 않는다.
}

void Bless::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if ( input.TargetType == SkillInput::TARGET_SELF )
	{
//		output.Damage   = input.INTE/20 + input.SkillLevel/20;
		output.Damage   = min(40,(4 + input.INTE/40 + input.SkillLevel/20));
	}
	else
	{
//		output.Damage   = input.INTE/20 + input.SkillLevel/20;
		output.Damage   = min(35,(2 + input.INTE/40 + input.SkillLevel/20));
	}

	output.Duration = (30 + input.SkillLevel * 3 / 2) * 10; // 30~50초
	output.Delay    = (7 - input.SkillLevel/20) * 10; // 6->3초
	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void ContinualLight::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	switch (g_pSkillInfoManager->getGradeByDomainLevel(input.DomainLevel))
	{
		case SKILL_GRADE_APPRENTICE:   output.Range = 1; break;
		case SKILL_GRADE_ADEPT:        output.Range = 3; break;
		case SKILL_GRADE_EXPERT:       output.Range = 4; break;
		case SKILL_GRADE_MASTER:       output.Range = 5; break;
		case SKILL_GRADE_GRAND_MASTER: output.Range = 6; break;
		default:                       output.Range = 0; break;
	}

	output.Delay = (6 - input.SkillLevel/25) * 10; // 5->2초
	output.Duration = (10 + input.SkillLevel/2) * 10;
}

void Flare::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Duration = (5 + input.SkillLevel/5) * 10; // 10~20초
	output.Duration = 15 * 10;
	output.Delay    = (6 - input.SkillLevel/33) * 10; // 6->3초

	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void Purify::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 10 + input.SkillLevel/10;
	output.Delay  = (5 - input.SkillLevel/33) * 10; // 5->2초

	switch (g_pSkillInfoManager->getGradeByDomainLevel(input.DomainLevel))
	{
		case SKILL_GRADE_APPRENTICE:   output.Range = 3; break;
		case SKILL_GRADE_ADEPT:        output.Range = 3; break;
		case SKILL_GRADE_EXPERT:       output.Range = 5; break;
		case SKILL_GRADE_MASTER:       output.Range = 5; break;
		case SKILL_GRADE_GRAND_MASTER: output.Range = 7; break;
		default:                       output.Range = 0; break;
	}

	// 공격계 기술에는 파티 보너스가 존재하지 않는다.
}

void AuraRing::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage = 5 + input.INTE/10 + input.SkillLevel/3; // by bezz. 2002.12.10
	output.Damage = 85 + input.INTE/10 + input.SkillLevel/3; // by bezz. 2002.12.10
	output.Delay  = max( 1, 2 - ( input.SkillLevel / 50 ) ) * 10; // by bezz. 2002.12.10

	// 공격계 기술에는 파티 보너스가 존재하지 않는다.
}

void Striking::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER)
	{
//		output.Damage = input.INTE/33 + input.SkillLevel/20;
		output.Damage = input.INTE/30 + input.SkillLevel/20;
	}
	else
	{
//		output.Damage = input.INTE/20 + input.SkillLevel/10;
		output.Damage = input.INTE/30 + input.SkillLevel/10;
	}

	output.Duration = (30 + input.SkillLevel * 3 / 2) * 10;
//	output.Duration = (30 + input.SkillLevel * 2 / 3) * 10;
	output.Delay    = (6 - input.SkillLevel/33) * 10; // 6->3초

	// 파티 보너스를 계산한다.
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void DetectInvisibility::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = 1200; // 30~50초
	output.Delay    = (6 - input.SkillLevel/33) * 10; // 6->3초

	switch (g_pSkillInfoManager->getGradeByDomainLevel(input.DomainLevel))
	{
		case SKILL_GRADE_APPRENTICE:   output.Range = 3; break;
		case SKILL_GRADE_ADEPT:        output.Range = 3; break;
		case SKILL_GRADE_EXPERT:       output.Range = 5; break;
		case SKILL_GRADE_MASTER:       output.Range = 5; break;
		case SKILL_GRADE_GRAND_MASTER: output.Range = 7; break;
		default:                       output.Range = 0; break;
	}

	// 파티 보너스를 계산한다.
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void AuraShield::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = 6000; // 10~60초
	//output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초 

	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
	output.Delay	= 40;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

void Visible::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = (5 - input.SkillLevel/50) * 10; // 5->3초
	output.Range = 3 + (input.SkillLevel/50);

	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

//////////////////////////////////////////////////////////////////////////////
// 힐링 계열
//////////////////////////////////////////////////////////////////////////////

void CureLightWounds::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER) 
	{
		output.Damage = 10 + input.SkillLevel/8;
	}
	else
	{
		output.Damage = 10 + input.SkillLevel/4;
	}

	output.Delay = 10; // 1초

	// 파티 보너스를 계산한다.
	output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void CureAll::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER) 
	{
		output.Damage = 50 + input.SkillLevel/4;
	}
	else
	{
		output.Damage = 35 + input.SkillLevel/2;
	}

	output.Delay = 40 - input.SkillLevel / 5;  // 4초~ 2초

	// 파티 보너스를 계산한다.
	output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void CurePoison::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
	
	output.Delay = (4 - input.SkillLevel/33) * 10; // 4->1초
}

void ProtectionFromPoison::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER)
	{
		//output.Damage = 10 + input.SkillLevel/10;
		output.Damage = min(20, 10 + input.INTE/20);
	}
	else
	{
		//output.Damage = 20 + input.SkillLevel/10;
		output.Damage = min(30, 20 + input.INTE/20);
	}

	//output.Duration = (30 + input.SkillLevel/2) * 10; // 30~80초
//	output.Duration = (50 + input.INTE * 2 / 3) * 10; // 30~80초// by sigi. 2002.12.3
	output.Duration = (60 + input.INTE / 2 + input.SkillLevel / 2) * 10; // 
	output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초

	// 파티 보너스를 계산한다.
	output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void CauseLightWounds::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Damage = 10 + input.SkillLevel/10;
	//output.Damage = 5 + input.INTE/10 + input.SkillLevel/10;	// by sigi. 2002.12.3
	output.Damage = 10 + input.INTE/10 + input.SkillLevel/10;	// by sigi. 2002.12.3
	output.Delay  = 10; // 1초

	// 공격계 기술에는 파티 보너스가 존재하지 않는다.
}

void CureSeriousWounds::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER)
	{
		output.Damage = 30 + input.SkillLevel/8;
	}
	else
	{
		output.Damage = 30 + input.SkillLevel/4;
	}

	output.Delay = 10; // 1초

	// 파티 보너스를 계산한다.
	output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void RemoveCurse::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = (5 - input.SkillLevel/33) * 10; // 5->2초

	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void ProtectionFromCurse::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// by sigi. 2002.12.3
	if (input.TargetType == SkillInput::TARGET_OTHER)
	{
		//output.Damage = 10 + input.SkillLevel/10;
		output.Damage = min(20, 10 + input.INTE/20);
	}
	else
	{
		//output.Damage = 20 + input.SkillLevel/10;
		output.Damage = max(30, 20 + input.INTE/20);
	}

	//output.Duration = (25 + input.SkillLevel/2) * 10; // 25~75초
	//output.Duration = (40 + input.INTE*2/3) * 10; // 25~75초	// by sigi. 2002.12.3
	output.Duration = (60 + input.SkillLevel/2 + input.INTE/2) * 10; // 25~75초	// by sigi. 2002.12.3
	output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초

	// 파티 보너스를 계산한다.
	output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void Resurrect::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20; // 2초
}

void CauseSeriousWounds::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Damage = 30 + input.SkillLevel/4;
	// by sigi. 2002.12.3
	//output.Damage = 5 + input.INTE/8 + input.SkillLevel/5;
	output.Damage = 15 + input.INTE/8 + input.SkillLevel/5;
	output.Delay  = 10; // 1초

	// 공격계 기술에는 파티 보너스가 존재하지 않는다.
}

void CureCriticalWounds::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER)
	{
		output.Damage = 200;
	}
	else
	{
		output.Damage = 200;
	}

	output.Delay = ( 40 - input.SkillLevel/5 ) * 10 ; // 40초~20초

	output.Duration = 6000 ; // 20초~40초

	// 파티 보너스를 계산한다.
	output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void ProtectionFromAcid::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// by sigi. 2002.12.3
	if (input.TargetType == SkillInput::TARGET_OTHER)
	{
		//output.Damage = 10 + input.SkillLevel/10;
		output.Damage = min(20, 10 + input.INTE/20);
	}
	else 
	{
		//output.Damage = 20 + input.SkillLevel/10;
		output.Damage = min(30, 20 + input.INTE/20);
	}

	//output.Duration = (20 + input.SkillLevel/2) * 10; // 20~70초
	//output.Duration = (40 + input.INTE*2/3) * 10; // 20~70초
	output.Duration = (60 + input.SkillLevel/2 + input.INTE/2) * 10; // 20~70초
	output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초

	// 파티 보너스를 계산한다.
	output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void Sacrifice::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = (30 + input.SkillLevel/5) * 10; // 30~50초
	output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초

	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void CauseCriticalWounds::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	/*
	output.Damage = 20 + input.SkillLevel/6; 
	output.Delay  = 10; // 1초 
	*/

	//output.Damage = 3 + input.SkillLevel / 10; // 3-13
	// by sigi. 2002.12.3
	output.Damage = 5 + input.INTE/10 + input.SkillLevel/5; // 3-13
	//output.Delay = ( 10 - input.SkillLevel / 33 ) * 10; // 10-7초
	output.Delay = ( 15 - input.SkillLevel / 20 ) * 10; // 15-10초
	output.Duration = ( 3 + input.SkillLevel / 50 ) * 10; // 3-5초

	// 공격계 기술에는 파티 보너스가 존재하지 않는다.
}

/*
void CureAll::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = (4 - input.SkillLevel/33) * 10; // 4->1초
}
*/

void RegenerationSkill::computeOutput(const SkillInput& input, SkillOutput& output) 
{
}

void EnergyDrop::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Damage = 25 + input.SkillLevel/5; // 20 ~ 45
	//output.Delay = 30; // 3초
	//output.Damage = 10 + input.INTE/10 + input.SkillLevel/5; // 20 ~ 45 12.06

	//2002.12.06 장홍창
//	output.Damage = 10 + input.INTE/10 + input.SkillLevel/3; 
	output.Damage = 18 + input.INTE/10 + input.SkillLevel/2.5; 
//	output.Delay = (8 - input.SkillLevel/20)*10;
	output.Delay = (5 - input.SkillLevel/33)*10;
}

void VigorDrop::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// by sigi. 2002.12.3
	//output.Damage = 25 + input.SkillLevel/5; // 20 ~ 45
	//output.Delay = 30; // 3초
	//output.Damage = 10 + input.INTE/10 + input.SkillLevel/5; // 20 ~ 45 2002.12.06

	// 2002.12.06 장홍창
//	output.Damage = 10 + input.INTE/10 + input.SkillLevel/3; 
	output.Damage = 18 + input.INTE/10 + input.SkillLevel/3; 
//	output.Delay = (8 - input.SkillLevel/20)*10;
	output.Delay = (5 - input.SkillLevel/33)*10;
}

void Activation::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 0;

	output.Duration = 6000;
	output.Delay    = (6 - input.SkillLevel/50) * 10;

	// 파티 보너스를 계산한다.
	output.Damage   = 0;
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void HolyBlast::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 15 + input.SkillLevel/8;
	output.Delay  = (8 - input.SkillLevel/33)*10;
	output.Duration = 10;
}

void Sanctuary::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 0;
	output.Delay = 5 - input.SkillLevel/50;
	output.Duration = (5 + input.SkillLevel/20)*10;
}

void Reflection::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 0;
	//output.Delay = 5 - input.SkillLevel/33;
	output.Duration = (20 + input.SkillLevel/5)*10;
	output.Delay	= output.Duration;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

void Hymn::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 10 + input.SkillLevel/10;
	output.Duration = (20 + input.SkillLevel/2)*10;
	output.Delay    = 7 - input.SkillLevel/20;
}

//////////////////////////////////////////////////////////////////////////////
// 뱀파이어 계열
//////////////////////////////////////////////////////////////////////////////

void PoisonousHands::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = min(20, 3 + (input.INTE-20)/3);
	output.Delay  = 6; // 0.6초
}

void AcidTouch::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/6 + input.INTE/15;
	output.Delay  = 6; // 0.6초
}

void GreenPoison::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = min(15, 5 + (input.INTE-20)/10);
	output.Duration = min(20, 10 + (input.INTE-20)/10) * 10;
	output.Delay    = max(2, 5 - (input.INTE-20)/50) * 10; // 5->2초
}

void Darkness::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = ( 1 + input.INTE/40 ) * 10;
	output.Delay    = max( 3,  12 - input.INTE/50 ) * 10;
	output.Range    = (  3 + (input.SkillLevel-15) / 25);
}

void YellowPoison::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = input.INTE / 4 * 10;
	output.Delay    = max(2, 10 - input.DEX/30 ) * 10;
}

void TransformToBat::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 30;
}

void SummonCasket::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 10;
}

void OpenCasket::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 30;
}

void AcidBolt::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = min(40, input.STR / 20 + input.INTE/5);
	output.Delay  = max(1, 3 - (input.DEX/50) - (input.INTE/50)) * 10; // by bezz. 2002.12.10
}

void GreenStalker::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = input.STR/20 + input.INTE/10;
	output.Tick     = 40;
	//output.Duration = (input.DEX/5 + input.INTE/5)*10; // by sigi. 2002.12.3
	output.Duration = (input.DEX/30 + input.INTE/30)*10; // by sigi. 2002.12.3
	output.Delay    = max(1, 5 - input.DEX/40 ) * 10;
}

void BloodyTunnel::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 30;
}

void Paralyze::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = min(40, 25 + (input.INTE-20)/20);
	//output.Duration = (3 + input.INTE/15) * 10;
	output.Duration = min( 300, (3 + input.INTE/15) * 10 );		// 최대값 추가 by bezz 2005.06.16
	output.Delay    = max(3, (6 - input.DEX/50)) * 10;
}

void BloodyMarker::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 30;
}

void DarkBluePoison::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = min(20, 5 + (input.INTE-20)/10);
	output.Duration = min(45, 21 + (input.INTE-20)/5) * 10;
	output.Tick     = 30;
	output.Delay    = max(3, 5 - (input.INTE-20)/10) * 10;
}

void TransformToWolf::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 3000;
}

void Doom::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = min(30, 20 + (input.INTE-20)/20);
//	output.Duration = min(80, 30 + (input.INTE-20)/3) * 10;
	output.Duration = min(30, 10 + (input.INTE-20)/10) * 10;
	output.Delay    = max(3, 5 - (input.INTE-20)/10) * 10;
}

void AcidBall::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/15 + input.INTE/5; // by bezz. 2002.12.10
	output.Delay  = max(1, 3-input.DEX/50-input.INTE/50) * 10; // by bezz. 2002.12.10
}

void Invisibility::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 30;
}

void AcidSwamp::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = input.STR/15 + input.INTE/4;
	output.Duration = max(40, ( (input.DEX-40 + input.INTE-40)/6)*10);
	output.Tick     = 15;
	output.Delay    = max(2, 6-input.DEX/40) * 10;
}

void Seduction::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = min(30, 10 + (input.INTE-20)/10);
	output.Duration = min(50, 20 + (input.INTE-20)/5) * 10;
	output.Delay    = max(3, 5 - (input.INTE-20)/10) * 10;
}

void BloodyNail::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/8 + input.DEX/10 + input.INTE/30;
	output.Delay  = 6; // 0.6초
}

void BloodyKnife::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/10 + input.INTE/5 + input.DEX/10;
	output.Delay  = max( 2, 6 - ( input.DEX/50 ) - ( input.INTE/50 ) ); // by bezz. 2002.12.10
}

void BloodyBall::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/8 + input.INTE/4 + input.DEX/8;
	output.Delay = 30;
}

void BloodyWave::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.INTE/6 + input.STR/7 + input.DEX/7;
	output.Delay = max(1, 3-input.DEX/50 ) * 10;
	// ToHit는 bKnockback으로 쓰인다.
	output.ToHit = 50; //min(35, 10 + (int)(input.INTE/10));
}

void BloodyMasterWave::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = min(80, 40 + (input.INTE-20)/4 );
	output.Delay = 30;
	// ToHit는 bKnockback으로 쓰인다.
	output.ToHit = min(55, 30 + (int)(input.INTE/10));
}

void BloodyWarp::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = min(70, 30 + (input.INTE-20)/4 );
	output.Delay = 30;
	// ToHit는 bKnockback으로 쓰인다.
	output.ToHit = min(35, 10 + (int)(input.INTE/10));
}

void BloodyWall::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage	= input.INTE/5 + input.STR/10;
	output.Duration = (input.DEX/15 + input.INTE/10) * 10;
	output.Delay    = max(2, 5 - input.DEX/50) * 10;
	output.Tick		= 10;
}

void BloodySnake::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = min(35, 10 + (input.INTE-20)/9 );
	output.Duration = (1 + (input.INTE-20)/80) * 10; 
	output.Delay    = max(3, 6 - (input.INTE-20)/50) * 10;
	output.Tick = 3;
}


void BloodySpear::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = min(180,input.STR/6 + input.INTE/2 + input.DEX/12);
	output.Delay = max(2, 6 - input.DEX/50) * 10;
}

void PoisonStrike::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = min(65, input.STR/10 + input.INTE*2/9);
	output.Delay  = max(1,5 - input.DEX/50) * 10;
}

void AcidStrike::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = min(75, input.STR/8 + input.INTE/4);
	output.Delay = max(1, 5 - input.DEX/50) * 10;
}

void BloodyStrike::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = min(90,input.STR/8 + input.INTE*10/23);
	output.Delay = max(2, 6 - input.DEX/50) * 10;
}

void PoisonStorm::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Damage = input.STR/30 + input.INTE/9; 12.6

	//2002.12.06 장홍창
	output.Damage = input.STR/30 + input.INTE/4;

	// 2003.3.20 Sequoia 딜레이 미니멈 넣었다.
	output.Delay = max( (8 - input.DEX/50) * 10, 20 );
  	output.Range = min(6,3 + (input.SkillLevel-40)/8);	
}

void AcidStorm::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Damage = input.STR/28 + input.INTE/8;  12.6

	// 2002.12.06 장홍창
	output.Damage = input.STR/25 + input.INTE*2/7;  

	// 2003.3.20 Sequoia 딜레이 미니멈 넣었다.
	output.Delay = max( (9 - input.DEX/50) * 10, 20 );
}

void BloodyStorm::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR/25 + input.INTE/6;
	output.Duration = (input.DEX/100 + input.INTE/100) * 10;

	// 2003.3.20 Sequoia 딜레이 미니멈 넣었다.
	output.Delay = max( (9 - input.DEX/50) * 10, 20 );
}

void Extreme::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// 데미지 보너스는 여기서 계산하지 않고, 
	// Vampire::initAllStat()에서 계산한다.
	output.Damage   = 0; 
//	output.Duration = (30 + (input.INTE-20)/6) * 10;  // 30 -> 80
	output.Duration = (60 + input.STR/15 + input.DEX/6 + input.INTE/20) * 10;  // 30 -> 80
	output.Delay    = (6 - input.STR/70) * 10; // 5 -> 2
}

void CriticalGround::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// acid bolt랑 똑같당 - -;
	//output.Damage = min(40, 20 + (input.INTE-20)/6);
	//output.Delay  = 10; // 1초

	// 20
	// 145 --> 145~290 --> 22~46
	// 500 --> 500~1000 --> 125~250 (기획140~280)

	int divider = 1;
	if (input.STR < 200)
	{
		divider = 6;
	}
	else
	{
		divider = 4;
	}

	output.Damage = max(20, (input.STR + rand()%input.STR)/divider);
	output.Delay  = 10; // 0.6초

	// 공격계 기술에는 파티 보너스가 존재하지 않는다.
}

void Peace::computeOutput( const SkillInput& input, SkillOutput& output )
{
//	output.Duration = (15 + input.INTE / 10)*10;
	output.Duration = (10 + input.INTE / 40 + input.SkillLevel / 20)*10;
	output.Delay = max( 10 - input.INTE / 20, 5 ) * 10;
}

void Death::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// 2003.3.20 by Sequoia 최대값이 추가되었다.
//	output.Damage   = min( input.INTE*10/25 + input.STR/6, 150 );
	output.Damage   = min( input.INTE/5 + input.STR/12, 80 );
	output.Duration = (input.INTE/5 ) * 10;
	output.Delay    = max(2, 5 - input.DEX/50) * 10;
}

void Mephisto::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// 이 기술만 특별히.. parameter를 다음과 같이 넘겨준다.
	 //input.SkillLevel = pVampire->getSTR()+pVampire->getDEX()+pVampire->getINT();
	 //input.DomainLevel = pVampire->getLevel();

	//output.Damage   = 10 + (input.SkillLevel-60)/15;
	//output.Duration = (25 + (input.DomainLevel/4))*10;

	// by sigi. 2002.12.3
	//output.Damage   = 10 + (input.STR + input.DEX + input.INTE)/30;
	output.Duration = max(10, (input.DomainLevel - 95)*10) * 10;
	output.Delay    = max(2, 5 - (input.INTE-20)/10) * 10;
}

void Transfusion::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Damage   = min(60, 15 + (input.INTE-20)/5);
	//output.Duration = (10 + (input.INTE-20)/10) * 10; 
	//output.Delay    = 5;//max(3, 6 - (input.INTE-20)/50) * 10;
	output.Delay    = max(3, 6 - (input.INTE-20)/50) * 10;
}

void SummonMonsters::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 0;
	output.Delay  = 20; // 2초
}

void GroundAttack::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = min(100, 30 + (input.INTE-20)/5);	// % damage
	output.Duration = 20; 	// 몇 초후 폭발
	output.Tick     = 15;
	output.Delay    = max(3, 6 - (input.INTE-20)/50) * 10;
}

void MeteorStrike::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = (int)(input.SkillLevel * 0.8 + ( rand() % (int)( input.SkillLevel * 0.4 + 1 ) )) + (input.STR + input.DEX)/6;
	output.Duration = 10; 	// 몇 초후 폭발
	output.Delay = 10;
}

void Hallucination::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Damage   = min(20, 10 + (input.INTE-20)/20);
	//output.Duration = min(80, 30 + (input.INTE-20)/3) * 10;
	output.Duration = ( 20 + (input.STR/65) + (input.DEX/85) + (input.INTE/40) ) * 10;
	//output.Delay    = max(3, 5 - (input.INTE-20)/10) * 10;
	//output.Delay	= ( 10 - (input.DEX/50) ) * 10;
	output.Delay = output.Duration;
}

void DuplicateSelf::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = min((3+rand()%5), input.INTE/100);	// 분신 개수
	output.Duration = min(80, 30 + (input.INTE-20)/3) * 10;
	output.Delay    = max(3, 5 - (input.INTE-20)/10) * 10;
}

void SoulChain::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = 100;
	output.Delay    = 1200;
}

void SharpShield::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 6000;
	//output.Duration = ( 30 + ( input.SkillLevel / 2 ) ) * 10;	// 30 - 80 초
	output.Damage   = ( input.STR / 20 ) + ( input.SkillLevel / 20 );
	//output.Delay    = ( 5 - ( input.SkillLevel / 25 ) ) * 10; 
	output.Delay	= 40;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

void WideLightning::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay	= 10;
	output.Damage	= ( input.STR / 8 ) + ( input.SkillLevel / 3 );
	output.Duration	= 6;	// 0.6초
	output.Tick		= 6;	// 0.6초
}

void GunShotGuidance::computeOutput( const SkillInput& input, SkillOutput& output )
{
	output.Damage	= 50 + input.SkillLevel / 2;
	output.Duration	= 10;
	output.Delay	= 10;
	output.Range	= 8;
}

void AirShield::computeOutput( const SkillInput& input, SkillOutput& output )
{
//	output.Damage	= 50 + ( input.SkillLevel >> 1 ); 			// 50 + SkillLevel / 2
	output.Damage	= 50 + ( input.SkillLevel / 5 ); 			// 50 + SkillLevel / 2
	//output.Duration = ( 10 + ( input.SkillLevel >> 1 ) ) * 10;	// 10 + SkillLevel / 2 (초)
	output.Duration = 6000;	// 30-80 초
	//output.Delay	= ( 5 - ( input.SkillLevel / 25 ) ) * 10;
	output.Delay	= ( 30 + ( input.SkillLevel >> 1 ) ) * 10;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

void BulletOfLight::computeOutput( const SkillInput& input, SkillOutput& output )
{
/*	switch( input.IClass )
	{
		case Item::ITEM_CLASS_SMG:
			output.Damage = 2 + ( input.SkillLevel / 10 );
			break;
		case Item::ITEM_CLASS_SG:
			output.Damage = 3 + ( input.SkillLevel / 8 );
			break;
		case Item::ITEM_CLASS_AR:
			output.Damage = 4 + ( input.SkillLevel / 6 );
			break;
		case Item::ITEM_CLASS_SR:
			output.Damage = 5 + ( input.SkillLevel / 5 );
			break;
		default:
			output.Damage = 0;
			break;
	}*/
	output.Damage = - 20 + ( input.SkillLevel * 2 / 3 ); // / 1.5 );
	output.ToHit  = - 10 + ( input.SkillLevel / 2 ); // / 1.5 );
	output.Delay  = 2;	// 0.2초
}

void HandsOfWisdom::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.STR / 15 + input.INTE / 8;
	output.Delay  = 6; // 0.6초
}

void LightBall::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage = 2 + (input.INTE/20) + (input.SkillLevel/10);
	output.Damage = 9 + (input.INTE/20) + (input.SkillLevel/10);
	output.Delay  = 10; // 1초
	output.Range  = 2 + (input.SkillLevel/25);
}

void HolyArrow::computeOutput(const SkillInput& input, SkillOutput& output) 
{
//	output.Damage = 2 + (input.INTE/20) + (input.SkillLevel/10);
	output.Damage = 8 + (input.INTE/20) + (input.SkillLevel/10);
	output.Delay  = 10; // 1초
	output.Range  = 2 + (input.SkillLevel/25);
}

void Rebuke::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Duration = ( 2 + (input.SkillLevel/20) ) * 10;
	output.Duration = ( 8 + (input.SkillLevel/20) ) * 10;
//	output.Damage	= ( input.INTE/10 ) + ( input.SkillLevel/5 );
	output.Damage	= 10 + ( input.INTE/10 ) + ( input.SkillLevel/5 );
	output.Delay	= 80;
}

void SpiritGuard::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = ( 10 + (input.SkillLevel/2.5) ) * 10;
	output.Damage	= ( input.INTE/10 ) + ( input.SkillLevel/4 );
	output.Delay	= 100;
	output.Tick		= 10;
}

void Thorns::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 3000;
	output.Damage	= ( 30 + (input.INTE / 7) ) * (11 + (int)((input.Range - 1) / 10)) / 10; 
	output.Delay	= 100;
	output.Tick		= 10;
}

void Regeneration::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Duration = ( 5 + (input.SkillLevel/10) ) * 10;
	output.Damage	= 10 + ( input.INTE/40 ) + ( input.SkillLevel/10 );
	output.Duration = ( 15 + (input.SkillLevel/2) ) * 10;
//	output.Delay	= ( 6 + (input.SkillLevel/50) ) * 10;
	output.Delay	= output.Duration;
	output.Tick		= 10;
	output.Range	= 2 + ( input.SkillLevel/50 );
}

void PowerOfLand::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay	= 10;
	output.Damage	= ( input.STR / 8 ) + ( input.SkillLevel / 3 );
	output.Duration = 10;	// 1초
	output.Tick		= 10;	// 1초
}

void TurnUndead::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Damage	= 10 + ( input.INTE / 20 ) + ( input.SkillLevel / 2 );
	output.Damage	= 20 + ( input.INTE / 20 ) + ( input.SkillLevel / 3 );
	output.Delay	= 10;
}

void Armageddon::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage	= ( input.STR / 8 ) + ( input.INTE / 6 );
	output.Delay	= 10;
	output.Duration	= ( ( input.DEX / 10 ) + ( input.INTE / 20 ) ) * 10;
	output.Tick		= 10;
}

void BloodyBreaker::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.INTE/4 + input.STR/5 + input.DEX/5;
	output.Delay = max(1, 3-input.DEX/50 ) * 10;
	// ToHit 은 knockback 확률
	output.ToHit = 50;
}

void RapidGliding::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Range = min( 6, 2 + ( input.DEX / 50 ) );
	output.Delay = max( 1, 5 - ( input.DEX / 60) ) * 10;// min=1
}

void MagicElusion::computeOutput(const SkillInput& input, SkillOutput& output)
{
	Attr_t SUM = ( input.STR + input.DEX + input.INTE );
	output.Damage =  SUM / 5;
	output.Duration = 50 + ( SUM / 3 );
	output.Delay = 50;
}

void PoisonMesh::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage =  5 + input.SkillLevel/4;
	output.Duration = 50;
	output.Delay = 50;
}

void IllusionOfAvenge::computeOutput(const SkillInput& input, SkillOutput& output)
{
	Attr_t SUM = ( input.STR + input.DEX + input.INTE );
	output.Damage = 15 + (SUM/3);
	output.Delay = 50;
}

void WillOfLife::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 5 + input.SkillLevel/7;//7
	output.Duration = 30 + input.SkillLevel;
	output.Delay = output.Duration * 2;
}

void DenialMagic::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 1800;
	output.Delay = ( 8 - ( input.SkillLevel / 20 ) ) * 10;
}

void Requital::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 6000;
	output.Delay = 40;
	output.Damage = ( input.INTE / 8 ) + ( input.SkillLevel / 4 );
	output.Damage = min(100,output.Damage);
}

void Concealment::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Duration = ( 5 + ( input.SkillLevel / 5 )) * 10;
	output.Duration = 6000;
//	output.Duration = ( 30 + ( input.SkillLevel / 2 )) * 10;
//	output.Delay = max( 1, 5 - ( input.DEX / 50 ) ) * 10;
	output.Delay = 50;
}

void SwordRay::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Damage = 5 + input.STR/15 + input.SkillLevel/10;
	output.Damage = input.STR/25 + input.SkillLevel/20;
	output.Range =  2 + input.SkillLevel/33;
	output.Delay = ( 5 - input.SkillLevel/25 ) * 10;
	output.Tick=5;
	output.Duration=5;
}

void MultiAmputate::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.STR/20 + input.SkillLevel/10;
	output.Range =  2 + input.SkillLevel/25;
	output.Delay = ( 5 - input.SkillLevel/25 ) * 10;
}

void HitConvert::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = (8 - input.SkillLevel/33) * 10;
	output.Range = 2 + input.SkillLevel/33;
}

void WildTyphoon::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.STR/12) + (input.SkillLevel/5);
	output.Delay = 50 - input.SkillLevel*10/25;
	output.Range = 1;
}

void UltimateBlow::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = 100 - input.SkillLevel*10/25;
	output.Range = 1 + input.SkillLevel/50;
}

void Illendue::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = ( input.INTE / 10 ) * ( 1 + ( input.SkillLevel / 25 ) );
	output.Delay = 50 - input.SkillLevel*10/33;
	output.Range = 5;
}

void Lightness::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Duration = ( 15 + ( ( input.SkillLevel / 25 ) * 10 ) ) * 10;
	output.Duration = min( 1200, ( (input.INTE/15) + (input.SkillLevel/2) ) * 10 );
	output.Delay = min ( 50, output.Duration );
}

void Flourish::computeOutput(const SkillInput& input, SkillOutput& output)
{
	// 1 <= SkillLevel <= 15
	if ( input.SkillLevel < 16 )
	{
		output.Damage = 3 + (int)( ( input.STR / 20.0 ) * ( 1.0 + ( input.SkillLevel / 22.5 ) ) );
		output.Damage = min( 30, output.Damage );
	}
	// 16 <= SkillLevel <= 30
	else
	{
		output.Damage = 3 + (int)( ( input.STR / 20.0 ) * ( 4.0 / 3.0 + ( input.SkillLevel / 45.0 ) ) );
		output.Damage = min( 30, output.Damage );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}
}

void Evade::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
//		output.Damage = (int)( ( input.DEX / 10.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 22.5 ) ) );
//		output.Damage = 10 + (int)( input.SkillLevel * 16.0 / 9.0 );
		output.Damage = 10 + (int)( input.SkillLevel * 1.7 );
	}
	else
	{
//		output.Damage = (int)( ( input.DEX / 10.0 ) * ( 4.0/3.0 + ( (float)(input.SkillLevel) / 45.0 ) ) );
//		output.Damage = 10 + (int)( 40.0 / 3.0 + input.SkillLevel * 8.0 / 9.0 );
		output.Damage = 10 + (int)( 13.3 + input.SkillLevel * 0.9 );
//		if ( input.SkillLevel == 30 ) output.Damage = min(55, (int)(output.Damage * 1.1) );
		if ( input.SkillLevel == 30 ) output.Damage = 230;
	}

	output.Damage = max( 55, output.Damage );
//	output.Duration = (int)(( 30.0 + ( (float)(input.SkillLevel) * 3.0 ) ) * 10);
//	output.Duration = ( 60 + input.DEX / 10 * ( input.SkillLevel * 2 / 3 ) ) * 10;
//	output.Duration = min( 1200, output.Duration );
         output.Duration = 6000;
//	output.Delay = max ( 50 , output.Duration * 2 - ( input.DEX * 2 ) );
	//output.Delay = output.Duration - ( input.SkillLevel / 5 );
	output.Delay = 30;//fk cd
}

void SharpRound::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
//		output.Damage = (int)( ( input.STR / 9.0 ) * ( 1.0 + ( input.SkillLevel / 15.0 ) ) );
		output.Damage = (int)(14.0 + ( input.STR / 15.0 ) * ( 1.0 + ( input.SkillLevel / 15.0 )));
		output.Damage = min( 55, output.Damage );
	}
	else
	{
//		output.Damage = (int)( ( input.STR / 9.0 ) * ( 1.5 + ( input.SkillLevel / 30.0 ) ) );
		output.Damage = (int)(14.0 + ( input.STR / 15.0 ) * ( 1.5 + ( input.SkillLevel / 30.0 )));
		output.Damage = min( 55, output.Damage );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	output.Delay = 0;
}

void BackStab::computeOutput(const SkillInput& input, SkillOutput& output)
{
}

void Blunting::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = min(80,(int)(( (input.STR/15.0) + (input.DEX/10.0) ) * ( 1.0 + (input.SkillLevel/15.0) )));
	}
	else
	{
		output.Damage = min(80,(int)(( (input.STR/15.0) + (input.DEX/10.0) ) * ( 1.5 + (input.SkillLevel/30.0) )));
		if ( input.SkillLevel == 30 ) output.Damage = min( 80, (int)(output.Damage * 1.1) );
	}

	output.Duration = min( 300, (int)((( (input.DEX+input.INTE) / 20.0 ) + ( (input.SkillLevel/2.0) )) * 10) );
	if ( input.SkillLevel == 30 ) output.Duration = (int)(output.Duration * 1.1);

	output.Delay = max(40,(int)(( 8 - (input.DEX/50.0) ) * 10));
	output.Range = 1 + (input.SkillLevel/10);
}

void GammaChop::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 10 + (int)( ( input.STR / 10.0 ) * ( 1.0 + ( input.SkillLevel / 11.25 ) ) );
		output.Damage = min( 120, output.Damage );
	}
	else
	{
		output.Damage = 10 + (int)( ( input.STR / 10.0 ) * ( 5.0/3.0 + ( input.SkillLevel / 22.5 ) ) );
		output.Damage = min( 120, output.Damage );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}
	output.Delay = 10;//max(10,(int)(( 4 - ( input.DEX / 60.0 ) ) * 10));
	output.Range = 1 + (input.SkillLevel / 15);
}

void CrossGuard::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
//		output.Damage = (int)( ( input.STR / 20.0 ) * ( 1.0 + ( input.SkillLevel / 11.25 ) ) );
		output.Damage = 15 + ( input.SkillLevel * 14 / 9 );
	}
	else
	{
//		output.Damage = (int)( ( input.STR / 20.0 ) * ( 5.0/3.0 + ( input.SkillLevel / 22.5 ) ) );
		output.Damage = 15 + ( 35 / 3 + input.SkillLevel * 7 / 9 );
	}

	output.Damage = min( 50, output.Damage );
	output.Duration = (int)(( 60.0 + ( input.DEX / 10.0 ) + ( input.SkillLevel * 2.0 ) ) * 10);
	output.Duration = min( 1800, output.Duration );

	if ( input.SkillLevel == 30 ) output.Duration = (int)(output.Duration * 1.1);

	output.Delay = output.Duration;
}

void KasasArrow::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
//		output.Damage = 7 + (int)( ( input.INTE / 8.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 22.5 ) ) );
		output.Damage = 15 + (int)( ( input.INTE / 55.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 30.0 ) ) );
	}
	else
	{
//		output.Damage = 7 + (int)( ( input.INTE / 8.0 ) * ( 4.0/3.0 + ( (float)(input.SkillLevel) / 45.0 ) ) );
		output.Damage = 15 + (int)( ( input.INTE / 50.0 ) * ( 1.5 + ( (float)(input.SkillLevel) / 25.0 ) ) );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.2);
	}

	output.Delay = 0;
	output.Range = 3 + (input.SkillLevel / 10);
}

void HandsOfFire::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (int)( 5.0 + input.SkillLevel*15.0/30.0 );
	if ( input.SkillLevel <= 15 )
	{
//		output.Damage = (int)( ( 1 + input.INTE / 50.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 22.5 ) ) );
//		output.Damage = (int)( ( 7 + input.INTE / 12.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 11.25 ) ) );
	}
	else
	{
//		output.Damage = (int)( ( 1 + input.INTE / 50.0 ) * ( 4.0/3.0 + ( (float)(input.SkillLevel) / 45.0 ) ) );
//		output.Damage = (int)( ( 7 + input.INTE / 12.0 ) * ( 5.0/3.0 + ( (float)(input.SkillLevel) / 22.5 ) ) );
	}

//	output.Duration = (int)(( 60.0 + (input.DEX / 10.0 ) + (input.SkillLevel*1.5) ) * 10);
	//if ( input.SkillLevel == 30 ) output.Duration = (int)(output.Duration * 1.1);
	//output.Duration = min(1200, output.Duration);
        output.Duration = 6000;
	//output.Delay = output.Duration;
	output.Delay = 40;//fk cd
}

void Prominence::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
//		output.Damage = (int)( ( input.INTE / 30.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 15.0 ) ) );
//		output.Damage = 15 + (int)( ( input.INTE / 20.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 11.25 ) ) );
		output.Damage = 5 + (int)( ( input.INTE / 25.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 10.0 ) ) );
	}
	else
	{
//		output.Damage = (int)( ( input.INTE / 30.0 ) * ( 1.5 + ( (float)(input.SkillLevel) / 30.0 ) ) );
//		output.Damage = 15 + (int)( ( input.INTE / 20.0 ) * ( 5.0/3.0 + ( (float)(input.SkillLevel) / 22.5 ) ) );
		output.Damage = 5 + (int)( ( input.INTE / 25.0 ) * ( 2.0 + ( (float)(input.SkillLevel) / 25.0 ) ) );
	}

	output.Duration = (int)(( 5.0 + ( input.INTE / 30.0 ) + (input.SkillLevel/5.0) ) * 10);
	output.Delay = output.Duration - ( input.DEX / 20 );
	if ( input.SkillLevel == 30 ) output.Duration = (int)(output.Duration * 1.1);

	output.Range = 2 + (input.SkillLevel / 10);
	output.Tick = 10;
}

void RingOfFlare::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
//		output.Damage = (int)( ( input.INTE / 10.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 15.0 ) ) );
		output.Damage = (int)( 5 + ( input.INTE / 17.0 ) + ( 1.0 + ( (float)(input.SkillLevel) / 3.0 ) ) );
	}
	else
	{
//		output.Damage = (int)( ( input.INTE / 10.0 ) * ( 1.5 + ( (float)(input.SkillLevel) / 30.0 ) ) );
		output.Damage = (int)( 2 + ( input.INTE / 15.0 ) + ( 1.0 + ( (float)(input.SkillLevel) / 2.0 ) ) );
	}

	output.Duration = (int)(( 35.0 + ( input.INTE / 30.0 ) + (input.SkillLevel/3.0) ) * 10);
	if ( input.SkillLevel == 30 ) output.Duration = (int)(output.Duration * 1.1);
        //output.Duration = 3600;
	//output.Delay = output.Duration;
	output.Delay = 40;
}

void BlazeBolt::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
//		output.Damage = 10 + (int)( ( input.INTE / 13.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 11.25 ) ) );
		output.Damage = 20 + (int)( ( input.INTE / 20.0 ) + ( 1.0 + ( (float)(input.SkillLevel) / 3.0 ) ) );
	}
	else
	{
//		output.Damage = 10 + (int)( ( input.INTE / 13.0 ) * ( 5.0 / 3.0 + ( (float)(input.SkillLevel) / 22.5 ) ) );
		output.Damage = 20 + (int)( ( input.INTE / 20.0 ) + ( ( (float)(input.SkillLevel) / 2.5 ) ) );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.2);
	}

	output.Delay = 10;
	output.Range = 4 + (input.SkillLevel / 10);
}

void IceField::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Duration = (int)max(10, (int)min(300.0, ( ( input.INTE / 30.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 22.5 ) ) + 5.0 ) * 10));
		output.Range = max(20, min(150, output.Duration) );
	}
	else
	{
		output.Duration = (int)max(10, (int)min(300.0, ( ( input.INTE / 30.0 ) * ( 4.0/3.0 + ( (float)(input.SkillLevel) / 45.0 ) ) + 5.0 ) * 10));
		output.Range = max(20, min(150, output.Duration) );
	}

	output.Delay = (int)(output.Duration * 1.2 - (input.SkillLevel));
	output.Tick=10;
}

void WaterBarrier::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = (int)min(50,(int)( 10.0 + ( input.INTE / 20.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 15.0 ) ) ));
	}
	else
	{
		output.Damage = (int)min(70,(int)( 10.0 + ( input.INTE / 20.0 ) * ( 1.5 + ( (float)(input.SkillLevel) / 30.0 ) ) ));
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	output.Duration = (int)(( 30.0 + ( input.INTE / 10.0 ) * ( 1.0 + (float)(input.SkillLevel) / 20.0 ) ) * 10);
	output.Duration = min( 1800, output.Duration );
	if ( input.SkillLevel == 30 ) output.Duration = 6000;
	output.Delay = 100;
}

void NymphRecovery::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		if ( input.TargetType == SkillInput::TARGET_OTHER )
		{
			output.Damage = 15 + (int)( ( input.INTE / 13.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 15.0 ) ) );
		}
		else
		{
			output.Damage = 15 + (int)( ( input.INTE / 10.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 15.0 ) )*1.25 );
		}
	}
	else
	{
		if ( input.TargetType == SkillInput::TARGET_OTHER )
		{
			output.Damage = 15 + (int)( ( input.INTE / 13.0 ) * ( 1.5 + ( (float)(input.SkillLevel) / 30.0 ) )*1.3 );
		}
		else
		{
			output.Damage = 15 + (int)( ( input.INTE / 10.0 ) * ( 1.5 + ( (float)(input.SkillLevel) / 30.0 ) )*1.55 );
		}
	}
	if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1)*2.0;

	output.Delay = (int)(( 4 - ( input.SkillLevel / 15.0 ) ) * 10);
}

void Liberty::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = ( 8 - (input.SkillLevel/10) ) * 10;
}

void Tendril::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
//		output.Duration = (int)(( 3 + ( input.INTE / 30.0 ) * ( 1.0 + (input.SkillLevel / 22.5) ) ) * 10);
		output.Duration = ( 3 + input.SkillLevel/7 ) * 10;
	}
	else
	{
//		output.Duration = (int)(( 3 + ( input.INTE / 30.0 ) * ( 4.0/3.0 + (input.SkillLevel / 45.0) ) ) * 10);
		output.Duration = ( 5 + input.SkillLevel/15 ) * 10;
	}

	output.Delay = ( 9 - (input.SkillLevel/6) ) * 10;
	output.Range = 1 + (input.SkillLevel/10);
}

void StoneAuger::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 20 + (int)(( ( input.INTE / 30.0 ) * ( 1.0 + (input.SkillLevel / 15.0) ) ));
		output.Damage = min( 50, output.Damage );
	}
	else
	{
		output.Damage = 20 + (int)(( ( input.INTE / 30.0 ) * ( 1.5 + (input.SkillLevel / 30.0) ) ));
		output.Damage = min( 50, output.Damage );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	output.Delay = ( 4 - (input.SkillLevel/10) ) * 10;
}

void EarthsTeeth::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 15 + (int)( ( input.INTE / 14.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 11.25 ) ) );
		output.Damage = min( 80, output.Damage );
	}
	else
	{
		output.Damage = 15 + (int)( ( input.INTE / 13.0 ) * ( 5.0/3.0 + ( (float)(input.SkillLevel) / 11.25 ) ) );
		output.Damage = min( 130, output.Damage );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	output.Delay = ( 2 - (input.SkillLevel / 10) ) * 10;
	output.Delay = max( output.Delay, 6 );
	output.Range = 3 + (input.SkillLevel / 15);
}

void HandsOfNizie::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 10 + (int)( ( input.INTE / 18.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 15.0 ) ) );
	}
	else
	{
		output.Damage = 10 + (int)( ( input.INTE / 18.0 ) * ( 1.5 + ( (float)(input.SkillLevel) / 30.0 ) ) );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	output.Delay = 6;
}

void GnomesWhisper::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Duration = min( 60, (int)( ( input.INTE / 5.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 11.25 ) ) ) );
	}
	else
	{
		output.Duration = min( 180, (int)( ( input.INTE / 5.0 ) * ( 5.0/3.0 + ( (float)(input.SkillLevel) / 22.5 ) ) ) );
		if ( input.SkillLevel == 30 ) output.Duration = min( 210, (int)(output.Duration * 1.1) );
	}

	output.Duration *= 10;
	output.Delay = output.Duration;
}

void RefusalEther::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = ( 7 - (input.SkillLevel/15) ) * 10;
	// 무조건 3X3으로 설정. by Taehyoun 2005.11.14
	output.Range = 1;
}

void EmissionWater::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 10 + (int)( ( input.INTE / 50.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 20.0 ) ) );
	}
	else
	{
		output.Damage = 10 + (int)( ( input.INTE / 55.0 ) * ( 2.0 + ( (float)(input.SkillLevel) / 15.0 ) ) );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	output.Delay = 10;
}

void BeatHead::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage =  min( 70, 10 + (int)( ( input.INTE / 10.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 22.5 ) ) ) );
	}
	else
	{
		output.Damage = min( 90, 10 + (int)( ( input.INTE / 10.0 ) * ( 1.3 + ( (float)(input.SkillLevel) / 22.5 ) ) ) );
		if ( input.SkillLevel == 30 ) min( 100, output.Damage = (int)(output.Damage * 1.1) );
	}

	output.Delay = 0;
}

void DivineSpirits::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 10 + (int)( (float)input.SkillLevel * 4.0 / 3.0 );
	}
	else
	{
		output.Damage = 20 + (int)( (float)input.SkillLevel * 2.0 / 3.0 );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	output.Duration = 30 + input.SkillLevel * 2;
	output.Duration *= 10;
	output.Delay = 30;
}

void BlitzSliding::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 4*(1+input.SkillLevel/25);
	output.Delay = ( 4 - (input.SkillLevel/50) ) * 10;
}

void JabbingVein::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = -30+(input.STR/20+input.SkillLevel/3);
	output.Delay = 2;

	if ( input.TargetType == SkillInput::TARGET_PC )
	{
		output.Range = 2+(input.SkillLevel/33);
		output.Duration = ( 1+ (input.SkillLevel/50) )*10;
	}
	else
	{
		output.Range = 5+(input.SkillLevel/20);
		output.Duration = ( 2+ (input.SkillLevel/25) )*10;
	}
}

void GreatHeal::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER) 
	{
		output.Damage = 100 + input.SkillLevel/3;
	}
	else
	{
		output.Damage = 110 + input.SkillLevel/3;
	}

	output.Delay = (5 - input.SkillLevel/33) * 10;

	// 파티 보너스를 계산한다.
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void DivineGuidance::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if ( input.TargetType == SkillInput::TARGET_PC )
	{
		output.Damage = 15 + ( 2 * input.SkillLevel/25 );
		output.Tick = 15;
		output.ToHit = ( input.INTE/5 ) + ( input.SkillLevel/10 );
		//output.Range = min( output.Range, 70 );
	}
	else
	{
		output.Damage = 20 + ( 3 * input.SkillLevel/20 );
		output.Tick = 10;
		output.ToHit = ( input.INTE/3 ) + ( input.SkillLevel/10 );
		//output.Range = min( output.Range, 90 );
	}

	output.Range = 3 + ( input.SkillLevel / 50 );
	output.Delay = ( 10 + input.SkillLevel/20 ) * 10;
	output.Duration = ( 10 + input.SkillLevel/20 ) * 10;
}

void BlazeWalk::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = ( 5 - input.SkillLevel/50 ) * 10;
	output.Damage = 2 * ( 1 + input.SkillLevel/25 );
}

void BloodyZenith::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = ( 7 - ( input.STR/100 + input.DEX/220 + input.INTE/330 ) ) * 10;
	output.Delay = max( 30, output.Delay );
//	output.Damage = 20 + input.STR/8 + input.DEX/20;
	output.Damage = min(150,30 + input.STR/3 + input.DEX/10 + input.INTE/20);
}

void Rediance::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Duration = ( 60 + input.SkillLevel*3/2 ) * 10;
	output.Duration = 6000;
	output.Delay = 40;
	output.Damage = 7 + input.SkillLevel/20 + input.DomainLevel/10;
}

void LarSlash::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 4;
	output.Damage = 5 + input.STR/10 + input.SkillLevel/8;
}

void Trident::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.ToHit  = -30 + input.DEX/20 + input.SkillLevel/4;
	output.Damage = -30 + input.STR/20 + input.SkillLevel/4;
	output.Delay  = 2;
}

void HeartCatalyst::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Damage = 2 + input.SkillLevel/20;
        output.Damage = 30;
	output.Tick = 20;
	output.Duration = 6000;
	output.Delay = 200;
}

void ProtectionFromBlood::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// by sigi. 2002.12.3
	if (input.TargetType == SkillInput::TARGET_OTHER)
	{
		//output.Damage = 10 + input.SkillLevel/10;
		output.Damage = min(20, 10 + input.INTE/20);
	}
	else 
	{
		//output.Damage = 20 + input.SkillLevel/10;
		output.Damage = min(30, 20 + input.INTE/20);
	}

	//output.Duration = (20 + input.SkillLevel/2) * 10; // 20~70초
	output.Duration = (60 + input.INTE/2 + input.SkillLevel/2) * 10; // 20~70초
	output.Delay    = (5 - input.SkillLevel/33) * 10; // 5->2초

	// 파티 보너스를 계산한다.
	output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void MoleShot::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.IClass == Item::ITEM_CLASS_SG)
	{
		output.Damage = 3 + input.SkillLevel/10;
	}

	else if(input.IClass == Item::ITEM_CLASS_AR ||
			input.IClass == Item::ITEM_CLASS_SMG)
	{
		output.Damage = 1 + input.SkillLevel/30;
	}

	else if(input.IClass == Item::ITEM_CLASS_SR)
	{
		output.Damage = input.SkillLevel/50;
	}

	output.ToHit  = -20 + input.SkillLevel/5;
	output.Delay  = 0;
}

void Eternity::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = -50 + (input.SkillLevel/2);
//	output.Range = 50 + (input.SkillLevel/3);
	output.Duration = 50;
}

void InstallTrap::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 600;
	output.Tick = (5 + ( input.SkillLevel/25 ))*10;
	output.Delay = (60 - (int)(input.SkillLevel/2.5)) * 10;
}

void HolyArmor::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = (30+(input.SkillLevel/2))*10;
	output.Damage = 10 + (input.INTE/20) + (input.SkillLevel/10);
	output.Delay = output.Duration;
}

void MercyGround::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Duration = (10 + input.INTE/20 + input.SkillLevel/10)*10;
	output.Duration = (15 + input.INTE/15 + input.SkillLevel/6)*10;
	output.Delay = (10 - input.SkillLevel/33) * 10;
}

void CreateHolyPotion::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = (5 - input.SkillLevel/33) * 10;
}

void TransformToWerwolf::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Delay = 50;
//	2007 06 22
	output.Delay = 900;
	output.Duration = 3000;
}

void GrayDarkness::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = min( 700, ( 10 + input.INTE/8 ) * 10 );
	output.Delay    = max( 4,  15 - ( input.DEX/50 + input.INTE/60 ) ) * 10;
//	output.Range    = (  3 + (input.SkillLevel-15) / 25);
}

void StoneSkin::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = min(80, 10 + input.STR/20 + input.INTE/5) * 10;
	output.Delay	= min(80, 10 + input.STR/20 + input.INTE/5) * 10;
	output.Damage	= min(30, 20 + input.INTE/20);
}

void TalonOfCrow::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay	= 6;
	output.Damage	= input.STR/6 + input.INTE/30;
}

void Howl::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration	= min( 120, 30 + input.STR/4 + input.DEX/3 + input.INTE/6 ) * 10;
	output.Range	= min( 100, 60 + input.DEX/20 );
}

void AcidEruption::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage	= min(180,(input.STR/6) + (input.INTE/2) + (input.DEX/10));
	output.Delay	= max(3, 6-(input.DEX/50)) * 10;
}

void Teleport::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Range	= min(6, 3 + input.SkillLevel/10);
	output.Delay = max(1, 5-(input.DEX/60)-(input.INTE/150)) * 10;
}

void FirePiercing::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 30 + ( input.INTE/20 + (3 + input.SkillLevel/4) );
	}
	else
	{
		output.Damage = 30 + ( input.INTE/20 + (2 + input.SkillLevel/3) );
	}

	if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.2);

	output.Delay=0;
}

void SoulRebirth::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = ( 8 - input.SkillLevel/10 ) * 10;
	output.Range = 2 + input.SkillLevel/10;
	
	if ( input.SkillLevel <= 15 )
	{
		output.Tick = 20 + input.SkillLevel*2;
	}
	else
	{
		output.Tick = 50 + input.SkillLevel/2;
	}
}

void IceLance::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = (int)(28 + (input.INTE/25) * ( 1.2 + (input.SkillLevel/12.0) ));
	}
	else
	{
		output.Damage = (int)(28 + (input.INTE/22) * ( 1.0 + (input.SkillLevel/13.0) ));
	}

	output.Range = 4 + input.SkillLevel/10;
}

void FrozenGlacier::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (10 + (int)(input.INTE/3)) * (11 + (int)((input.Range - 1) / 10)) / 10;

	output.Range = 7;
}

void ExplosionWater::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = (int)(20 + (input.INTE/30.0) + ( 3.0 + (input.SkillLevel/4.0) ));
	}
	else
	{
		output.Damage = (int)(20 + (input.INTE/23.0) + ( 3.0 + (input.SkillLevel/3.0) ));
	}

	output.Range = 4 + input.SkillLevel/10;
	output.Duration = ( 2 + ( input.SkillLevel/10 ) ) * 10;
	output.Delay = ( 15 - ( input.SkillLevel/6 ) ) * 10;

	// 데미지 감소율
	output.Tick = 50 - ( input.SkillLevel/3 );
}

void FrozenArmor::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = (int)min(20,(int)( 10.0 + ( input.INTE / 40.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 15.0 ) ) ));
		output.Tick = max( 20, (int)min(15,(int)( 5.0 + ( input.INTE/30.0 ) * ( 1.0 + ( (float)(input.SkillLevel)/22.5) ) ) ) * 10 ); 
	}
	else
	{
		output.Damage = (int)min(20,(int)( 10.0 + ( input.INTE / 40.0 ) * ( 1.5 + ( (float)(input.SkillLevel) / 30.0 ) ) ));
//		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
		output.Tick = max( 20, (int)min(15,(int)( 5.0 + ( input.INTE/30.0 ) * ( 4.0/3.0 + ( (float)(input.SkillLevel)/45.0) ) ) ) * 10 ); 
	}

	output.Duration = (int)(( 20.0 + ( input.INTE / 20.0 ) * ( 1.0 + (float)(input.SkillLevel) / 20.0 ) ) * 10);
	if ( input.SkillLevel == 30 ) output.Duration = (int)(output.Duration * 1.1);
	//output.Duration = min( 600, output.Duration );
        output.Duration = 6000;//fk cd
	//output.Delay = output.Duration;
	output.Range=1;
	output.Delay = 40;//fk cd
}

void ReactiveArmor::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = (int)min(30,(int)( 5.0 + ( input.INTE / 30.0 ) * ( 1.1 + ( (float)(input.SkillLevel) / 20.0 ) ) ));
	}
	else
	{
		output.Damage = (int)min(45,(int)( 5.0 + ( input.INTE / 30.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 15.0 ) ) ));
//		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	//output.Duration = (int)(( 20.0 + ( input.INTE / 20.0 ) * ( 1.0 + (float)(input.SkillLevel) / 4.0 ) ) * 10);
	output.Duration = 1800;
	if ( input.SkillLevel == 30 ) output.Duration = 6000;
	//output.Duration = min( 1800, output.Duration );
	output.Delay = max( 2, 5 - (input.INTE-20) / 10 ) * 10;
}


void MagnumSpear::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage =(int)( 40.0 + ( input.INTE / 10.0 ) + ( 3.0 + (float)(input.SkillLevel) / 4.0 ));

	}
	else
	{
		output.Damage = (int)( 40.0 + ( input.INTE/ 6.0 ) + ( 3.0 + (float)(input.SkillLevel) / 3.0 ));
	}

	output.Range =  (int)( 4.0 + (float)(input.SkillLevel) / 10 );
	
	output.Delay = 11;
		
}

void MagnumSpear::computeOutputByCreature(PlayerCreature* pPC, SkillOutput& output)
{
	if ( pPC->isOusters() )
	{
		Ousters *pOusters = dynamic_cast<Ousters *>(pPC);
		
		if ( pOusters->hasSkill( SKILL_MAGNUM_SPEAR_MASTERY ) != NULL )
		{
			output.Delay = 20;
		}
	}

}
//20071227 - 헬파이어 수정
//		Duration 값을 100(10초) 동안으로 고정한다.
//		Tick값을 20(2초)로 고정한다.
//			Tick값은 EffectHellFire의 affect 주기로 사용될 것이다.
//			즉, 2초 주기로 EffectHellFire에서 Damage를 주도록 수정된다.
//			기존에는 HellFire 스킬이 발동되는 순간에 Damage를 한방에 줬었다.
//		ToHit값은 수정되면서 의미가 없어질 것이다.
//			여기서 계산된 output.Damage 값은 EffectHellFire의 Damage로 설정될 것이다.
//				기존에는 ToHit값이 EffectHellFire의 Damage로 설정되어, 이 값을 EffectHellFireToEnemy의 Damage값으로 지정했었다.
//			EffectHellFire에 설정된 Damage값은 EffectHellFire::affect함수에서, 해당 타일에 위치하는 크리쳐에게 0.4배로 Damage를 적용하게 될 것이다.
//			또한, EffectHellFire에서 해당 타일에 위치하는 크리쳐에게 붙이는 EffectHellFireToEnemy의 Damage값에 1/7배로 설정할 것이다.
//				기존에는 EffectHellFireToEnemy에 의한 Damage가 너무 컸으며, 이를 다소 줄이기 위함이다.
void HellFire::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage =(int)( 30.0 + ( input.INTE / 8.0 ) + ( 1.0 + (float)(input.SkillLevel) / 2.0 ));
		output.Range = (4+ (int)( input.INTE / 150 ) + (int)( (input.SkillLevel) / 15 ) )*10; // 20080131

	}
	else
	{
		output.Damage = (int)( 30.0 + ( input.INTE/ 6.0 ) + ( 1.0 + (float)(input.SkillLevel) ));
		output.Range=(4+ (int)( input.INTE / 150 ) + (int)( (input.SkillLevel) / 15 ) )*10; //20080131
	}
	output.Duration = 95;
	output.ToHit = 0;	//사용되지 않는다.
	output.Delay = max(80, 130 - (int)( input.INTE / 10 )); // 20080131

	output.Tick=20;
}
/* -- 아래는 20071227 헬파이어 수정 전의 코드다.
void HellFire::computeOutput(const SkillInput& input, SkillOutput& output)
{
if ( input.SkillLevel <= 15 )
{
output.Damage =(int)( 30.0 + ( input.INTE / 8.0 ) + ( 1.0 + (float)(input.SkillLevel) / 2.0 ));
//		output.Duration = (int)max(10, (int)min(100.0, ( ( input.INTE / 30.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 22.5 ) ) + 5.0 ) * 10));
output.Duration = (int)(( 6.0 + (input.INTE/60.0) + (input.SkillLevel/15.0) ) * 10);
output.Range = max(20, min(150, (int)(( ( input.INTE / 30.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 22.5 ) ) + 5.0 ) * 10)) );
}
else
{
output.Damage = (int)( 30.0 + ( input.INTE/ 6.0 ) + ( 1.0 + (float)(input.SkillLevel) ));
//		output.Duration = (int)max(10, (int)min(100.0, ( ( input.INTE / 30.0 ) * ( 4.0/3.0 + ( (float)(input.SkillLevel) / 45.0 ) ) + 5.0 ) * 10));
output.Duration = (int)(( 6.0 + (input.INTE/60.0) + (input.SkillLevel/10.0) ) * 10);
output.Range = max(20, min(150, (int)(( ( input.INTE / 30.0 ) * ( 1.0 + ( (float)(input.SkillLevel) / 45.0 ) ) + 5.0 ) * 10)) );
}
// ToHit 을 Speed Damage 로 사용
output.ToHit = (int)(5.0 + ( input.INTE / 50.0 ) * (1.0 + (input.SkillLevel / 15.0)) );
output.Delay = (int)max( 40.0 , (output.Duration * 1.2 - (input.SkillLevel * 10)) );
output.Tick=5;
}
*/

void GroundBless::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = (int)( 1.0 + (input.STR + input.DEX + input.INTE)/100.0 + 0.5 + input.SkillLevel/10.0);
	}
	else
	{
		output.Damage = (int)( 1.0 + (input.STR + input.DEX + input.INTE)/100.0 + 1.0 + input.SkillLevel/12.0);
	}

	output.Duration = (int)(( 20.0 + ( input.INTE / 20.0 ) * ( 1.0 + (float)(input.SkillLevel) / 4.0 ) ) * 10);
	output.Duration = min( 3000, input.INTE*3 );
	output.Delay = max( 2, 5 - (input.INTE-20) / 10 ) * 10;
}

void SharpChakram::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = (int)((11 + input.SkillLevel/6)*1.3);
	}
	else
	{
		output.Damage = (int)((11 + input.SkillLevel/4)*1.3);
//		
	}
        if ( input.SkillLevel == 30 )
        {
         output.Damage = 30;
        }
	output.Duration = 450 + input.DEX/2 + input.SkillLevel*15;
	output.Duration = min( 900, output.Duration );
	if ( input.SkillLevel == 30 ) output.Duration = 6000;
	//output.Delay = output.Duration;
	output.Delay = 30;//fk cd
}

void DestructionSpear::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = min( 60, 15 + (input.DEX/20) + (input.SkillLevel/3) );
		output.Duration = max(20,min(150,(int)(( 5.0 + (input.DEX/30.0) * ( 1.0 + (input.SkillLevel/22.5) ) ) * 10)));
	}
	else
	{
		output.Damage = min( 60, 15 + (input.DEX/20) + (input.SkillLevel/2) );
		output.Duration = max(20,min(150,(int)(( 5.0 + (input.DEX/30.0) * ( 4.0/3.0 + (input.SkillLevel/45.0) ) ) * 10)));
	}

	output.Delay = 30 - input.DEX/6 - input.SkillLevel;
	output.Delay = max( output.Delay, 6 );
}

void Brionac::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (30 + (input.DEX / 10.0)) * (11 + (int)((input.Range - 1) / 10)) / 10;
	output.Duration = max( 20, min(150, (int)(( 5.0 + (input.DEX/30.0) * 2.0 ) * 10.0)));

	output.Delay = 6;
}

void ShiftBreak::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 5 + input.STR/20 + input.DEX/20 + input.SkillLevel/3;
	}
	else 
	{
		output.Damage = 5 + input.STR/20 + input.DEX/20 + input.SkillLevel/2;
	}

	if ( input.SkillLevel == 30 )
		output.Damage = (int)(output.Damage * 1.1);

	output.Delay = max(6, (int)(30 - (input.DEX/12) - (input.SkillLevel/1.5))); 
	output.Range = 1;
}

void FatalSnick::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = min( 160, 20 + (int)( ( input.STR / 10.0 ) * ( 1.0 + ( input.SkillLevel / 11.25 ) ) ) );
	}
	else
	{
		output.Damage = min( 160, 20 + (int)( ( input.STR / 10.0 ) * ( 5.0/3.0 + ( input.SkillLevel / 22.5 ) ) ) );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}
//	output.Delay = max(20, (4 - (input.DEX/60)) * 10 );
	output.Delay = 10;
	output.Range = 1 + (input.SkillLevel / 15);
}

void ChargingAttack::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 20 + (int)( (input.STR/10.0 ) * ( 1.0 + (input.SkillLevel/11.25)) );
	}
	else
	{
//		output.Damage = 20 + (int)( (input.STR/10.0 ) * ( 5.0/3.0 + (input.SkillLevel/22.5)) );
//		2007 06 22
		output.Damage = 20 + (int)( (input.STR/10.0 + input.DEX/10.0 ) * ( 5.0/3.0 + (input.SkillLevel/22.5)) );
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	output.Duration = (4 + (input.SkillLevel/30.0)) * 10;
//	output.Delay = max(50, (10 - (input.DEX/60) - (input.SkillLevel/10) ) * 10 );
	output.Delay = 300;
	output.Range =  3 + (input.SkillLevel / 10);
}

void DuckingWallop::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 20 + (input.DEX/10) * (1 + (input.SkillLevel/11.25));
	}
	else
	{
		output.Damage = 20 + (input.DEX/10) * (5.0/3.0 + (input.SkillLevel/22.5));
	}

	output.Delay = max(20,100 - (input.DEX/6) - input.SkillLevel);
}

void DistanceBlitz::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 20 + (input.STR+input.DEX)/30 * (1.0 + (input.SkillLevel/15.0));
	}
	else
	{
		output.Damage = 20 + (input.STR+input.DEX)/30 * (5.0/3.0 + (input.SkillLevel/10.0));
	}

	output.Delay = ( 3 - (input.SkillLevel/15.0) ) * 10;
	output.Delay = max( 10, output.Delay );
}

void SummonGroundElemental::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = ( 10 + input.INTE/8 + input.SkillLevel ) * 10;
	output.Delay = output.Duration / 2;
}

void SummonFireElemental::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Duration = ( 10 + input.INTE/8 + input.SkillLevel*2 ) * 10;
	//output.Duration = min( output.Duration, 1200 );
        output.Duration = 6000;
	output.Delay = 300;

	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 12 + (input.INTE/20) + (input.SkillLevel/4);
	}
	else
	{
		output.Damage = 12 + (input.INTE/15) + (input.SkillLevel/3);
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage*1.2);
	}

	output.Range = 4 + input.SkillLevel/10;
}

void SummonWaterElemental::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Duration = ( 5 + input.INTE/8 + input.SkillLevel*2 ) * 10;
	//output.Duration = min( output.Duration, 1200 );
        output.Duration = 6000;//fk cd
	output.Delay = 300;

	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 8 + (input.INTE/30.0) * (1 + input.SkillLevel/45.0)*2;
		output.Damage = min(output.Damage,180);
	}
	else
	{
		output.Damage = 8 + (input.INTE/40.0) * (1.5 + input.SkillLevel/30.0)*2;
		output.Damage = min(output.Damage,180);
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage*1.1);
	}
}

void MeteorStorm::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 50 + (int)(( ( input.INTE / 10.0 ) + ( 1.0 + (input.SkillLevel / 6.0) ) ));
	}
	else
	{
		output.Damage = 50 + (int)(( ( input.INTE / 5.0 ) + ( 1 + (input.SkillLevel / 3.0) ) ));
		if ( input.SkillLevel == 30 ) output.Damage = (int)(output.Damage * 1.1);
	}

	output.Delay = ( 8 - (input.SkillLevel/5) ) * 10;
}

void AsteroidBomber::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (20 + (int)( input.INTE / 1.8 )) * (11 + (int)((input.Range - 1) / 10)) / 10; 
	output.Delay = 25;
}

void AsteroidBomber::computeOutputByCreature(PlayerCreature* pPC, SkillOutput& output)
{
	if ( pPC->isOusters() )
	{
		Ousters *pOusters = dynamic_cast<Ousters *>(pPC);
		
		OustersSkillSlot* pMeteorSkillSlot = pOusters->hasSkill(SKILL_METEOR_STORM);
		if ( pMeteorSkillSlot != NULL )
		{
			output.Delay = (8 - (pMeteorSkillSlot->getExpLevel() / 5)) * 10;
		}
	}
}

void Incinerate::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (10 + (int)( input.INTE / 1.8 )) * (11 + (int)((input.Range - 1) / 10)) / 10;
	output.Delay = 25;
}

void WideIceField::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 200;
	output.Range = 200;
	output.Tick = 5;
}

void Glacier1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 50;
}

void Glacier2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 100;
}

void IceAuger::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 300+(input.INTE*3);
	output.Delay = 10;
}

void IceHail::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 30;
	output.Damage = 50;
	output.Tick = 5;
}

void WideIceHail::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 50;
	output.Damage = 50+(input.INTE*2);
	output.Tick = 5;
}

void IceWave::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 320+(input.INTE*1);
	output.Delay  = 10;
}

void LandMineExplosion::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 320;
	output.Delay  = 10;
}

void ClaymoreExplosion::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 320;
	output.Delay  = 10;
}

void PleasureExplosion::computeOutput(const SkillInput& input, SkillOutput& output) 
{
}

void DeleoEfficio::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay=150;
}
void ReputoFactum::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration=300;
	output.Delay=200;
}

void SwordOfThor::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Damage = 80 + input.STR/2 + input.SkillLevel/2;
	output.Damage = min(200, input.STR/5 + input.SkillLevel/3);
	//output.Delay = 150;
	output.Delay = 134 - (input.SkillLevel/2);
	output.Duration = 150 + input.SkillLevel;
}

void MagmaDetonation::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (30 + (input.STR/10));
	output.Range = 7;
	output.Delay = 20;
	output.Duration = 10;
}

void MagmaDetonation2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (140 + (input.STR/10));
	output.Range = 7;
	output.Delay = 20;
	output.Duration = 10;
}

void SquallyBarrier1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = 450;
	output.Duration = 600;
}

void SquallyBarrier2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = 400;
	output.Duration = 600;
}

void BurningSolCharging::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = 0;
}

void BurningSolLaunch::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = 0;

	switch ( input.Range )
	{
		case 0:
			output.Damage = 35 + (input.STR/6) + (input.SkillLevel/3);
			break;
		case 1:
			output.Damage = 70 + (input.STR/6) + (input.SkillLevel/3);
			break;
		case 2:
			output.Damage = 120 + (input.STR/5) + (input.SkillLevel/2);
			break;
		default:
			output.Damage = 200 + (input.STR/5) + (input.SkillLevel/2);
			break;
	}
}

void SweepVice::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = 20;

	switch ( input.Range )
	{
		case 0:
			output.Damage = 40 + (input.INTE/5) + (input.SkillLevel/3);
			break;
		case 1:
			output.Damage = 30 + (input.INTE/5) + (input.SkillLevel/3);
			break;
		case 2:
		default:
			output.Damage = 20 + (input.INTE/5) + (input.SkillLevel/3);
			break;
	}
}

void Whitsuntide::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 50 + (input.SkillLevel/2);
	output.Range = 20 + (input.SkillLevel/10);
//	output.Delay = 80 - input.SkillLevel/3;
	//output.Delay = 100 - input.SkillLevel/5;
	output.Delay = ( 40 - input.SkillLevel/10 ) * 10;	// 40초 - 30초//fk
	//output.Delay = 70;	// xuese
	output.Duration = 300;
}

void ViolentPhantom::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Damage = 5 + input.STR/30 + input.DEX/60;
	output.Damage = 5 + input.STR/30 + input.DEX/60;
	output.Delay = 0;
}

void LethalClaw::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (5 + (input.STR/30) + (input.DEX/13)) * (11 + (int)((input.Range - 1) / 10)) / 10 ;
	output.Delay = 0;
}

void InstallTurret::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 8 + (input.SkillLevel/7);
	output.ToHit = 40 + (input.SkillLevel/10);
//	output.Delay = 330 + input.SkillLevel*2;
	output.Delay = 100 - input.SkillLevel/33*10;
	output.Duration = 900 + input.SkillLevel*20;
}

void TurretFire::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.ToHit  = 0;
	output.Damage = -10 + (input.SkillLevel/5);
}

void SummonGoreGland::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = input.INTE * 10 / 25 + 50;
	output.Delay = output.Duration;
}

void GoreGlandFire::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 20 + input.INTE/10;
	output.Damage = min( 80, output.Damage );
}

void ThrowingAxe::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.INTE/1;  //Random(870, 1000);
	output.Duration = 20; 	// 몇 초후 폭발
	output.Delay = 10;
}

void ChoppingFirewood::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.INTE/1;  //Random(870, 1000);
	output.Duration = 10; 	// 몇 초후 폭발
	output.Delay = 10;
}

void ChainThrowingAxe::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.INTE/1.6; //Random(500, 650);
	output.Duration = 20; 	// 몇 초후 폭발
	output.Delay = 10;
}

void MultiThrowingAxe::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.INTE/1; //Random(870, 1000);
	output.Duration = 20; 	// 몇 초후 폭발
	output.Delay = 10;
}
void PlayingWithFire::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.INTE/2.5; // 400;
	output.Duration = 20; 	// 몇 초후 폭발
	output.Delay = 0;
}

void InfinityThunderbolt::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Damage = 40 + (input.SkillLevel/2);
	output.Damage = 10 + (input.SkillLevel/5);
	output.Duration = 0;
	output.Delay = 4;
}

void HeavenlySpark::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 60 + (int)((input.Range - 1) / 10) * 10;
	output.Duration = 0;
	output.Delay = 4;
}

void SpitStream::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 30 + (input.STR/10) + (input.SkillLevel/5);
	output.Duration = 4; 	// 몇 초후 폭발
	output.Delay = 0;
}

void BladeStorm::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = (80 + (input.STR/8)) * (11 + (int)((input.Range - 1) / 10)) / 10;
	output.Delay = 0;
}

void PlasmaRocketLauncher::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min( 120, (input.STR/10) + (input.SkillLevel/3) );
	// 본서버 패치용
	//	20070705
//	output.ToHit  = 10 + (input.STR/10) + (input.SkillLevel/2);
//	2007 07 05
	output.ToHit  = (input.STR/12) + (input.SkillLevel/5);
	output.Duration = (input.Range-1) * 3;
	output.Delay = 0;
}

void ExplosionRocketLauncher::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (30 + (input.DEX/10)) * (11 + (int)((input.Range - 1) / 10)) / 10;
	output.ToHit  = 28 + (input.STR/12);
	output.Duration = (input.Distance-1) * 3;
	output.Delay = 0;
}

void BombingStar::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 40 + (input.INTE/5)+(input.SkillLevel/2);
	output.Delay = 20;
}

void Genesis::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (10 + (input.INTE / 2.5)) * (11 + (int)((input.Range - 1) / 10)) / 10*0.7;//fk
      //  output.Damage = (10 + (input.INTE / 4.0)) * (11 + (int)((input.Range - 1) / 10)) / 10;//xue se
	output.Delay = 20;
}

void IntimateGrail::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if(input.TargetType == SkillInput::TARGET_SELF)
		output.Duration = 90 + (input.INTE/5) + (input.SkillLevel/2);
	else
		output.Duration = 10 + (input.INTE/100.0) + (input.SkillLevel/15.0);

	output.Duration *= 10;

	output.Delay = 15 - (input.SkillLevel/30);
	output.Delay *= 10;
	output.Range = 4 + (input.SkillLevel/50);
}

void NooseOfWraith::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min(30 + (input.INTE/3), 200);
	output.Delay = 18;
	output.Range = 7;
}

void SetAfire::computeOutput(const SkillInput& input, SkillOutput& output) 
{
        int abc;
        abc = ( 13 - ( input.STR/60 + input.DEX/100 ) ) * 10;
        if (abc <0)
        {
          output.Delay = 0;
         }
        else
        {
	output.Delay = ( 13 - ( input.STR/60 + input.DEX/100 ) ) * 10;
        }
	//output.Damage = 10 + (input.STR/8) + (input.DEX/12);
	output.Damage = 20 + (input.STR/6) + (input.DEX/10);
}

void SharpHail::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20;
	
	if ( input.SkillLevel <= 15 )
	{
		output.Damage = 30 + ((input.DEX+input.STR)/20.0) * (1 + (input.SkillLevel/15.0)*1.5);
	}
	else
	{
		output.Damage = 30 + ((input.DEX+input.STR)/20.0) * (5.0/3.0 + (input.SkillLevel/10.0)*1.5);
	}
	output.Tick = 3;
	output.Duration = 10;
}
void IceHorizon::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = (5 + (input.INTE/25)+(input.SkillLevel/3))*10;
	output.Delay = output.Duration;
	output.Tick = 50;

	if( input.SkillLevel <= 15)
	{
		output.Damage = max(150, (int)(20 + (input.INTE/15.0) + (3 + (input.SkillLevel/4.0))) );;
	}
	else
	{
		output.Damage = max(250, (int)(20 + (input.INTE/10.0) + (3 + (input.SkillLevel/3.0))) );
	}
}

void FuryOfGnome::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if( input.SkillLevel <= 15 )
	{
		output.Duration = min(10, (int)(5 * ( 1+ (input.SkillLevel/22.5))) );
		output.Damage   = 30 + (input.INTE/10.0) + (1 + (input.SkillLevel/6.0));
	}
	else
	{
		output.Duration = min(10, (int)(5 * ( 4.0/3.0 + (input.SkillLevel/45.0))) );
		output.Damage   = 50 + (input.INTE/5.0) + (1 + (input.SkillLevel/3.0));
	}
	output.Duration *= 10;
	output.Delay = (output.Duration) - (input.SkillLevel);
}

void SummonMiga::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 15 + (input.SkillLevel/2);
	output.Duration *= 10;

	output.Delay = 80 - input.SkillLevel;
}

void SummonMigaAttack::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if ( input.DEX <= 15 )
	{
		output.Damage = 40 + (int)((input.INTE/8.0) + (1+(((float)input.DEX)/6.0)));
	}
	else
	{
		output.Damage = 40 + (int)((input.INTE/6.0) + (1+(((float)input.DEX)/3.0)));
	}
	
}

void Cannonade::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 52 + (rand()%30);
	output.Duration = 20;
}

void SelfDestruction::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay=0;
	output.Damage=110 + (rand()%40);
}

void ARAttack::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay=0;
	output.Damage=0;
}

void SMGAttack::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay=0;
	output.Damage=0;
}

void GrenadeAttack::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration=10;
	output.Damage=80;
}

void Halo::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min(200,80 + (input.STR/7) + (min(input.Range,10)*5));
	output.Range = 60 + (min(input.Range,10)*3);
	output.Duration = ( 5 + (input.DEX/100.0) * 3 ) * 10;
	output.Delay = output.Duration;
}

void Destinies::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 70 + (input.INTE/10) + (min(input.Range,10)*4);
	output.Delay = 200;
}

void FierceFlame::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Damage = (input.INTE/8) + min(input.Range,10); // 20080131
	output.Damage = (10 + input.INTE/16) + min(input.Range,10); // 20080229
	output.Delay = max(80, 130 - (int)( input.INTE / 10 )); // 20080131
	output.Duration = (5 + (int)floor(min((int)floor(input.Range/5.0),10)/2.0)) * 10;
}

void ShadowOfStorm::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min(240,40 + (input.INTE/30) + min(input.Range,10));
	output.Duration = 50 + (min(input.Range,10)*5);
	output.Delay = 50;
}

void WildWolf::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.DEX/8) + (input.STR/30) + min(input.Range,10)*2;
	output.Delay = max(5,(10 - (input.DEX/200))) * 10;
}

void Aberration::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Duration = (10 + input.INTE/40 + min(input.Range,10)*2)*10;
	//output.Duration = (5 + input.INTE/80 + min(input.Range,10))*10;
	output.Duration = ( input.INTE/80 + input.Range/2 )*10;
	//output.Delay = output.Duration;
	//output.Delay = max( 200, ( 40 - ( input.INTE/50 + input.Range ) ) * 10 );
	output.Delay = max( 300, ( 50 - ( input.INTE/50 + input.Range ) ) * 10 );
	//output.Range = min(40,30 + input.INTE/40);
}

void DragonTornado::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 30 + (input.STR/15) + (min(input.Range,10)*2);
	output.Range = min(200,80 + (input.STR/10) + (min(input.Range,10)*4));
	output.Tick = min(200,50 + (input.STR/10) + (min(input.Range,10)*4));
	output.Duration = (20 + min(input.Range,10))*10;
	output.Delay = 300;
}

void BikeCrash::computeOutput(const SkillInput& input, SkillOutput& output)
{
	// Range는 스킬(전직)레벨을 의미 한다.
	output.Damage = (20 + (input.STR/10) + (min(input.Range,10)*2))*3;
	output.Delay = (4 - min(input.Range,10)/5)*10;
}

void HarpoonBomb::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Damage = -25 + (input.STR/15) + (min(input.Range,10)*4);
	output.Damage = min( 100, 20 + (input.STR/4 + min(input.Range,10)*4) );
	output.Delay = max(240, (70 - (input.DEX/10 + input.Range)) * 10 );
	output.Range = 30 + min(input.Range,10)*2;
//	output.Tick = 100 + (min(input.Range,10)*5);
//	이건 이펙트 데미지네...
	output.Tick = min( 200, 70+(input.STR/2 + input.Range*5) );
//	output.Duration = ( 4 - (min(input.Range,10)/10.0) ) * 10;
	output.Duration = 100;
}

void PassingHeal::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 80 + (min(input.Range,10)*4);
	output.Delay = ( 3 - (min(input.Range,10)/10) ) * 10;
	output.Range = 5 + (min(input.Range,10)/5);
	output.Duration = 0;
}

void RottenApple::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Delay = ( 8 - (min(input.Range,10)/3) ) * 10;
	output.Damage = min( 40 + ( input.INTE / 5 ) + (int)( input.Range * 2.5 ) , 170 );
	output.Range = 7;
	output.Delay = 20;
}

void FeebleVirus::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 2 + ( input.SkillLevel / 10 );
	output.Tick     = 20;
	output.Duration = 50 + ( input.SkillLevel * 4 );
}

void SwordCombo::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage	= 50 + ( input.STR / 10 );
	output.Range	= 2;
}

void BladeCombo::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage	= 70 + ( input.STR / 10 );
	output.Range	= 2;
}

void VampireCombo::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage	= 20 + ( input.STR / 20 + input.DEX / 40 );
	output.Range	= 2;
}

void OustersCombo::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage	= 50 + ( input.STR / 10 + input.DEX / 20 );
	output.Range	= 2;
}

// 2006.06.23
void Brandish::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 200;
	output.Delay = 200;
	output.Range = 16;
}

// 2006.06.25
void Parasite::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 100;
	output.Duration = 200;
	output.Range = 14;
}

// 2006.06.25
void SoundWave::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 150;
	output.Duration = 100;
	output.Delay = 200;
	output.Tick = 10;
	// 2006.08.02 수정
	output.Range = 18;
}

void FireWave::computeOutput(const SkillInput& input, SkillOutput& output)
{
}

void TargetSummonMonsters::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 0;
	output.Delay  = 600; // 1분
}

void BloodyBurst::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 600;
	output.Delay = 1800;
}

void TraceCreature::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 10;
	output.Delay = 10;
}

void ShineSword::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min( 190, (input.STR/2 + input.STR/15 + max(0, min((input.Range-10), 10)*3)) );
	// 딜레이 450 
	output.Delay = max( 450, int(80 - (input.DEX/6 + input.Range)) * 10 );
	output.Duration = 250;
}

void BombCrashWalk::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min( 250, 20+((input.STR/2) + (input.STR/15) + max(0, min((input.Range-10), 10))*3) ); 
	// 딜레이 200
	//output.Delay = max( 450, (80 - (input.DEX/6 + input.Range)) * 10 );
	output.Delay = 50;
	output.Duration = 40;
}

void SatelliteBomb::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min(200, 30 + (input.STR/2 + input.STR/3 + max(0, min((input.Range-10), 10)) * 3) );
	output.Duration	= 10;
	output.Delay	= max(450, (75 - (input.DEX/20 + input.Range)) * 10 );
}

void IllusionInversion::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = max( 450, (80 - ( input.DEX/6 + input.Range )) * 10);
	output.Damage = min( 210, 10 + (input.INTE/3 + input.INTE/10 + max(0, min((input.Range-10), 10))*3) );
}

void HeavenGround::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min( 200, 20 + (input.INTE/3 + input.INTE/10 + max(0, min((input.Range-10), 10))*3) );
	//output.Delay = max( 450, (80 - ( input.DEX/6 + input.Range )) * 10);
        output.Delay = 200;
	output.Duration = min( 300, (input.INTE/15 + input.Range/2) * 10 );
}

void BloodyScarify::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min(400, 100+(input.INTE/2 + input.DEX/10 + max(0, min((input.Range-10), 10))*3));
	Duration_t duration = Duration_t(rand()%41 + 30);
	output.Duration = duration;
	output.Delay = max( 450, (75 - ((input.DEX/20)+(input.Range))) * 10 );
}

void BloodCurse::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = min( 400, (10 + (input.DEX/30 + input.Range)) * 10 );
	output.Delay = 450;
}

void BloodsSymposionAttack::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min(215, input.STR/3 + input.STR/50 + input.DEX/10 + max(0, min((input.Range-10), 10))*2);
	output.Duration = (10 + (input.DEX/30 + input.Range)) * 10;
	output.Delay = max( 450, (80-((input.DEX/6)+input.Range))*10 );
}

void DummyDrake::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min( 320, (input.INTE/2 + input.INTE/30 + max(0, min((input.Range-10)*5, 10))) );
	output.Delay = max(400, int((65 - (input.DEX/20)+(input.Range*1.5) ) * 10) );
	Duration_t duration = Duration_t(rand()%41 + 30);
	output.Duration = duration;
}

void HydroConvergence::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min(185, 20 + ((input.INTE/4) + (input.INTE/30) + max(0, min((input.Range-10), 10))));
	output.Delay = max(400, int((65 - (input.DEX/20)+(input.Range*1.5) ) * 10) );
	output.Duration = 160;
}

void SummonClay::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Delay = min(400, int((65 - (input.DEX/20)+(input.Range*1.5) ) * 10) );
         output.Delay = 50;
	output.Duration = min(300, (input.INTE/15 + input.Range/2)*10);
}

void HeterChakram::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = min(350, (30 + input.STR/2 + input.DEX/15 + max(0, min((input.Range-10),10))*5) );
	output.Duration = 10;
	output.Delay = max(400, int(65-(input.DEX/20 + input.Range*1.5))*10);
}

void Tanning::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay    = 400;
}

void RequestResurrect::computeOutput(const SkillInput& input, SkillOutput& output)
{
}

// 20070820 
void MagicShield1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 10;
	output.Duration = 3000;
	output.Delay = 3000;
}

// 20070828 
void MagicShield2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15;
	output.Duration = 6000;
	output.Delay = 40;
}
void EaglesEye1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 5;
	output.Duration = 12000;
	output.Delay = 100;
}

void EaglesEye2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 10;
	output.Duration = 12000;
	output.Delay = 100;
}

void FreezeRing1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 10 + input.INTE/25;
	output.Delay = 150;
//	output.Duration = 150;
}

void FreezeRing2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE/18;
	output.Delay = 150;
//	output.Duration = 150;
}

void BatStorm1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 30 + (input.DEX+input.STR) / 10 ;
	output.Duration = 10;
	output.Delay = 20;
}

void BatStorm2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 30 + (input.DEX+input.STR) / 6 ;
	output.Duration = 10;
	output.Delay = 20;
}

void CurseOfBlood1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 10 + input.INTE / 25 ;
//	output.Duration = (input.INTE / input.Range) * 10;
	output.Delay = 120;
}

void CurseOfBlood2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 10 + input.INTE / 18 ;
//	output.Duration = (input.INTE / input.Range) * 10;
	output.Delay = 120;
}

void VenomCircle::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (15 + input.INTE / 2.8) * (11 + (int)((input.Range - 1) / 10)) / 10;
	output.Duration = max(20, min(40, (int)(input.INTE / 15))) ;
	output.Delay = max((8 - input.INTE / 150) * 10, 40);
}

void BloodyShout1::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Damage = (input.STR + input.DEX) / 30;
	output.Duration = 20 * 600;
	output.Delay = 100;
}

void BloodyShout2::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	output.Damage = ((input.STR + input.DEX) / 30) * 1.5;
	output.Duration = 20 * 600;
	output.Delay = 100;
}

void BloodyWings::computeOutput(const SkillInput& input, SkillOutput& output)
{
//	if(input.Range == 0)
//	output.Damage = (input.INTE)*2;
//	else
//	output.Damage = (input.INTE/input.Range)*2;

	output.Delay = 60;
}

void MistOfSoul1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 10 + input.INTE/25;
	output.Delay = 120;
}

void MistOfSoul2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 10 + input.INTE/18;
	output.Delay = 120;
}

void OniblaRaiseBone::computeOutput(const SkillInput& input, SkillOutput& output)
{
	// 몬스터의 경우 input.SkillLevel 는 몬스터 레벨
	output.Damage = 1325;
	output.Tick = 20;
	output.Duration = 50 + ( input.SkillLevel * 4 );
	output.Delay = 50;
}

void OniblaDropSkull::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 75;
	output.Duration = 45;
	output.Tick = 10;
	output.Delay = output.Duration;
}

void AgonyChildDropHead::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 1060;
	output.Duration = 10;
	output.Delay = 10;
}

void AgonyChildShadowFour::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.STR/20) * (3 + (input.DEX/40));
	output.Delay = 15;
}

void VillainLadyGhostShadow::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 500;
	output.Delay = 20;
}

void VillainLadySelfDestruction::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 554;
	output.Delay = 0;
}

void OniSpanielThrowingAxe::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void SiusCrash::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void LycanHowl::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void SmileofDeath::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void BlessofLycan::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void FuryShock::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void RailGunAttack::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void BLAZEWALK2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 115 + input.STR/4;
	output.Delay = 1;
}

void GLADIATOR::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	// penalty값은 Berserker.cpp에 있다.
	//output.Damage    = (input.STR/20) * (1 + input.SkillLevel/20); //  데미지 보너스
	//output.ToHit    = (input.DEX/10) * (1 + input.SkillLevel/10); //  투힛 보너스

	// 2002.12.06 장홍창
//	output.Damage    = (input.STR/20) * (1 + input.SkillLevel/25); //  데미지 보너스
//	output.ToHit    = (input.DEX/10) * (1 + input.SkillLevel/12); //  투힛 보너스
	output.ToHit    = 10; //  투힛 보너스

//	output.Duration = (30 + input.SkillLevel/5) * 10;
	output.Duration = 18000;
	//output.Delay    = max(5 - input.SkillLevel/33,2) * 10;
	output.Delay	= 100;		// Delay 와 Duration 이 같다. by bezz 2003.3.5
}

void CRUSHINGSTORM::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/1.0) + ( 1.67 + ((float)input.DEX/1.2)));
	output.Delay = 84;
	output.Duration = 240 ;
}

void SATELLITEBOMB2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Damage = 15 + ((input.STR + input.DEX)*1.2);//fk
        output.Damage = 15 + (input.DEX * 1.0);
	output.Duration	= 10;
	output.Delay = 100;
}

void INTIMATEGRAIL2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if(input.TargetType == SkillInput::TARGET_SELF)
		output.Duration = 1200;
	else
		output.Duration = 10 + (input.INTE/100.0) + (100/15.0);

	output.Duration *= 10;

//	output.Delay = 10;
	output.Delay = 120;
	output.Range = 6;
}

void HOLYARMOR2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 12000;
	output.Damage = (10 + (input.INTE/20) + (100/10))*1.2;
	output.Delay = 30;
}


void STRIKING2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER)
	{
		output.Damage = (input.INTE/30 + 100/20)*1.2;
	}
	else
	{
		output.Damage = (input.INTE/30 + 100/10)*1.2;
	}

	output.Duration = 12000;
	output.Delay    = (6 - 100/33) * 10; // 6->3초

	// 파티 보너스를 계산한다.
	output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void BLESS2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
        int INTE =min(1265,input.INTE);
	if ( input.TargetType == SkillInput::TARGET_SELF )
	{
		//output.Damage   = (4 + INTE/40 + 100/20)*1.1;
	output.Damage   = 15 + (INTE/40)*2.2;//max 80
	}
	else
	{
	output.Damage   = 15 + (INTE/40)*1.9;//max 70
	}

	output.Duration = 12000; // 30~50초
	output.Delay    = (7 - 100/20) * 10; // 6->3초

	//output.Damage = min(55, output.Damage);
	// 파티 보너스를 계산한다.
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void GLORYGROUND::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage	= (10 + ( input.INTE/40 ) + ( 100/10 ))*1.5;
	output.Duration = 400;
	output.Delay	= output.Duration;
	output.Tick		= 10;
	output.Range	= 5;
}

void PASSINGHEALK2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (80 + (min(input.Range,10)*4))*1.5;
	output.Delay = ( 3 - (min(input.Range,10)/10) ) * 10;
	output.Range = 5 + (min(input.Range,10)/5);
	output.Duration = 0;
}

void GREATHEAL2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER) 
	{
		output.Damage = 380; //fk=300
	}
	else
	{
		output.Damage = 380;
	}

	output.Delay = (5 - 100/33) * 10;

	// 파티 보너스를 계산한다.
	//output.Duration = getPercentValue(output.Duration, PartyDurationBoost[input.PartySize]);
}

void CHAINOFDEMON::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = min(40, 25 + (input.INTE-20)/20);
	//output.Duration = (3 + input.INTE/15) * 10;
	output.Duration = 50;		// 최대값 추가 by bezz 2005.06.16
	output.Delay    = 300;
	output.Tick    = 50;
}

void BLOODYSKULL::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = (int)(50 + (input.INTE)*1.1)*1.2;
	output.Duration = 5; 	// 몇 초후 폭발
	output.Delay = 20;
}

void RAGEOFBLOOD::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 300;//7
	output.Duration = 200;
	output.Delay = 1800;
}

void ALLYSTERWIND::computeOutput(const SkillInput& input, SkillOutput& output)
{

	output.Damage = 200;
	output.Duration = 600;
	output.Delay = 1200;
}

void FLAMESIGHT::computeOutput(const SkillInput& input, SkillOutput& output)
{
	
	output.Duration = 600;
	output.Delay = 1200;
}

void SPIRALMEGALITH::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + input.INTE/2;
	output.Delay = 1;
}

void RADCHIA::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = (5 + (input.INTE/25)+(30/3))*10;
	output.Delay = 250;
	output.Tick = 50;
	output.Damage = min(500, (int)(20 + (input.INTE/2.0) + (3 + (30/3.0))) );
}

void HEALPASS2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void BREATHOFDRYAD::computeOutput(const SkillInput& input, SkillOutput& output)
{
        output.Damage = 200;
	output.Duration = 12000;
	output.Delay = 10;
}

void CUREEFFECT2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void LIVINGDEADBECHOESPER::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void MIHAIBATSTREAM::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void TSARBLUEROSEDEVIL::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void FOLLETPOISONFRAME::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void FUEFOLLETREDROSEDEVIL::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void DACULETALONOFDDRACULE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void DRACULEBLOOKDYSCARIFY2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void DRACULEBLOODYCARPE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void DRACULEDRACULECASKET::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void DRAGONHURRICANE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 30 + (input.STR*0.8) + (min(input.Range,10)*2);
	output.Range = max(200,80 + (int)(input.STR*0.8) + (min(input.Range,10)*2));
	output.Tick = max(200,50 + (int)(input.STR*0.8) + (min(input.Range,10)*2));
	output.Duration = (20 + min(input.Range,10))*10;
	output.Delay = 300;
}

void LARSTROKE::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 0;
	output.Damage = 5 + (input.STR + input.DEX)*0.45;
}

void GAEBULGA::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.ToHit  = -30 + input.DEX/20 + 100/4;
	output.Damage = 30 + (input.STR + input.DEX)*0.15;
	output.Delay  = 0;
}

void VICIOUSGUIDANCE::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if ( input.TargetType == SkillInput::TARGET_PC )
	{
		output.Damage = 15 + ( input.INTE/20 )*2;
		output.Tick = 15;
		output.ToHit = ( input.INTE/20 ) + ( 100/10 );
		//output.Range = min( output.Range, 70 );
	}
	else
	{
		output.Damage = 15 + ( input.INTE/20 )*2;
		output.Tick = 14;
		output.ToHit = ( input.INTE/20 ) + ( 100/10 );
		//output.Range = min( output.Range, 90 );
	}

	output.Range = 7;
	output.Delay = 150;
	output.Duration = 150;
}

void SPECTORINVERSE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = 40;
	output.Damage = max( 210, 10 + ((int)(input.INTE/1*1.2) + max(0, min((input.Range-10), 10))*3));
}


void BATBREAKER::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.DEX/4 ;
	output.Duration = 10;
	output.Delay = 20;
}

void SHADYDOUPLE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE*1.6;
	output.Delay  = 20; // by rallser. 2002.12.10
}

void CHAKRAMHAIL::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20;
        output.Damage = 30 + ((input.DEX+input.STR)/20.0) * (5.0/3.0 + (30/10.0)*1.5)*2;
	output.Tick = 3;
	output.Duration = 10;
}

void FLAMESPIKE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + input.INTE*0.7;
        output.Delay=0;
}

void BLIZZARD::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE/2.7*1.3;
	output.Tick = 3;
	output.Delay = 20;
	output.Duration = 10;
}

void DEMOLISHER::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + input.INTE*1;
	output.Delay = 20;
}

void MAGMATICERUPTION::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 1 + (input.STR*1.1)*0.8;
	output.Range = 3;
	output.Delay = 80;
	output.Duration = 80;
}

void ADV2LIGHTINGSHOCK::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (1 + ((input.STR+input.DEX)/1.3));
	output.Range = 5;
	output.Delay = 40;
	output.Duration = 14;
}
void ADV2HALCYONMK1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 8 + (100/7);
	output.ToHit = 40 + (100/10);
	output.Delay = 10;
	output.Duration = 900 + 100*20;
       //  output.Duration = 150;
}
void MK1Fire::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	//output.Delay = 15;
	//output.Range = 7;
	output.Damage = input.DEX*0.9;
	output.Duration = 10;
}
void ADV2SOLARFLARE::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20;
	output.Range = 7;
	output.Duration = 10;
	output.Damage = input.INTE*1.3;
}
void ADV2VIACRUCIS::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20;
	output.Duration = 10;
	output.Damage = input.INTE*1.4*0.7;
}
void ADV2CHAOSSPLITTER::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 15 + input.DEX*0.65;
	output.Delay  = 0; // 0.6초
}
void ADV2DEADLYGROUND::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = input.INTE * 10 / 25 + 50;
	output.Delay = output.Duration;
}

void DEADLYATT::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 20 + input.INTE*0.6;
	output.Duration = 10; 	// 몇 초후 폭발
	output.Delay = 10;
}
void ADV2SUDDENBLAST::computeOutput(const SkillInput& input, SkillOutput& output) 
{
        output.Damage = 20 + (int)( (input.STR/10.0 + input.DEX/10.0 ) * ( 5.0/3.0 + (input.SkillLevel/22.5)) );
        output.Damage = output.Damage*2;
	output.Duration = (3 + (input.SkillLevel/30.0)) * 10;
//	output.Delay = max(50, (10 - (input.DEX/60) - (input.SkillLevel/10) ) * 10 );
	output.Delay = 30;
	output.Range = 7;
}
void ADV2METEORBLAZE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE*1.3;
	output.Tick = 5;
	output.Delay = 40;//CD -1
	output.Duration = 20;
}
void ADV2TORRENTIALLIFE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 50 + (input.INTE/2);
	output.Range = 20 + (100/10);
//	output.Delay = 80 - input.SkillLevel/3;
	//output.Delay = 100 - input.SkillLevel/5;
	//output.Delay = ( 40 - input.SkillLevel/10 ) * 10;	// 40초 - 30초//fk
	output.Delay = 60;	// xuese
	output.Duration = 300;
}
void ADV2CYCLINGWARROCK::computeOutput(const SkillInput& input, SkillOutput& output)
{
		output.Damage = 50 + (input.INTE*0.5);
	//}
	output.Duration = 6000;
	output.Delay = 20;
}
void ADV2TRUECLOLOROFVAMPLOAD::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 600;
	output.Delay = 3000;
}
void ADV2THEROAROFAWAKENWARRIOR::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 600;
	output.Delay = 3000;
}
void ADV2THERAGEOFGUARDIANSPRIT::computeOutput(const SkillInput& input, SkillOutput& output)
{

	output.Duration = 600;
	output.Delay = 3000;
}
//add by rall hf 221
void ENRAGECONQUROR::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 200 + input.STR * 0.8;
	//output.Duration = 20 ;
	output.Delay = 60;
}
void ATTRACTCIRCLE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 300 + (input.STR + input.DEX) * 0.6;
	output.Delay = 30;
}
void IGNISINFERNUM::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (1 + ((input.STR+input.DEX)/1.3));
	output.Range = 6;
	output.Delay = 120;
	output.Duration = 100;
}
void DEMONICRESURRECTIO::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 100;
	output.Delay = 150;
	output.Tick = 10;
	output.Damage = min(250, (int)(20 + (input.INTE/2.0) + (3 + (30/3.0))) );
}
void WOLFROAR::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 36000;
        output.Damage =70;//attr +40
	output.Delay = 300;
}
void EVGENEIABELLUM::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 500;
	output.Delay = 900;
}
void OUTRAGEBOOSTER::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = 15;
	output.Duration = 20;
	output.Delay    = 120;
}
void ICEPRESSURE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE/2.7;
	output.Tick = 3;
	output.Delay = 20;
	output.Duration = 10;
}
void CRYSTALTHORN::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE/2.9;
	output.Tick = 3;
	output.Delay = 20;
	output.Duration = 10;
}
void FAVOROFLUNA::computeOutput(const SkillInput& input, SkillOutput& output)
{
	if(input.TargetType == SkillInput::TARGET_SELF)
		output.Duration = 1200;
	else
		output.Duration = 10 + (input.INTE/100.0) + (100/15.0);

	output.Duration *= 10;
	output.Delay = 50;
	output.Range = 4;
}
void TEMENOS::computeOutput(const SkillInput& input, SkillOutput& output) 
{

	output.Damage = 300 + input.INTE/5; // 3-13
	output.Delay = 120; // 15-10초
	output.Duration = 40; // 3-5초
}

void DEPENDSOUL::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 12000;
	output.Delay = 600;
}

void PULSEMAGNETICFIELD::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = 200;
	output.Delay    = 300;
	output.Range    = 6;
}
void TRANSFORMDARKBATSTORM::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 1000;
	output.Delay = 20;
	output.Range = 8;
}
void ULRAYIRYMELLEATTACKA::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay=0;
	output.Damage=1000;
}
void ULRAYIRYMELLEATTACKB::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay=0;
	output.Damage=1500;
}
void CLIENTSHADYDOUPLE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void CLIENTBATBREAKER::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void GarumMagic::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void ClavieMagic::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void PalusMiddleRangeMagic::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void PalusLongRangeMagic::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void BifronzeMagic::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void WideWildWolf::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 15 + (((float)input.STR/30.0) * ( 1.67 + ((float)input.DEX/20.0)));
	output.Delay = 15;
}

void BlackAstralMagicCrystal::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.INTE/12)*(1+(input.SkillLevel/37));
	output.Duration = 10;
	output.Delay = 15;
}

void WhiteAstralSpellCrystal::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.INTE/12)*(1+(input.SkillLevel/36));
	output.Duration = 10;
	output.Delay = 15;
}
//221skill
void ADV2ADVENTBLADE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 400;
	output.Delay = 600;
	output.Damage = 50;
}
void ADV2CHAINLIGHTNINGSWORD::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.STR+input.DEX)/3.5*1.4;
	output.Range = 10;
	output.Delay = 40;
	output.Duration = 30;
}
void ADV2HALCYONMK1NAPALNBULLET::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.DEX*1.1;
	output.Delay = 20;
	output.Duration = 10;
}
void ADV2CONTRADICTO::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay=150;
}
void ADV2DIVINETRACES::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER) 
	{
		output.Damage = 1500;//hp+500
	}
	else
	{
		output.Damage = 1500;
	}

	output.Delay = 150;

}
void ADV2WILDWOLFSTRIKE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 30 + (input.DEX+input.STR) / 2.3 *1.15;
	output.Duration = 10;
	output.Delay = 60;//cd -1
}
void ADV2DEADLYSTORM::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (int)(input.INTE / 2)*1.2 ;
	output.Duration = 80;
	output.Delay = 100;//cd -5
}
void ADV2SHININGCHAKRAMSPIN::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (1 + ((input.STR+input.DEX)/2.1)*0.8);
	output.Range = 6;
	output.Delay = 120;
	output.Duration = 80;
}
void ADV2INFINITYBLAZE::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE * 2.3 ;
	output.Delay = 20;//fk-1
	output.Range = 7;
}
void ADV2GRACEOFELQUINES::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	int INTE = min(1405,input.INTE);
	//cout<<"ADV2GRACEOFELQUINES INTE:"<<INTE<<endl;
	output.Damage   = 9 + INTE/40;
	int rate = output.Damage;
	//cout<<" ADV2GRACEOFELQUINES otehr rate::"<<rate<<endl;
	output.Duration = 12000; 
	output.Delay    = 20;
}
void ADV2SUMMONDEFENSIVESPINEL::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Delay = 300;
	output.Duration = 1800;
}
void CREATUREGOLEMQUAKEATTACK::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = Random(4000, 5900);
	output.Duration = 10; 
	output.Delay = 10;
}
void VOMIT::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = input.INTE/1;
		output.Delay  = 10;
}
void LAYWASTE::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.INTE/1; 
	output.Delay  = 10;
}
void HALFMOONKICK::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = Random(4500, 5500);
		output.Delay  = 0;
}
void BLOODPUNISH::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = Random(5500, 6000);
		output.Delay  = 0;
}
void BLOODPRAY::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = Random(6500, 7000);
		output.Delay  = 0;
}
void DEVILWAVE::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = Random(300, 600);
		output.Delay  = 0;
}
void PARASITEATTACK::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = Random(300, 600);
		output.Delay  = 0;
}
void DEVILSCREAM::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = Random(300, 600);
		output.Delay  = 0;
}
void BONESTRIKE::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = Random(300, 600);
		output.Delay  = 0;
}
void LIGHTNINGSLASH::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = Random(4300, 6600);
		output.Delay  = 0;
}
void GENESISATTACK::computeOutput(const SkillInput& input, SkillOutput& output) 
{
		output.Damage = Random(6300, 8600);
		output.Delay  = 0;
}
//231skill
void BLOODYSKULL2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = (int)(50 + (input.INTE)*1.1)*1.2;
	output.Damage = output.Damage*1.25;
	output.Duration = 5;
	output.Delay = 20;
}
void ADV2CHAOSSPLITTER2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = 15 + input.DEX*0.65;
	output.Damage = output.Damage*1.15;
	output.Delay  = 0;
}
void CRYSTALTHORN2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE/2.9;
	output.Damage = output.Damage*1.15;
	output.Tick = 3;
	output.Delay = 20;
	output.Duration = 10;
}
void BLIZZARD2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE/2.7*1.3;
	output.Damage = output.Damage*1.15;
	output.Tick = 3;
	output.Delay = 20;
	output.Duration = 10;
}
void ADV2METEORBLAZE2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE*1.3;
	output.Damage = output.Damage*1.15;
	output.Tick = 5;
	output.Delay = 40;//CD -1
	output.Duration = 20;
}
void CHAKRAMHAIL2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20;
        output.Damage = 30 + ((input.DEX+input.STR)/20.0) * (5.0/3.0 + (30/10.0)*1.5)*2;
	output.Damage = output.Damage*1.15;
	output.Tick = 3;
	output.Duration = 10;
}
void MK1Fire2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.DEX*0.9;
	output.Damage = output.Damage*1.15;
	output.Duration = 10;
}
void ADV2SOLARFLARE2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20;
	output.Range = 7;
	output.Duration = 10;
	output.Damage = input.INTE*1.3;
	output.Damage = output.Damage*1.15;
}
void ADV2VIACRUCIS2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20;
	output.Duration = 10;
	output.Damage = input.INTE*1.4*0.7;
	output.Damage = output.Damage*1.15;
}
void ADV2LIGHTINGSHOCK2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (1 + ((input.STR+input.DEX)/1.3));
	output.Damage = output.Damage*1.15;
	output.Range = 5;
	output.Delay = 40;
	output.Duration = 14;
}
void DRAGONHURRICANE2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 30 + (input.STR*0.8) + (min(input.Range,10)*2);
	output.Damage = output.Damage*1.15;
	output.Range = max(200,80 + (int)(input.STR*0.8) + (min(input.Range,10)*2));
	output.Tick = max(200,50 + (int)(input.STR*0.8) + (min(input.Range,10)*2));
	output.Duration = (20 + min(input.Range,10))*10;
	output.Delay = 300;
}
void BLADEERUPTION1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = 200 + input.STR * 1.1; 
	output.Delay = 40;
}
void ATTRACTSWORDCRASH1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.DEX * 1.85;
	output.Delay = 120;
}
void STRATEGICBOMBER1::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = input.DEX*1.1;
	output.Delay = 40;
	output.Duration = 40;
}
void AURAEXPLOSION1::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20;
	output.Range = 6;
	output.Duration = 10;
	output.Damage = input.INTE*1.3*1.23;
}
void HOLYACIDALIUM1::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 30;
	output.Range = 6;
	output.Duration = 10;
	output.Damage = input.INTE*1.4*0.7*1.23;
}
void BATSPEAR1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Damage = (1 + ((input.STR+input.DEX)/1.3));
	output.Damage = 30 + (input.DEX+input.STR) / 2.3 *1.15*1.05*3;
	output.Range = 6;
	output.Delay = 40;
	output.Duration = 10;
}
void EXTENSIVEDELUSION1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE*1.9;
	output.Delay = 180;
	output.Range = 7;
	output.Duration = 20;
}
void ACCELERATECHARGE1::computeOutput(const SkillInput& input, SkillOutput& output) 
{
        output.Damage = 20 + input.DEX*1.3;
	output.Duration = (3 + (input.SkillLevel/30.0)) * 10;
	output.Delay = 40;
}
void RAGEBURST1::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = input.INTE*2.0;
	output.Delay    = 60;
}
void FROZENGROUND1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE/2.7*2;
	output.Tick = 10;
	output.Delay = 100;
	output.Duration = 100;
}
void METEORICFALL1::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE/2.9;
	output.Damage = output.Damage*1.45;
	output.Tick = 3;
	output.Duration = 10;
	output.Delay = 20;
}

void BLADEERUPTION2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (200 + input.STR * 1.1)*1.1; 
	output.Delay = 40;
}
void ATTRACTSWORDCRASH2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.DEX * 1.85)*1.1;
	output.Delay = 120;
}
void STRATEGICBOMBER2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage = (input.DEX*1.1)*1.1;
	output.Delay = 40;
	output.Duration = 40;
}
void AURAEXPLOSION2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 20;
	output.Range = 6;
	output.Duration = 10;
	output.Damage = (input.INTE*1.3*1.23)*1.2;
}
void HOLYACIDALIUM2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Delay = 30;
	output.Range = 6;
	output.Duration = 10;
	output.Damage = (input.INTE*1.4*0.7*1.23)*1.2;
}
void BATSPEAR2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	//output.Damage = (1 + ((input.STR+input.DEX)/1.3));
	output.Damage = (30 + (input.DEX+input.STR) / 2.3 *1.15*1.05*3)*1.1;
	output.Range = 6;
	output.Delay = 40;
	output.Duration = 10;
}
void EXTENSIVEDELUSION2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.INTE*1.9)*1.2;
	output.Delay = 180;
	output.Range = 7;
	output.Duration = 20;
}
void ACCELERATECHARGE2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
        output.Damage = (20 + input.DEX*1.3)*1.1;
	output.Duration = (3 + (input.SkillLevel/30.0)) * 10;
	output.Delay = 40;
}
void RAGEBURST2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = (input.INTE*2.0)*1.2;
	output.Delay    = 60;
}
void FROZENGROUND2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.INTE/2.7*2)*1.2;
	output.Tick = 10;
	output.Delay = 100;
	output.Duration = 100;
}
void METEORICFALL2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (input.INTE/2.9)*1.2;
	output.Damage = output.Damage*1.45;
	output.Tick = 3;
	output.Duration = 10;
	output.Delay = 20;
}
void WARPPORTAL::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Range	= 6;
	output.Delay = 20;
}

void SetAfire2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
        int abc;
        abc = ( 13 - ( input.STR/60 + input.DEX/100 ) ) * 10;
        if (abc <0)
        {
          output.Delay = 0;
         }
        else
        {
	output.Delay = ( 13 - ( input.STR/60 + input.DEX/100 ) ) * 10;
        }
	output.Damage = 20 + ((input.STR/6) + (input.DEX/10))*4;
}
void SHADYDOUPLE2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.INTE*1.6*1.2;
	output.Delay  = 20; // by rallser. 2002.12.10
}
void PotentialExplosion2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Damage   = (1+100/15)*1.15;
        output.Duration = 6000;
	output.Delay	= 40;
}
void SharpShield2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 6000;
	output.Damage   = ( input.STR / 20 ) + ( 100 / 20 );
	output.Damage = output.Damage*1.15;
	output.Delay	= 40;
}
void Sniping2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	output.Duration = 500;
	output.Delay    = 50;
}
void Thorns2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Duration = 3000;
	output.Damage	= ( 30 + (input.INTE / 7) ) * (11 + (int)((input.Range - 1) / 10)) / 10; 
	output.Damage = output.Damage*1.15;
	output.Delay	= 100;
	output.Tick		= 10;
}
void CureSeriousWounds2::computeOutput(const SkillInput& input, SkillOutput& output) 
{
	if (input.TargetType == SkillInput::TARGET_OTHER)
	{
		output.Damage = 30 + input.INTE/8;
	}
	else
	{
		output.Damage = 30 + input.INTE/4;
	}

	output.Delay = 40;
	//output.Damage   = getPercentValue(output.Damage, PartyEffectBoost[input.PartySize]);
	output.Damage = (output.Damage*2)*1.4;
	output.Delay = 40;
}
void Halo2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = input.DEX/3*1.1;
	output.Range = 60 + (min(input.Range,10)*3);
	output.Duration = ( 5 + (input.DEX/100.0) * 3 ) * 10;
	output.Delay = output.Duration;
}
void RingOfFlare2::computeOutput(const SkillInput& input, SkillOutput& output)
{

	output.Damage = (int)( 2 + ( input.INTE / 15.0 ) + ( 1.0 + (30 / 2.0 ) ) );

	//output.Duration = (int)(( 35.0 + ( input.INTE / 30.0 ) + (30/3.0) ) * 10);
	output.Duration = 6000;
	output.Damage = output.Damage*1.2;
	output.Delay = 40;
}
void WaterBarrier2::computeOutput(const SkillInput& input, SkillOutput& output)
{
	output.Damage = (int)(output.Damage * 1.1);
	output.Duration = 6000;
	output.Delay = 100;
	output.Damage = output.Damage*2;
}
void ReactiveArmor2::computeOutput(const SkillInput& input, SkillOutput& output)
{

	output.Damage = (int)min(50,(int)( 5.0 + ( input.INTE / 30.0 ) * 3 ));

	output.Duration = 6000;

	output.Delay = max( 2, 5 - (input.INTE-20) / 10 ) * 10;
}

