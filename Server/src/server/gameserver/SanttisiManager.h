//////////////////////////////////////////////////////////////////////////////
// Filename    : HellGardenTowerManager.h 
// Written by  : œ­
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __Santtisi_MANAGER_H__
#define __Santtisi_MANAGER_H__

#include "Timeval.h"
#include "Mutex.h"

//////////////////////////////////////////////////////////////////////////////
// class HellGardenTowerManager
//////////////////////////////////////////////////////////////////////////////

class Zone;
class Creature;
class PlayerCreature;

class SanttisiManager
{
public : 
	enum SanttisiState
	{ 
		STATE_OPEN,
		STATE_WAIT,
		STATE_STAGE_1,		// Ãþž¶ŽÙ ClearÇÏžé ºž»ó ·£Žý Áö±Þ (ŸŸŸÑ)
		STATE_STAGE_2,
		STATE_STAGE_3,
		STATE_STAGE_4,
		STATE_STAGE_5,
		STATE_STAGE_6,
		STATE_STAGE_7,
		STATE_STAGE_8,
		STATE_STAGE_9,
		STATE_STAGE_10,
		STATE_INIT,			// ŸÖŽúÇÑÅ× ºž»ó ŸÆÀÌÅÛ Áö±Þ(œÊÀÚ°¡, ³»¿ëÈ®ÀÎ) ÃÊ±âÈ­

		STATE_MAX
	}; 

	// Çï°¡µç ÅžÀÔ °¢ Ãþ¿¡ ÇØŽçÇÑŽÙ.
	enum SanttisiType
	{
		TYPE_Santtisi_1,
		TYPE_Santtisi_MAX
	};
	enum SanttisiTowerType
	{
		TYPE_Santtisi_TOWER_0,
		TYPE_Santtisi_TOWER_1,
		TYPE_Santtisi_TOWER_MAX
	};

	// Çï°¡µç Åž¿ö ÅžÀÔ Çï°¡µç °¢ÃþÀÇ 4°³ÀÇ Åž¿ö¿¡ ÇØŽçÇÑŽÙ.

	struct MonsterTypeList
	{
		MonsterType_t	MonsterType;
		int				MonsterCount;
	};

public:
	SanttisiManager(Zone* pZone) throw(Error);
	~SanttisiManager() throw();

	int getCurrentState() const { return m_State; }

	bool isSummonTiming() throw(Error);

	bool enterPC(PlayerCreature* pPC) throw(Error);  // Áž¿¡ ÃâÀÔÀÌ °¡ŽÉÇÑ°¡? 
	bool leaveCreature(Creature* pCreature) throw(Error);  // Áž¿¡Œ­ ³ª°£ °æ¿ì

	void resetStateTime() { m_StateTime.tv_sec = 0; }

	bool heartbeat() throw (Error);

	void lock() throw(Error) { m_Mutex.lock(); }
    void ulnock() throw(Error) { m_Mutex.unlock(); }

	string toString() const throw(Error);

	// 2007.01.02
	void setMaxPassPlayer( int player ) { m_nMaxPassPlayer = player; }
	int	 getMaxPassPlayer() { return m_nMaxPassPlayer; }

	void MonsterListLoad();
	void initSanttisi();
protected :
	void processCloseState() throw (Error);
	void processTantacleCombatState() throw (Error);
	void processMikllizzCombatState() throw (Error);
	void processSanttisiState() throw (Error);

	void summonMonster() throw (Error);	// žóœºÅÍ ŒÒÈ¯
	void addMikllizzCorpse() throw (Error);	// ¹ÌÅ¬ž®Áî œÃÃŒ Ãß°¡
	void removeMikllizzCorpse() throw (Error);	// ¹ÌÅ¬ž®Áî œÃÃŒ »èÁŠ
	void activeMikllizz() throw (Error);	// ¹ÌÅ¬ž®Áî È°ŒºÈ­
	void kickOutAllPC() throw (Error);		// ÇÃ·¹ÀÌŸî žðµÎ ÂÑŸÆ³»±â
	void setStateClose() throw (Error);		// State Close »óÅÂ·Î žžµçŽÙ.
	void giveKillingReward() throw (Error);		// ž¶œºÅÍ Á×¿ŽÀ» ¶§ ¹ÞŽÂ ºž»ó
	void decreaseSealingStone() throw (Error); // Áž¿¡ ÀÖŽÂ À¯ÀúµéÀÇ ºÀÀÎŒ®À» ÇØÁŠ
	void broadcastClosingMessage( ) throw (Error);
	void broadcastResurrectMikllizMessage( int minute ) throw (Error);


private : 
	Zone*				m_pZone;
	ObjectID_t			m_MikllizzID;
	ObjectID_t			m_MikllizzCorpseID;
	ZoneCoord_t			m_MikllizzX;
	ZoneCoord_t			m_MikllizzY;

	int					m_ClosingMessageStep;
	int					m_ResurrectMikllizzMessageStep;

	int					m_nMaxPassPlayer;	   // ÃÖŽë ÃâÀÔ »ç¿ëÀÚ Œö
	int					m_nPassPlayer;       // PassžŠ ¹ÞÀº »ç¶÷ Œö 

	Timeval				m_TimeOutMessageNextTime;

	int					m_State;             // ÇöÀçÀÇ State
	Timeval				m_StateTime;         // ÇöÀçÀÇ State ÁöŒÓµÉ œÃ°£

	// ¿©±â ºÎÅÍ žÍµçŽÙ
	// Åž¿öŽÂ 4°¡Áö ÅžÀÔÀ» °¡ÁøŽÙ
	int					m_SanttisiType;
	// 1Ãþ ºÎÅÍ 5Ãþ±îÁö
	int					m_SanttisiTowerType;
	int					m_KickZoneID; // ºÎÈ° À§Ä¡
	int					m_SummonStep;
	bool				m_StartCombat; // 

	MonsterTypeList		m_MonterList[STATE_MAX];
//	int					m_CurrentMonstCount;

	int					m_PartyID; // ÇöÀç Çï°¡µç¿¡ µéŸî¿Â ÆÄÆŒID

	mutable Mutex		m_Mutex;				// m_nPassPlayeržŠ È®œÇÈ÷ ÃŒÅ©ÇÒ·Á°í..
};

#endif
