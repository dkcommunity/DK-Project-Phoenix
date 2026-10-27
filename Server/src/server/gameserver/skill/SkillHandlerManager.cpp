//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillHandlerManager.cpp 
// Written By  : Elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SkillHandlerManager.h"
#include "Assert.h"

// 일반 공격
#include "AttackMelee.h"
#include "AttackArms.h"

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
#include "Revealer.h"
#include "CreateBomb.h"
#include "CreateMine.h"
#include "InstallMine.h"
#include "DisarmMine.h"
#include "ObservingEye.h"
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
#include "Regeneration.h"
#include "Resurrect.h"
#include "EnergyDrop.h"
#include "Peace.h"
#include "Activation.h"
#include "HolyBlast.h"
//#include "MassCure.h"
//#include "MassHeal.h"
#include "HolyArrow.h"
#include "TurnUndead.h"
#include "DenialMagic.h"
#include "Illendue.h"


// 뱀프 계열
#include "BloodDrain.h"

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
//#include "Death.h"

#include "BloodyNail.h"
#include "BloodyKnife.h"
#include "BloodyBall.h"
#include "BloodyWave.h"
#include "BloodyMasterWave.h"
#include "BloodyWarp.h"
#include "BloodySpear.h"
#include "BloodyWall.h"
#include "BloodySnake.h"
#include "BloodyBreaker.h"
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

#include "Unburrow.h"
#include "Untransform.h"

#include "EatCorpse.h"
#include "Howl.h"
#include "ThrowHolyWater.h"

#include "Restore.h"

#include "Uninvisibility.h"
#include "Death.h"
#include "Transfusion.h"
#include "Mephisto.h"


// 기타 계열
#include "CriticalGround.h"
#include "SummonMonsters.h"
#include "GroundAttack.h"
#include "MeteorStrike.h"

#include "Hallucination.h"
#include "DuplicateSelf.h"

// 2002.10.23
#include "SoulChain.h"

// 2002.12.26
#include "SharpShield.h"
#include "WideLightning.h"
#include "GunShotGuidance.h"

#include "HandsOfWisdom.h"
#include "Armageddon.h"

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
#include "AbsorbSoul.h"
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
#include "BiteOfDeath.h"
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

// 아우스터즈 140레벨 스킬
#include "SharpHail.h"
#include "IceHorizon.h"
#include "FuryOfGnome.h"
#include "SummonMiga.h"
#include "SummonMigaAttack.h"
#include "Cannonade.h"
#include "SelfDestruction.h"
#include "ARAttack.h"
#include "SMGAttack.h"
#include "GrenadeAttack.h"

#include "UnterfeldwebelFire.h"
#include "FeldwebelFire.h"
#include "FeebleVirus.h"

// 승직 스킬
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
#include "MonsterRangeSkill.h"
#include "SimpleTileMeleeSkill.h"
#include "SimpleTileMissileSkill.h"

#include "SwordCombo.h"
#include "BladeCombo.h"
#include "VampireCombo.h"
#include "OustersCombo.h"

#include "HolySword.h"
#include "HolyBlade.h"
#include "BurstGun.h"
#include "HolyMace.h"
#include "HolyCross.h"
#include "DarkForce.h"
#include "NaturalForce.h"
#include "FireForce.h"
#include "WaterForce.h"
#include "EarthForce.h"
#include "LoseSight.h"
#include "WideBloodDrain.h"
#include "NaturalPeace.h"
#include "PartyAura.h"

#include "Brandish.h"
#include "SoundWave.h"
#include "Parasite.h"
#include "FireWave.h"

#include "TargetSummonMonsters.h"

// 2006.08.14
#include "BloodyBurst.h"
#include "TraceCreature.h"

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

#include "Tanning.h"

#include "RequestResurrect.h"
// 20070820
#include "MagicShield1.h"
#include "MagicShield2.h"
#include "EaglesEye1.h"
#include "EaglesEye2.h"
#include "FreezeRing1.h"
#include "FreezeRing2.h"
#include "BatStorm1.h"
#include "BatStorm2.h"
#include "CurseOfBlood1.h"
#include "CurseOfBlood2.h"
#include "BloodDrain2.h"
#include "BloodyShout1.h"
#include "BloodyShout2.h"
#include "BloodyWings.h"
#include "MistOfSoul1.h"
#include "MistOfSoul2.h"

// 승직 레어스킬 
#include "HeavenlySpark.h" // 20080401
#include "BladeStorm.h" // 20080403
#include "ExplosionRocketLauncher.h" // 20080407
#include "Genesis.h" // 20080408
#include "Thorns.h" // 20080408
#include "LethalClaw.h" // 20080410
#include "VenomCircle.h" // 20080414
#include "Brionac.h" // 20080415
#include "AsteroidBomber.h" // 20080415
#include "FrozenGlacier.h" // 20080415
#include "Incinerate.h" // 20080415

#include "SkillScript.h"

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
#include "BlessofLycan.h"
#include "SmileofDeath.h"
#include "FuryShock.h"
#include "RailGunAttack.h"
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
#include "ADV2METEORBLAZE.h"
#include "ADV2CYCLINGWARROCK.h"
#include "ADV2TRUECLOLOROFVAMPLOAD.h"
#include "ADV2THEROAROFAWAKENWARRIOR.h"
#include "ADV2THERAGEOFGUARDIANSPRIT.h"
#include "ENRAGECONQUROR.h"
#include "ATTRACTCIRCLE.h"
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
#include "CLIENTSHADYDOUPLE.h"
#include "CLIENTBATBREAKER.h"
//2014-7-14 221skill
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
//2014-7-24 boss skill
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

// global variable declaration
SkillHandlerManager * g_pSkillHandlerManager = NULL;

namespace
{
	class MappedMonsterTileSkill : public SkillHandler
	{
	public:
		enum Style
		{
			STYLE_MISSILE,
			STYLE_MELEE
		};

		MappedMonsterTileSkill(const char* handlerName, SkillType_t skillType, Style style, int radius, bool magic)
			throw()
		: m_HandlerName(handlerName),
		  m_SkillType(skillType),
		  m_Style(style),
		  m_Radius(radius),
		  m_Magic(magic)
		{
		}

		string getSkillHandlerName() const throw() { return m_HandlerName; }
		SkillType_t getSkillType() const throw() { return m_SkillType; }

		void computeOutput(const SkillInput& input, SkillOutput& output)
		{
			SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(m_SkillType);
			output.Damage = (pSkillInfo->getMinDamage() + pSkillInfo->getMaxDamage()) / 2;
			if (output.Damage <= 0) output.Damage = input.INTE;
			if (output.Damage <= 0) output.Damage = 1;

			output.Delay = pSkillInfo->getMinDelay();
			if (output.Delay <= 0) output.Delay = 20;
		}

