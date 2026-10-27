//----------------------------------------------------------------------
// MAttackCreatureAroundThrowEffectGenerator.h
//----------------------------------------------------------------------
// Creature추적
//----------------------------------------------------------------------

#ifndef	__MATTACKCREATUREAROUNDTHROWEFFECTGENERATOR_H__
#define	__MATTACKCREATUREAROUNDTHROWEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MAttackCreatureAroundThrowEffectGenerator : public MEffectGenerator {
	public :
		MAttackCreatureAroundThrowEffectGenerator()	{}
		~MAttackCreatureAroundThrowEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_ATTACK_CREATURE_AROUND_THROW; }

		// Effect생성
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttackCreatureAroundThrowEffectGenerator	g_AttackCreatureAroundThrowEffectGenerator;

#endif

