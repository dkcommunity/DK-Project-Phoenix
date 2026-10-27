#----------------------------------------------------------------------
#
# Filename    : Makefile
# Written By  : Reiot
# Description :
#
#----------------------------------------------------------------------

#----------------------------------------------------------------------
# Directories Setting
#----------------------------------------------------------------------
VSHOME   = ../../../..
STLDIR =
BINDIR   = $(VSHOME)/bin
LIBDIR   = $(VSHOME)/lib
LOGDIR   = $(VSHOME)/log
SRCDIR   = ../../..
COREDIR  = ../../../Core
SERVDIR  = ../..
DBDIR    = ../../database
MYSQLDIR = /usr/local/lib/mysql
GSDIR    = ..
SKILLDIR = ../skill
ITEMDIR  = ../item
WARDIR  = ../war
#SIMPLEQUESTDIR  = ./Squest
#LUASCRIPTDIR  = ./luaScript


#----------------------------------------------------------------------
# General Rules
#----------------------------------------------------------------------
.SUFFIXES    : .cpp .o

CC       = g++
DEFS     = -D__LINUX__ -D_REENTRANT -D__GAME_SERVER__
INCLUDES =  -I$(COREDIR) -I$(SERVDIR) -I$(DBDIR) -I$(GSDIR) -I$(SKILLDIR) -I$(ITEMDIR) -I$(WARDIR) -I$(LUASCRIPTDIR) -I.
CCFLAGS  = -Wall -c -g -std=c++98 -std=c++11 -U_GLIBCXX_ASSERTIONS -D_GLIBCXX_NO_ASSERTIONS -fno-stack-protector -fno-enforce-eh-specs
#CCFLAGS  = -Wall -c

.cpp.o :
	$(CC) $(CCFLAGS) $(DEFS) $(INCLUDES) $<

#----------------------------------------------------------------------
# Objects
#----------------------------------------------------------------------

QUEST_OBJS = Trigger.o TriggerManager.o PropertyBuffer.o VariableBuffer.o VariableInfo.o TriggerParser.o

CONDITION_OBJS = \
	ConditionFactoryManager.o \
	ConditionEveryTime.o \
	ConditionAnsweredBy.o \
	ConditionFlagOn.o \
	ConditionFlagOff.o \
	ConditionAttrComp.o \
	ConditionRaceComp.o \
	ConditionSameClan.o \
	ConditionPayPlay.o \
	ConditionEnterMasterLair.o \
	ConditionEnterCastle.o \
	ConditionEnterHolyLand.o \
ConditionEnterCastleDungeon.o \
	ConditionHasQuest.o \
	ConditionHasInvenSpace.o \
	ConditionCanEnterPayZone.o \
	ConditionCanEnterBeginnerZone.o \
	ConditionCanWarpGate.o \
	ConditionCanEnterLevelWarZone.o \
	ConditionCanPetQuest.o \
	ConditionCanEnterEventZone.o \
	ConditionEffectFlag.o \
	ConditionCanEnterGDRLair.o \
	ConditionExistReinforce.o \
	ConditionSiegeDefenderSide.o \
	ConditionSiegeAttackerSide.o \
	ConditionNotGuildMember.o \
	ConditionIsGuildMember.o \
	ConditionCanEnterQuestZone.o \
	ConditionHasEnoughFamilyCoin.o \
	ConditionEnterMikllizzLair.o \
	ConditionEnterHellGardenTower.o \
	ConditionTalkedBy.o