		void execute(Monster* pMonster, ZoneCoord_t x, ZoneCoord_t y) throw(Error)
		{
			__BEGIN_TRY

			if (pMonster == NULL)
				throw Error("MappedMonsterTileSkill::execute() null monster");

			SkillInput input(pMonster);
			SkillOutput output;
			computeOutput(input, output);

			SIMPLE_SKILL_INPUT param;
			param.SkillType     = m_SkillType;
			param.SkillDamage   = output.Damage;
			param.Delay         = output.Delay;
			param.ItemClass     = Item::ITEM_CLASS_MAX;
			param.STRMultiplier = 0;
			param.DEXMultiplier = 0;
			param.INTMultiplier = 0;
			param.bMagicHitRoll = m_Magic;
			param.bMagicDamage  = m_Magic;
			param.bAdd          = false;

			for (int dx = -m_Radius; dx <= m_Radius; ++dx)
			{
				for (int dy = -m_Radius; dy <= m_Radius; ++dy)
				{
					param.addMask(dx, dy, 100);
				}
			}

			SIMPLE_SKILL_OUTPUT result;
			if (m_Style == STYLE_MELEE)
				g_SimpleTileMeleeSkill.execute(pMonster, x, y, param, result);
			else
				g_SimpleTileMissileSkill.execute(pMonster, x, y, param, result);

			__END_CATCH
		}

	private:
		const char* m_HandlerName;
		SkillType_t m_SkillType;
		Style m_Style;
		int m_Radius;
		bool m_Magic;
	};

	MappedMonsterTileSkill g_IronMaiden("IronMaiden", SKILL_IRON_MAIDEN, MappedMonsterTileSkill::STYLE_MISSILE, 3, true);
	MappedMonsterTileSkill g_ChaosSplitter2M("ChaosSplitter2M", SKILL_CHAOS_SPLITTER_2_M, MappedMonsterTileSkill::STYLE_MELEE, 1, false);
	MappedMonsterTileSkill g_FireWaveM("FireWaveM", SKILL_FIRE_WAVE_M, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_ExtensiveDelusion2M("ExtensiveDelusion2M", SKILL_EXTENSIVE_DELUSION_2_M, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_BloodySkull2M("BloodySkull2M", SKILL_BLOODY_SKULL_2_M, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_TemenosM("TemenosM", SKILL_TEMENOS_M, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_WildWolfStrikeM("WildWolfStrikeM", SKILL_WILD_WOLF_STRIKE_M, MappedMonsterTileSkill::STYLE_MELEE, 1, false);
	MappedMonsterTileSkill g_VenomCircleM("VenomCircleM", SKILL_VENOM_CIRCLE_M, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_BatSpear2M("BatSpear2M", SKILL_BAT_SPEAR_2_M, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_FragrantBreath("FragrantBreath", SKILL_FRAGRANT_BREATH, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_TreeTrunkStrike("TreeTrunkStrike", SKILL_TREE_TRUNK_STRIKE, MappedMonsterTileSkill::STYLE_MELEE, 0, false);
	MappedMonsterTileSkill g_DeathfulBreath("DeathfulBreath", SKILL_DEATHFUL_BREATH, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_SpiritOfDeadSickle("SpiritOfDeadSickle", SKILL_SPIRIT_OF_DEAD_SICKLE, MappedMonsterTileSkill::STYLE_MELEE, 1, false);
	MappedMonsterTileSkill g_ComeOnOver("ComeOnOver", SKILL_COME_ON_OVER, MappedMonsterTileSkill::STYLE_MISSILE, 1, true);
	MappedMonsterTileSkill g_ErebusIsCry("ErebusIsCry", SKILL_EREBUS_IS_CRY, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_RollingContinue("RollingContinue", SKILL_ROLLING_CONTINUE, MappedMonsterTileSkill::STYLE_MELEE, 1, false);
	MappedMonsterTileSkill g_Bite("Bite", SKILL_BITE, MappedMonsterTileSkill::STYLE_MELEE, 0, false);
	MappedMonsterTileSkill g_ThornLaunch("ThornLaunch", SKILL_THORN_LAUNCH, MappedMonsterTileSkill::STYLE_MISSILE, 2, false);
	MappedMonsterTileSkill g_OneTwoCut("OneTwoCut", SKILL_ONE_TWO_CUT, MappedMonsterTileSkill::STYLE_MELEE, 0, false);
	MappedMonsterTileSkill g_Overeat("Overeat", SKILL_OVEREAT, MappedMonsterTileSkill::STYLE_MELEE, 0, false);
	MappedMonsterTileSkill g_ThornLaunchAddDebuff("ThornLaunchAddDebuff", SKILL_THORN_LAUNCH_ADD_DEBUFF, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
	MappedMonsterTileSkill g_TreeTrunkStrikeAddDebuff("TreeTrunkStrikeAddDebuff", SKILL_TREE_TRUNK_STRIKE_ADD_DEBUFF, MappedMonsterTileSkill::STYLE_MELEE, 1, true);
	MappedMonsterTileSkill g_SpiritOfDeadSickleAddDebuff("SpiritOfDeadSickleAddDebuff", SKILL_SPIRIT_OF_DEAD_SICKLE_ADD_DEBUFF, MappedMonsterTileSkill::STYLE_MELEE, 1, true);
	MappedMonsterTileSkill g_PowerfulClaw("PowerfulClaw", SKILL_POWERFUL_CLAW, MappedMonsterTileSkill::STYLE_MELEE, 0, false);
	MappedMonsterTileSkill g_AdelanteIsCry("AdelanteIsCry", SKILL_ADELANTE_IS_CRY, MappedMonsterTileSkill::STYLE_MISSILE, 2, true);
}
// 몬스터 레인지 스킬들
MonsterRangeSkill
	g_TraslaAttack(SKILL_TRASLA_ATTACK, false),
	g_PuscaAttack(SKILL_PUSCA_ATTACK, false),
	g_NodCopilaAttack(SKILL_NOD_COPILA_ATTACK, false),
	g_NodCopilaAttack2(SKILL_NOD_COPILA_ATTACK_2, false);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SkillHandlerManager::SkillHandlerManager () 
	throw ()
: m_SkillHandlers(NULL) , m_Size(SKILL_MAX)
{
	__BEGIN_TRY

	Assert(m_Size > 0);
	
	// 스킬 핸들러 배열을 생성한다.
	m_SkillHandlers = new SkillHandler*[ m_Size ];
	
	// 스킬 핸들러의 배열을 초기화한다.
	for (int i = 0 ; i < m_Size ; i ++) 
		m_SkillHandlers[i] = NULL;
			
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SkillHandlerManager::~SkillHandlerManager () 
	throw ()
{
	__BEGIN_TRY
		
	Assert(m_SkillHandlers != NULL);

	// 스킬 핸들러를 각각 삭제한다.
	for (int i = 0 ; i < m_Size ; i ++) 
	{
		SAFE_DELETE(m_SkillHandlers[i]);
	}
	
	// 스킬핸들러 배열을 삭제한다.
	SAFE_DELETE_ARRAY(m_SkillHandlers);
			
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SkillHandlerManager::init ()
	 throw (Error)
{
	__BEGIN_TRY
		
	// 기본 공격
	addSkillHandler(&g_AttackMelee);
	addSkillHandler(&g_AttackArms);

	// 검 계열
	addSkillHandler(&g_DoubleImpact);
	addSkillHandler(&g_TripleSlasher);
	addSkillHandler(&g_RainbowSlasher);
	addSkillHandler(&g_ThunderSpark);
	addSkillHandler(&g_DancingSword);
	addSkillHandler(&g_CrossCounter);
	addSkillHandler(&g_FlashSliding);
	addSkillHandler(&g_LightningHand);
	addSkillHandler(&g_SwordWave);
	addSkillHandler(&g_SnakeCombo);
	addSkillHandler(&g_WindDivider);
	addSkillHandler(&g_ThunderBolt);
	addSkillHandler(&g_Expansion);
	addSkillHandler(&g_MiracleShield);
	addSkillHandler(&g_ThunderFlash);
	addSkillHandler(&g_ThunderStorm);
	addSkillHandler(&g_MentalSword);

	//addSkillHandler(&g_DragonRising);
	//addSkillHandler(&g_FiveStormCrash);
	//addSkillHandler(&g_HeavensSword);

	// 도계열
	addSkillHandler(&g_SingleBlow);
	addSkillHandler(&g_SpiralSlay);
	addSkillHandler(&g_TripleBreak);
	addSkillHandler(&g_WildSmash);
	addSkillHandler(&g_GhostBlade);
	addSkillHandler(&g_PotentialExplosion);
	addSkillHandler(&g_ShadowWalk);
	addSkillHandler(&g_ChargingPower);
	addSkillHandler(&g_HurricaneCombo);
	addSkillHandler(&g_TornadoSever);
	addSkillHandler(&g_Earthquake);
	addSkillHandler(&g_Berserker);
	addSkillHandler(&g_MoonlightSever);
	addSkillHandler(&g_ShadowDancing);
	addSkillHandler(&g_Typhoon);

	// 군인 계열
	addSkillHandler(&g_QuickFire);
	addSkillHandler(&g_DoubleShot);
	addSkillHandler(&g_TripleShot);
	addSkillHandler(&g_MultiShot);
	addSkillHandler(&g_HeadShot);
	addSkillHandler(&g_Piercing);
	addSkillHandler(&g_Sniping);
	addSkillHandler(&g_MindControl);
	addSkillHandler(&g_Revealer);
	//addSkillHandler(&g_DetectMine);
	addSkillHandler(&g_InstallMine);
	addSkillHandler(&g_CreateBomb);
	addSkillHandler(&g_CreateMine);
	addSkillHandler(&g_ObservingEye);

	// 인챈 계열
	addSkillHandler(&g_CreateHolyWater);
	addSkillHandler(&g_Light);
	addSkillHandler(&g_DetectHidden);
	addSkillHandler(&g_AuraBall);
	addSkillHandler(&g_Bless);
	addSkillHandler(&g_ContinualLight);
	addSkillHandler(&g_Flare);
	addSkillHandler(&g_Purify);
	addSkillHandler(&g_AuraRing);
	addSkillHandler(&g_Striking);
	addSkillHandler(&g_DetectInvisibility);
	//addSkillHandler(&g_Identify);
	addSkillHandler(&g_AuraShield);
	addSkillHandler(&g_VigorDrop);
	//addSkillHandler(&g_Enchant);
	addSkillHandler(&g_Visible);
	addSkillHandler(&g_Sanctuary);
	addSkillHandler(&g_Reflection);
	addSkillHandler(&g_Hymn);

	// 힐링 계열
	addSkillHandler(&g_CureLightWounds);
	addSkillHandler(&g_CurePoison);
	addSkillHandler(&g_ProtectionFromPoison);
	addSkillHandler(&g_CauseLightWounds);
	addSkillHandler(&g_CureSeriousWounds);
	addSkillHandler(&g_RemoveCurse);
	addSkillHandler(&g_ProtectionFromCurse);
	addSkillHandler(&g_CauseSeriousWounds);
	addSkillHandler(&g_CureCriticalWounds);
	addSkillHandler(&g_ProtectionFromAcid);
	addSkillHandler(&g_Sacrifice);
	addSkillHandler(&g_CauseCriticalWounds);
	addSkillHandler(&g_Resurrect);
	addSkillHandler(&g_EnergyDrop);
	addSkillHandler(&g_CureAll);
	addSkillHandler(&g_Peace);
	addSkillHandler(&g_Activation);
	addSkillHandler(&g_HolyBlast);
	//addSkillHandler(&g_RegenerationSkill);
	//addSkillHandler(&g_MassCure);
	//addSkillHandler(&g_MassHeal);

	// 뱀프 계열
	addSkillHandler(&g_BloodDrain);

	addSkillHandler(&g_PoisonousHands);
	addSkillHandler(&g_GreenPoison);
	addSkillHandler(&g_YellowPoison);
	addSkillHandler(&g_DarkBluePoison);
	addSkillHandler(&g_GreenStalker);

	addSkillHandler(&g_AcidTouch);
	addSkillHandler(&g_AcidBolt);
	addSkillHandler(&g_AcidBall);
	addSkillHandler(&g_AcidSwamp);

	addSkillHandler(&g_Paralyze);
	addSkillHandler(&g_Doom);
	addSkillHandler(&g_Seduction);
	//addSkillHandler(&g_Blind);
	addSkillHandler(&g_Death);

	addSkillHandler(&g_BloodyNail);
	addSkillHandler(&g_BloodyKnife);
	addSkillHandler(&g_BloodyBall);
	addSkillHandler(&g_BloodyWave);
	addSkillHandler(&g_BloodyMasterWave);
	addSkillHandler(&g_BloodyWarp);
	addSkillHandler(&g_BloodySpear);
	addSkillHandler(&g_BloodyWall);
	addSkillHandler(&g_BloodySnake);
	addSkillHandler(&g_BloodyBreaker);
	addSkillHandler(&g_RapidGliding);

	addSkillHandler(&g_Transfusion);
	addSkillHandler(&g_Mephisto);

	addSkillHandler(&g_Hide);
	addSkillHandler(&g_Darkness);
	addSkillHandler(&g_Invisibility);
	addSkillHandler(&g_TransformToWolf);
	addSkillHandler(&g_TransformToBat);

	//addSkillHandler(&g_SummonWolf);
	addSkillHandler(&g_SummonCasket);
	addSkillHandler(&g_OpenCasket);
	//addSkillHandler(&g_RaisingDead);
	//addSkillHandler(&g_SummonServant);
	
	addSkillHandler(&g_BloodyMarker);
	addSkillHandler(&g_BloodyTunnel);

	addSkillHandler(&g_PoisonStrike);
	addSkillHandler(&g_AcidStrike);
	addSkillHandler(&g_BloodyStrike);
	addSkillHandler(&g_PoisonStorm);
	addSkillHandler(&g_AcidStorm);
	addSkillHandler(&g_BloodyStorm);
	addSkillHandler(&g_Extreme);

	addSkillHandler(&g_Unburrow);
	addSkillHandler(&g_Untransform);
	addSkillHandler(&g_Uninvisibility);

	addSkillHandler(&g_EatCorpse);
	addSkillHandler(&g_Howl);
	addSkillHandler(&g_ThrowHolyWater);

	addSkillHandler(&g_Restore);
	
	addSkillHandler(&g_CriticalGround);
	addSkillHandler(&g_SummonMonsters);
	addSkillHandler(&g_GroundAttack);
	addSkillHandler(&g_MeteorStrike);
	addSkillHandler(&g_Hallucination);
	addSkillHandler(&g_DuplicateSelf);

	addSkillHandler(&g_SoulChain);

	addSkillHandler(&g_SharpShield);
	addSkillHandler(&g_WideLightning);
	addSkillHandler(&g_GunShotGuidance);

	addSkillHandler(&g_AirShield);
	addSkillHandler(&g_BulletOfLight);
	addSkillHandler(&g_HandsOfWisdom);
	addSkillHandler(&g_LightBall);
	addSkillHandler(&g_HolyArrow);
	addSkillHandler(&g_Rebuke);
	addSkillHandler(&g_SpiritGuard);
	addSkillHandler(&g_Regeneration);
	addSkillHandler(&g_PowerOfLand);
	addSkillHandler(&g_TurnUndead);
	addSkillHandler(&g_Armageddon);

	addSkillHandler(&g_MagicElusion);
	addSkillHandler(&g_PoisonMesh);
	addSkillHandler(&g_IllusionOfAvenge);
	addSkillHandler(&g_WillOfLife);

	addSkillHandler(&g_DenialMagic);
	addSkillHandler(&g_SwordRay);
	addSkillHandler(&g_Concealment);
	addSkillHandler(&g_Requital);
	addSkillHandler(&g_MultiAmputate);

	addSkillHandler(&g_HitConvert);
	addSkillHandler(&g_WildTyphoon);
	addSkillHandler(&g_UltimateBlow);
	addSkillHandler(&g_Illendue);
	addSkillHandler(&g_Lightness);

	addSkillHandler(&g_Flourish);
	addSkillHandler(&g_Evade);
	addSkillHandler(&g_SharpRound);
	addSkillHandler(&g_BackStab);
	addSkillHandler(&g_Blunting);
	addSkillHandler(&g_GammaChop);
	addSkillHandler(&g_CrossGuard);
	addSkillHandler(&g_KasasArrow);
	addSkillHandler(&g_HandsOfFire);
	addSkillHandler(&g_Prominence);
	addSkillHandler(&g_RingOfFlare);
	addSkillHandler(&g_BlazeBolt);
	addSkillHandler(&g_IceField);
	addSkillHandler(&g_WaterBarrier);
	addSkillHandler(&g_NymphRecovery);
	addSkillHandler(&g_Liberty);
	addSkillHandler(&g_Tendril);
	addSkillHandler(&g_StoneAuger);

	addSkillHandler(&g_EarthsTeeth);
	addSkillHandler(&g_AbsorbSoul);
	addSkillHandler(&g_GnomesWhisper);
	addSkillHandler(&g_HandsOfNizie);
	addSkillHandler(&g_RefusalEther);

	addSkillHandler(&g_EmissionWater);
	addSkillHandler(&g_BeatHead);

	addSkillHandler(&g_DivineSpirits);
	addSkillHandler(&g_BlitzSliding);
	addSkillHandler(&g_JabbingVein);
	addSkillHandler(&g_GreatHeal);
	addSkillHandler(&g_DivineGuidance);
	addSkillHandler(&g_BlazeWalk);
	addSkillHandler(&g_BloodyZenith);

	addSkillHandler(&g_Rediance);
	addSkillHandler(&g_LarSlash);
	addSkillHandler(&g_Trident);

	addSkillHandler(&g_HeartCatalyst);
	addSkillHandler(&g_ProtectionFromBlood);
	addSkillHandler(&g_MoleShot);
	addSkillHandler(&g_Eternity);

	addSkillHandler(&g_InstallTrap);
	addSkillHandler(&g_HolyArmor);
	addSkillHandler(&g_MercyGround);
	addSkillHandler(&g_CreateHolyPotion);

	addSkillHandler(&g_TransformToWerwolf);
	addSkillHandler(&g_GrayDarkness);
	addSkillHandler(&g_StoneSkin);
	addSkillHandler(&g_TalonOfCrow);
	addSkillHandler(&g_BiteOfDeath);
	addSkillHandler(&g_AcidEruption);

	addSkillHandler(&g_Teleport);
	addSkillHandler(&g_FirePiercing);
	addSkillHandler(&g_SoulRebirth);
	addSkillHandler(&g_IceLance);
	addSkillHandler(&g_ExplosionWater);
	addSkillHandler(&g_FrozenArmor);
	addSkillHandler(&g_ReactiveArmor);
	addSkillHandler(&g_MagnumSpear);
	addSkillHandler(&g_HellFire);
	addSkillHandler(&g_GroundBless);

	addSkillHandler(&g_SharpChakram);
	addSkillHandler(&g_DestructionSpear);
	addSkillHandler(&g_ShiftBreak);
	addSkillHandler(&g_FatalSnick);
	addSkillHandler(&g_ChargingAttack);
	addSkillHandler(&g_DuckingWallop);
	addSkillHandler(&g_DistanceBlitz);
	addSkillHandler(&g_SummonGroundElemental);
	addSkillHandler(&g_SummonFireElemental);
	addSkillHandler(&g_SummonWaterElemental);
	addSkillHandler(&g_MeteorStorm);
	addSkillHandler(&g_WideIceField);

	addSkillHandler(&g_Glacier1);
	addSkillHandler(&g_Glacier2);
	addSkillHandler(&g_IceAuger);
	addSkillHandler(&g_IceHail);
	addSkillHandler(&g_WideIceHail);
	addSkillHandler(&g_IceWave);

	addSkillHandler(&g_LandMineExplosion);
	addSkillHandler(&g_ClaymoreExplosion);
	addSkillHandler(&g_PleasureExplosion);

	addSkillHandler(&g_DeleoEfficio);
	addSkillHandler(&g_ReputoFactum);
	addSkillHandler(&g_SwordOfThor);
	addSkillHandler(&g_BurningSolCharging);
	addSkillHandler(&g_BurningSolLaunch);
	addSkillHandler(&g_SweepVice1);
	addSkillHandler(&g_SweepVice3);
	addSkillHandler(&g_SweepVice5);
	addSkillHandler(&g_Whitsuntide);
	addSkillHandler(&g_ViolentPhantom);
	addSkillHandler(&g_InstallTurret);
	addSkillHandler(&g_TurretFire);
	addSkillHandler(&g_SummonGoreGland);
	addSkillHandler(&g_GoreGlandFire);
	addSkillHandler(&g_ThrowingAxe);
	addSkillHandler(&g_ChoppingFirewood);
	addSkillHandler(&g_ChainThrowingAxe);
	addSkillHandler(&g_MultiThrowingAxe);
	addSkillHandler(&g_PlayingWithFire);

	addSkillHandler(&g_InfinityThunderbolt);
	addSkillHandler(&g_SpitStream);
	addSkillHandler(&g_PlasmaRocketLauncher);
	addSkillHandler(&g_IntimateGrail);
	addSkillHandler(&g_BombingStar);
	addSkillHandler(&g_SetAfire);
	addSkillHandler(&g_NooseOfWraith);

	addSkillHandler(&g_SharpHail);
	addSkillHandler(&g_IceHorizon);
	addSkillHandler(&g_FuryOfGnome);
	addSkillHandler(&g_SummonMiga);
	addSkillHandler(&g_SummonMigaAttack);
	addSkillHandler(&g_Cannonade);
	addSkillHandler(&g_SelfDestruction);
	addSkillHandler(&g_ARAttack);
	addSkillHandler(&g_SMGAttack);
	addSkillHandler(&g_GrenadeAttack);
	addSkillHandler(&g_Halo);
	addSkillHandler(&g_Destinies);
	addSkillHandler(&g_FierceFlame);
	addSkillHandler(&g_ShadowOfStorm);
	addSkillHandler(&g_WildWolf);
	addSkillHandler(&g_Aberration);
	addSkillHandler(&g_DragonTornado);
	addSkillHandler(&g_BikeCrash);
	addSkillHandler(&g_HarpoonBomb);
	addSkillHandler(&g_PassingHeal);
	addSkillHandler(&g_RottenApple);
	addSkillHandler(&g_TraslaAttack);
	addSkillHandler(&g_PuscaAttack);
	addSkillHandler(&g_NodCopilaAttack);
	addSkillHandler(&g_NodCopilaAttack2);
	addSkillHandler(&g_UnterfeldwebelFire);
	addSkillHandler(&g_FeldwebelFire);
	addSkillHandler(&g_FeebleVirus);
	addSkillHandler(&g_SwordCombo);
	addSkillHandler(&g_BladeCombo);
	addSkillHandler(&g_VampireCombo);
	addSkillHandler(&g_OustersCombo);
	addSkillHandler(&g_HolySword);
	addSkillHandler(&g_HolyBlade);
	addSkillHandler(&g_BurstGun);
	addSkillHandler(&g_HolyMace);
	addSkillHandler(&g_HolyCross);
	addSkillHandler(&g_DarkForce);
	addSkillHandler(&g_NaturalForce);
	addSkillHandler(&g_FireForce);
	addSkillHandler(&g_WaterForce);
	addSkillHandler(&g_EarthForce);
	addSkillHandler(&g_LoseSight);
	addSkillHandler(&g_WideBloodDrain);
	addSkillHandler(&g_NaturalPeace);
	addSkillHandler(&g_PartyAura);

	// 2006.06.23
	addSkillHandler(&g_Brandish);
	addSkillHandler(&g_SoundWave);
	addSkillHandler(&g_Parasite);
	addSkillHandler(&g_FireWave);

	// 2006.07.20
	addSkillHandler(&g_TargetSummonMonsters);

	// 2006.08.16
	addSkillHandler(&g_BloodyBurst);
	addSkillHandler(&g_TraceCreature);

	addSkillHandler(&g_ShineSword);
	addSkillHandler(&g_BombCrashWalk);
	addSkillHandler(&g_SatelliteBomb);
	addSkillHandler(&g_IllusionInversion);
	addSkillHandler(&g_HeavenGround);
	addSkillHandler(&g_BloodyScarify);
	addSkillHandler(&g_BloodCurse);
	addSkillHandler(&g_BloodsSymposionAttack);
	addSkillHandler(&g_DummyDrake);
	addSkillHandler(&g_HydroConvergence);
	addSkillHandler(&g_SummonClay);
	addSkillHandler(&g_HeterChakram);

	// 2007.01.22
	addSkillHandler(&g_Tanning);

	// 2007.02.08
	addSkillHandler(&g_RequestResurrect);

	// 20070820
	addSkillHandler(&g_MagicShield1);
	// 20070828
	addSkillHandler(&g_MagicShield2);
	addSkillHandler(&g_EaglesEye1);
	addSkillHandler(&g_EaglesEye2);
	addSkillHandler(&g_FreezeRing1);
	addSkillHandler(&g_FreezeRing2);
	addSkillHandler(&g_BatStorm1);
	addSkillHandler(&g_BatStorm2);
	addSkillHandler(&g_CurseOfBlood1);
	addSkillHandler(&g_CurseOfBlood2);
	addSkillHandler(&g_BloodDrain2);
	addSkillHandler(&g_BloodyShout1);
	addSkillHandler(&g_BloodyShout2);
	addSkillHandler(&g_BloodyWings);
	addSkillHandler(&g_MistOfSoul1);
	addSkillHandler(&g_MistOfSoul2);

#ifdef __MENEGROTH_DOUNGEON_SYSTEM__
	addSkillHandler(new SkillScript("AttackCerberus", SKILL_ATTACK_CERBERUS));
	addSkillHandler(new SkillScript("AttackManticoret", SKILL_ATTACK_MANTICORET));
	addSkillHandler(new SkillScript("AttackBogletH", SKILL_ATTACK_BOGLETH));
	addSkillHandler(new SkillScript("AttackBogletB", SKILL_ATTACK_BOGLETB));
	addSkillHandler(new SkillScript("AttackMassacre", SKILL_ATTACK_MASSACRE));
	addSkillHandler(new SkillScript("AttackPlumpyBoar", SKILL_ATTACK_PLUMPYBOAR));
	addSkillHandler(new SkillScript("AttackCabracam", SKILL_ATTACK_CABRACAM));

	addSkillHandler(new SkillScript("MenegrothTrap1", SKILL_MENEGROTH_TRAP_1));
	addSkillHandler(new SkillScript("MenegrothTrap2", SKILL_MENEGROTH_TRAP_2));
#endif // __MENEGROTH_DOUNGEON_SYSTEM__

	addSkillHandler(&g_HeavenlySpark); // 20080401
	addSkillHandler(&g_BladeStorm); // 20080403
	addSkillHandler(&g_ExplosionRocketLauncher); // 20080407
	addSkillHandler(&g_Genesis); // 20080408
	addSkillHandler(&g_Thorns); // 20080408
	addSkillHandler(&g_LethalClaw); // 20080410
	addSkillHandler(&g_VenomCircle); // 20080414
	addSkillHandler(&g_Brionac); // 20080415
	addSkillHandler(&g_AsteroidBomber); // 20080415
	addSkillHandler(&g_FrozenGlacier); // 20080415
	addSkillHandler(&g_Incinerate); // 20080415
	
	addSkillHandler(new SkillScript("TiffaugesBalberithBlue", SKILL_TIFFAUGES_BALBERITH_BLUE));
	addSkillHandler(new SkillScript("TiffaugesBalberithRed", SKILL_TIFFAUGES_BALBERITH_RED));
	addSkillHandler(new SkillScript("TiffaugesBerithBlue", SKILL_TIFFAUGES_BERITH_BLUE));
	addSkillHandler(new SkillScript("TiffaugesBerithRed", SKILL_TIFFAUGES_BERITH_RED));
	addSkillHandler(new SkillScript("TiffaugesValkyrjaBlue", SKILL_TIFFAUGES_VALKYRJA_BLUE));
	addSkillHandler(new SkillScript("TiffaugesValkyrjaRed", SKILL_TIFFAUGES_VALKYRJA_RED));
	addSkillHandler(new SkillScript("TiffaugesGomoryABlue", SKILL_TIFFAUGES_GOMORY_A_BLUE));
	addSkillHandler(new SkillScript("TiffaugesGomoryBBlue", SKILL_TIFFAUGES_GOMORY_B_BLUE));
	addSkillHandler(new SkillScript("TiffaugesGomoryARed", SKILL_TIFFAUGES_GOMORY_A_RED));
	addSkillHandler(new SkillScript("TiffaugesGomoryBRed", SKILL_TIFFAUGES_GOMORY_B_RED));
	addSkillHandler(new SkillScript("TiffaugesKrsnikABlue", SKILL_TIFFAUGES_KRSNIK_A_BLUE));
	addSkillHandler(new SkillScript("TiffaugesKrsnikBBlue", SKILL_TIFFAUGES_KRSNIK_B_BLUE));
	addSkillHandler(new SkillScript("TiffaugesKrsnikARed", SKILL_TIFFAUGES_KRSNIK_A_RED));
	addSkillHandler(new SkillScript("TiffaugesKrsnikBRed", SKILL_TIFFAUGES_KRSNIK_B_RED));
	addSkillHandler(new SkillScript("TiffaugesGusionABlue", SKILL_TIFFAUGES_GUSION_A_BLUE));
	addSkillHandler(new SkillScript("TiffaugesGusionBBlue", SKILL_TIFFAUGES_GUSION_B_BLUE));
	addSkillHandler(new SkillScript("TiffaugesGusionCBlue", SKILL_TIFFAUGES_GUSION_C_BLUE));
	addSkillHandler(new SkillScript("TiffaugesGusionARed", SKILL_TIFFAUGES_GUSION_A_RED));
	addSkillHandler(new SkillScript("TiffaugesGusionBRed", SKILL_TIFFAUGES_GUSION_B_RED));
	addSkillHandler(new SkillScript("TiffaugesGusionCRed", SKILL_TIFFAUGES_GUSION_C_RED));
	addSkillHandler(new SkillScript("TiffaugesPaimonBlue", SKILL_TIFFAUGES_PAIMON_BLUE));
	addSkillHandler(new SkillScript("TiffaugesPaimonRed", SKILL_TIFFAUGES_PAIMON_RED));
	addSkillHandler(new SkillScript("TiffaugesMorgothABlue", SKILL_TIFFAUGES_MORGOTH_A_BLUE));
	addSkillHandler(new SkillScript("TiffaugesMorgothBBlue", SKILL_TIFFAUGES_MORGOTH_B_BLUE));
	addSkillHandler(new SkillScript("TiffaugesMorgothCBlue", SKILL_TIFFAUGES_MORGOTH_C_BLUE));
	addSkillHandler(new SkillScript("TiffaugesMorgothARed", SKILL_TIFFAUGES_MORGOTH_A_RED));
	addSkillHandler(new SkillScript("TiffaugesMorgothBRed", SKILL_TIFFAUGES_MORGOTH_B_RED));
	addSkillHandler(new SkillScript("TiffaugesMorgothCRed", SKILL_TIFFAUGES_MORGOTH_C_RED));
	
	addSkillHandler(new OniblaRaiseBone());
	addSkillHandler(new OniblaDropSkull());
	addSkillHandler(new AgonyChildDropHead());
	addSkillHandler(new AgonyChildShadowFour());
	addSkillHandler(new VillainLadyGhostShadow());
	addSkillHandler(new VillainLadySelfDestruction());
	addSkillHandler(new OniSpanielThrowingAxe());
	addSkillHandler(new BlackAstralMagicCrystal());
	addSkillHandler(new WhiteAstralSpellCrystal());
	
	addSkillHandler(&g_MagmaDetonation);
	addSkillHandler(&g_MagmaDetonation2);
	addSkillHandler(&g_SquallyBarrier1);
	addSkillHandler(&g_SquallyBarrier2);
	addSkillHandler(new SiusCrash());
	addSkillHandler(new LycanHowl());
	addSkillHandler(new BifronzeMagic());
	addSkillHandler(new PalusMiddleRangeMagic());
	addSkillHandler(new PalusLongRangeMagic());
	addSkillHandler(new GarumMagic());
	addSkillHandler(new ClavieMagic());
	addSkillHandler(new WideWildWolf());
	addSkillHandler(new BlessofLycan());
	addSkillHandler(new SmileofDeath());
	addSkillHandler(new FuryShock());
	addSkillHandler(new RailGunAttack());

	addSkillHandler(&g_BLAZEWALK2);
	addSkillHandler(&g_GLADIATOR);
	addSkillHandler(&g_CRUSHINGSTORM);
	addSkillHandler(&g_SATELLITEBOMB2);
	addSkillHandler(&g_INTIMATEGRAIL2);
	addSkillHandler(&g_HOLYARMOR2);
	addSkillHandler(&g_STRIKING2);
	addSkillHandler(&g_BLESS2);
	addSkillHandler(&g_GLORYGROUND);
	addSkillHandler(&g_PASSINGHEALK2);
	addSkillHandler(&g_GREATHEAL2);
	addSkillHandler(&g_CHAINOFDEMON);
	addSkillHandler(&g_BLOODYSKULL);
	addSkillHandler(&g_RAGEOFBLOOD);
	addSkillHandler(&g_ALLYSTERWIND);
	addSkillHandler(&g_FLAMESIGHT);
	addSkillHandler(&g_SPIRALMEGALITH);
	addSkillHandler(&g_RADCHIA);
	addSkillHandler(&g_HEALPASS2);
	addSkillHandler(&g_BREATHOFDRYAD);
        addSkillHandler(new CUREEFFECT2);
	addSkillHandler(new LIVINGDEADBECHOESPER());
	addSkillHandler(new MIHAIBATSTREAM());
	addSkillHandler(new TSARBLUEROSEDEVIL());
	addSkillHandler(new FOLLETPOISONFRAME());
	addSkillHandler(new FUEFOLLETREDROSEDEVIL());
	addSkillHandler(new DACULETALONOFDDRACULE());
	addSkillHandler(new DRACULEBLOOKDYSCARIFY2());
	addSkillHandler(new DRACULEBLOODYCARPE());
	addSkillHandler(new DRACULEDRACULECASKET());
	addSkillHandler(new DRAGONHURRICANE());
	addSkillHandler(&g_LARSTROKE);
	addSkillHandler(&g_GAEBULGA);
	addSkillHandler(&g_VICIOUSGUIDANCE);
	addSkillHandler(&g_SPECTORINVERSE);
	addSkillHandler(&g_BATBREAKER);
	addSkillHandler(&g_SHADYDOUPLE);
	addSkillHandler(&g_CHAKRAMHAIL);
	addSkillHandler(&g_FLAMESPIKE);
	addSkillHandler(&g_BLIZZARD);
	addSkillHandler(&g_DEMOLISHER);
        addSkillHandler(&g_MAGMATICERUPTION);
        addSkillHandler(&g_ADV2LIGHTINGSHOCK);
        addSkillHandler(&g_ADV2HALCYONMK1);
        addSkillHandler(&g_MK1Fire);
        addSkillHandler(&g_ADV2SOLARFLARE);
        addSkillHandler(&g_ADV2VIACRUCIS);
        addSkillHandler(&g_ADV2CHAOSSPLITTER);
        addSkillHandler(&g_ADV2DEADLYGROUND);
        addSkillHandler(&g_ADV2SUDDENBLAST);
        addSkillHandler(&g_ADV2METEORBLAZE);
        addSkillHandler(&g_ADV2TORRENTIALLIFE);
        addSkillHandler(&g_ADV2CYCLINGWARROCK);
        addSkillHandler(&g_ADV2TRUECLOLOROFVAMPLOAD);
        addSkillHandler(&g_ADV2THEROAROFAWAKENWARRIOR);
        addSkillHandler(&g_ADV2THERAGEOFGUARDIANSPRIT);
        addSkillHandler(&g_DEADLYATT);
        addSkillHandler(&g_ENRAGECONQUROR);
        addSkillHandler(&g_ATTRACTCIRCLE);
        addSkillHandler(&g_IGNISINFERNUM);
        addSkillHandler(&g_DEMONICRESURRECTIO);
        addSkillHandler(&g_WOLFROAR);
        addSkillHandler(&g_EVGENEIABELLUM);
        addSkillHandler(&g_OUTRAGEBOOSTER);
        addSkillHandler(&g_ICEPRESSURE);
        addSkillHandler(&g_CRYSTALTHORN);
        addSkillHandler(&g_FAVOROFLUNA);
        addSkillHandler(&g_TEMENOS);
        addSkillHandler(&g_PULSEMAGNETICFIELD);
        addSkillHandler(&g_DEPENDSOUL);
        addSkillHandler(new ULRAYIRYMELLEATTACKA());
        addSkillHandler(new ULRAYIRYMELLEATTACKB());
        addSkillHandler(new TRANSFORMDARKBATSTORM());
	addSkillHandler(new CLIENTSHADYDOUPLE());
	addSkillHandler(new CLIENTBATBREAKER());
	addSkillHandler(&g_ADV2ADVENTBLADE);//221 blaze
	addSkillHandler(&g_ADV2CHAINLIGHTNINGSWORD);//221 sword
	addSkillHandler(&g_ADV2HALCYONMK1NAPALNBULLET);//221 gun
	addSkillHandler(&g_ADV2CONTRADICTO);//221 enchant
	addSkillHandler(&g_ADV2DIVINETRACES);//221 heal
	addSkillHandler(&g_ADV2WILDWOLFSTRIKE);//221 zhan gui
	addSkillHandler(&g_ADV2DEADLYSTORM);//221 zhi gui
	addSkillHandler(&g_ADV2SHININGCHAKRAMSPIN);//221 zhan mo
	addSkillHandler(&g_ADV2INFINITYBLAZE); // 221 huo mo
	addSkillHandler(&g_ADV2GRACEOFELQUINES); // 221 shui mo	
	addSkillHandler(&g_ADV2SUMMONDEFENSIVESPINEL); // 221 tu mo	
	addSkillHandler(&g_CREATUREGOLEMQUAKEATTACK); // boss skill 
	addSkillHandler(&g_LAYWASTE); // boss skill 
	addSkillHandler(&g_VOMIT); // boss skill 
	addSkillHandler(&g_HALFMOONKICK); // boss skill BLOODPUNISH
	addSkillHandler(&g_BLOODPUNISH);
	addSkillHandler(&g_BLOODPRAY);
	addSkillHandler(&g_DEVILWAVE);
	addSkillHandler(&g_PARASITEATTACK);
	addSkillHandler(&g_DEVILSCREAM);
	addSkillHandler(&g_BONESTRIKE);
	addSkillHandler(&g_LIGHTNINGSLASH);
	addSkillHandler(&g_GENESISATTACK);
	addSkillHandler(&g_BLOODYSKULL2);
	addSkillHandler(&g_ADV2CHAOSSPLITTER2);
	addSkillHandler(&g_CRYSTALTHORN2);
	addSkillHandler(&g_BLIZZARD2);
	addSkillHandler(&g_ADV2METEORBLAZE2);
	addSkillHandler(&g_CHAKRAMHAIL2);
	addSkillHandler(&g_ADV2SOLARFLARE2);
	addSkillHandler(&g_ADV2VIACRUCIS2);
	addSkillHandler(&g_MK1Fire2);
	addSkillHandler(&g_ADV2LIGHTINGSHOCK2);
	addSkillHandler(&g_DRAGONHURRICANE2);

	addSkillHandler(&g_BLADEERUPTION1);
	addSkillHandler(&g_ATTRACTSWORDCRASH1);
	addSkillHandler(&g_STRATEGICBOMBER1);
	addSkillHandler(&g_AURAEXPLOSION1);
	addSkillHandler(&g_HOLYACIDALIUM1);

	addSkillHandler(&g_BATSPEAR1);
	addSkillHandler(&g_EXTENSIVEDELUSION1);
	addSkillHandler(&g_ACCELERATECHARGE1);
	addSkillHandler(&g_RAGEBURST1);
	addSkillHandler(&g_FROZENGROUND1);
	addSkillHandler(&g_METEORICFALL1);

	addSkillHandler(&g_BLADEERUPTION2);
	addSkillHandler(&g_ATTRACTSWORDCRASH2);
	addSkillHandler(&g_STRATEGICBOMBER2);
	addSkillHandler(&g_AURAEXPLOSION2);
	addSkillHandler(&g_HOLYACIDALIUM2);

	addSkillHandler(&g_BATSPEAR2);
	addSkillHandler(&g_EXTENSIVEDELUSION2);
	addSkillHandler(&g_ACCELERATECHARGE2);
	addSkillHandler(&g_RAGEBURST2);
	addSkillHandler(&g_FROZENGROUND2);
	addSkillHandler(&g_METEORICFALL2);

	addSkillHandler(&g_WARPPORTAL);

	addSkillHandler(&g_SetAfire2);
	addSkillHandler(&g_SHADYDOUPLE2);
	addSkillHandler(&g_PotentialExplosion2);
	addSkillHandler(&g_SharpShield2);
	addSkillHandler(&g_Sniping2);
	addSkillHandler(&g_Thorns2);
	addSkillHandler(&g_CureSeriousWounds2);
	addSkillHandler(&g_Halo2);
	addSkillHandler(&g_RingOfFlare2);
	addSkillHandler(&g_WaterBarrier2);
	addSkillHandler(&g_ReactiveArmor2);

	addSkillHandler(&g_IronMaiden);
	addSkillHandler(&g_ChaosSplitter2M);
	addSkillHandler(&g_FireWaveM);
	addSkillHandler(&g_ExtensiveDelusion2M);
	addSkillHandler(&g_BloodySkull2M);
	addSkillHandler(&g_TemenosM);
	addSkillHandler(&g_WildWolfStrikeM);
	addSkillHandler(&g_VenomCircleM);
	addSkillHandler(&g_BatSpear2M);
	addSkillHandler(&g_FragrantBreath);
	addSkillHandler(&g_TreeTrunkStrike);
	addSkillHandler(&g_DeathfulBreath);
	addSkillHandler(&g_SpiritOfDeadSickle);
	addSkillHandler(&g_ComeOnOver);
	addSkillHandler(&g_ErebusIsCry);
	addSkillHandler(&g_RollingContinue);
	addSkillHandler(&g_Bite);
	addSkillHandler(&g_ThornLaunch);
	addSkillHandler(&g_OneTwoCut);
	addSkillHandler(&g_Overeat);
	addSkillHandler(&g_ThornLaunchAddDebuff);
	addSkillHandler(&g_TreeTrunkStrikeAddDebuff);
	addSkillHandler(&g_SpiritOfDeadSickleAddDebuff);
	addSkillHandler(&g_PowerfulClaw);
	addSkillHandler(&g_AdelanteIsCry);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SkillHandlerManager::addSkillHandler (SkillHandler * pSkillHandler) 
	 throw (Error)
{
	__BEGIN_TRY

	if (pSkillHandler == NULL)
	{
		throw Error("null Skill Handler");
	}

	SkillType_t SkillType = pSkillHandler->getSkillType();

	if ((int)SkillType < 0 || SkillType >= m_Size)
	{
		StringStream msg;
		msg << "Skill Handler Upper Bound (Type : " << (int)SkillType << ", Max : " << m_Size << ")";
		throw Error(msg.toString());
	}

	if (m_SkillHandlers[ SkillType ] != NULL) {
		StringStream msg;
		msg << "duplicate Skill Handler, " << pSkillHandler->getSkillHandlerName () ;
		throw Error(msg.toString());
	}
	
	m_SkillHandlers[ SkillType ] = pSkillHandler;
			
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SkillHandler * SkillHandlerManager::getSkillHandler (SkillType_t SkillType)
	throw (Error)
{
	__BEGIN_TRY

	// Wide Storm 을 가지고 있을 경우, Wide 처리는 원래 기술 안에서 처리한다.
/*	if ( SkillType == SKILL_DARKNESS_WIDE )
	{
		SkillType = SKILL_DARKNESS;
	}
	else if ( SkillType == SKILL_POISON_STORM_WIDE )
	{
		SkillType = SKILL_POISON_STORM;
	}
	else if ( SkillType == SKILL_ACID_STORM_WIDE )
	{
		SkillType = SKILL_ACID_STORM;
	}
*/
	if ((int)SkillType < 0 || SkillType >= m_Size)
	{
		StringStream msg;
		msg << "Skill Handler Upper Bound (Type : " << (int)SkillType << ", Max : " << m_Size << ")";
		throw Error(msg.toString());
	}

	if (m_SkillHandlers[SkillType]  == NULL)
	{
		StringStream msg;
		msg << "None Skill Handler (Type : " << (int)SkillType << ")";
		throw Error(msg.toString());
	}

	return m_SkillHandlers[SkillType];
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string SkillHandlerManager::toString () const
	throw ()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "SkillHandlerManager(\n";
	for (int i = 0 ; i < m_Size ; i ++)
	{
		msg << "SkillHandlers[" << i << "] == ";
		msg	<< (m_SkillHandlers[i] == NULL ? "NULL" : m_SkillHandlers[i]->getSkillHandlerName());
		msg << "\n";
	}
	msg << ")";
	return msg.toString();

	__END_CATCH
}
	