ACTION_OBJS = \
	ActionFactoryManager.o \
	ActionSetPosition.o \
	ActionWander.o \
	ActionSay.o \
	ActionRandomSay.o \
	ActionAsk.o \
	ActionQuitDialogue.o \
	ActionPrepareShop.o \
	ActionSell.o \
	ActionBuy.o \
	ActionRegenShop.o \
	ActionTeachSkill.o \
	ActionHeal.o \
	ActionRedeemMotorcycle.o \
	ActionSearchMotorcycle.o \
	ActionRepair.o \
	ActionStashSell.o \
	ActionStashOpen.o \
	ActionRestore.o \
	ActionSetResurrectZone.o \
	ActionSilverCoating.o \
	ActionCreateGuild.o \
	ActionDestroyGuild.o \
	ActionTutorial.o \
	ActionGiveNewbieItem.o \
	ActionActivatePortal.o \
	ActionTurnOnFlag.o \
	ActionTurnOffFlag.o \
	ActionSystemMessage.o \
	ActionTakeDonation.o \
	ActionCallHelicopter.o \
	ActionRegenEventShop.o \
	ActionAskDynamic.o \
	ActionSayDynamic.o \
	ActionGiveSpecialEventItem.o \
	ActionRedistributeAttr.o \
	ActionTradeEventItem.o \
	ActionTradeLairItem.o \
	ActionShowGuildDialog.o \
	ActionSimpleQuestList.o \
	ActionSimpleQuestComplete.o \
	ActionSimpleQuestRegen.o \
	ActionEnterCastleWithFee.o \
	ActionShowTaxBalance.o \
	ActionAskVariable.o \
	ActionWarRegistration.o \
	ActionShowWarSchedule.o \
	ActionJoinRaceWar.o \
	ActionInitPartnerWaitingManager.o \
	ActionWaitForMeetCouple.o \
	ActionWaitForApartCouple.o \
	ActionAcceptCoupleRequest.o \
	ActionAcceptApartCouple.o \
	ActionForceApartCouple.o \
	ActionEnterPKZone.o \
	ActionWarpToResurrectPosition.o \
	ActionSelectQuest.o \
	ActionQuestReward.o \
	ActionWarpToNoviceZone.o \
	ActionCancelQuest.o \
	ActionInitSimpleQuest.o \
	ActionWanderZone.o \
	ActionInitEventQuest.o \
	ActionAskByQuestLevel.o \
	ActionGiveEventQuest.o \
	ActionAdvanceEventQuest.o \
	ActionCancelEventQuestScript.o \
	ActionRewardEventQuest.o \
	ActionGiveLotto.o \
	ActionGiveEventQuestScript.o \
	ActionStartEventQuest.o \
	ActionWarpInZone.o \
	ActionEventMeet.o \
	ActionGiveFinalLottoScript.o \
	ActionGiveQuestItem.o \
	ActionTakeOutGoods.o \
	ActionClearRankBonus.o \
	ActionContractGnomesHorn.o \
	ActionDownSkill.o \
	ActionMiniGame.o \
	ActionGiveItem.o \
	ActionActivateMazeExit.o \
	ActionActivateMazeEnter.o \
	ActionActivateMazeReturn.o \
	ActionSystemMessagePerRace.o \
	ActionWarpLevelWarZone.o \
	ActionCheckPartnerName.o \
	ActionStartPetQuest.o \
	ActionPetWithdraw.o \
	ActionPetDeposit.o \
	ActionEnterEventZone.o \
	ActionEnterGDRLair.o \
	ActionTradeGQuestEventItem.o \
	ActionRegisterSiege.o \
	ActionRegisterReinforce.o \
	ActionAcceptReinforce.o \
	ActionDenyReinforce.o \
	ActionEnterSiege.o \
	ActionRecallSiege.o \
	ActionSelectBloodBible.o \
	ActionClearBloodBible.o \
	ActionModifyTaxRatio.o \
	ActionSwapAdvancementItem.o \
	ActionShowDonationDialog.o \
	ActionEnterQuestZone.o \
	ActionShowConfirmGetEventItemDialog.o \
	ActionGiveCommonEventItem.o \
	ActionGiveCommonQuestItem.o \
	ActionCheckCommonQuestItem.o \
	ActionResetAttribute.o \
	ActionCheckEventLevelUp200604.o \
	ActionGiveEventLevelUp200604.o \
	ActionGiveComebackEventItem200604.o \
	ActionCheckComebackEventItem200604.o \
	ActionRedistributeAttr50.o \
	ActionChangePlayerColor.o \
	ActionExchangeOfMoney.o \
	ActionSoccerEvent200606.o \
	ActionTradeSealingStone.o \
	ActionEnterCastleB2F.o \
	ActionEnterSurvivalZone.o \
	ActionGambleSystem.o \
	ActionCheckGlutinousRiceKneading.o \
	ActionGiveEventBoxItem.o \
	ActionTradeMysteryPotion.o \
	ActionTradeLeather.o \
	ActionGiveHellGardenCrossItem.o \
	ActionGiveHellGardenSeedItem.o \
	ActionEnterHellGarden.o \
	ActionAttendCheck.o \
	ActionEnterMenegroth.o \
	ActionGiveContributeReward.o \
	ActionInsertCabracamHeart.o \
	ActionCheckLevelWarReward.o \
	ActionGiveCrashingReward.o \
	ActionInitInstanceDoungeon.o \
	ActionBlitzRegister.o \
	ActionBlitzReward.o \
	ActionMapleRegister.o \
	ActionRecallParty.o \
	ActionMapleReward.o \
	ActionMonsterSummon.o \
	ActionEnterInstant.o \
	ActionQuitMobaQueue.o \
	ActionOpenAuctionHouse.o
#	ActionTradeGiftBox.o
#	ActionGiveTestServerReward.o
#	ActionGiveEventItem.o
#	ActionGiveAccountEventItem.o

OBJS = $(QUEST_OBJS) $(CONDITION_OBJS) $(ACTION_OBJS)

SRCS = $(OBJS:.o=.cpp)

TARGET   = $(LIBDIR)/libQuest.a


#----------------------------------------------------------------------
# actions
#----------------------------------------------------------------------
all :
#	$(MAKE) -C luaScript
	$(MAKE) $(TARGET)
#$(MAKE) -C Squest

$(TARGET) : $(OBJS)
	ar rc $@ $^

clean :
	rm -rf *.o
#	$(MAKE) -C luaScript clean
#	$(MAKE) -C Squest clean

cleanall :
	rm -rf *.o $(TARGET)
#	$(MAKE) -C luaScript cleanall
#	$(MAKE) -C Squest cleanll

depall :
	gccmakedep $(SRCS) $(INCLUDES)
#	$(MAKE) -C luaScript depall
#	$(MAKE) -C Squest depall

dep :
	gccmakedep $(SRCS) $(INCLUDES)

# DO NOT DELETE
