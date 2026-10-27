//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFadeOut.cpp
// Written by  : elca
// Description : 
// 군인기술 Sniping 또는 뱀파이어 기술 Invisibility로 인해서
// 현재 점점 희미해져가고 있는(사라지고 있는) 크리쳐에 붙는 이펙트이다.
//////////////////////////////////////////////////////////////////////////////

#include "EffectFadeOut.h"
#include "EffectInvisibility.h"
#include "EffectSnipingMode.h"
#include "EffectSnipingMode2.h"
#include "Slayer.h"
#include "Vampire.h"
#include "DB.h"
#include "ZoneUtil.h"

#include "Gpackets/GCDeleteObject.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectFadeOut::EffectFadeOut(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFadeOut::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFadeOut::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFadeOut::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	ZoneCoord_t x = pCreature->getX();
	ZoneCoord_t y = pCreature->getY();
	pCreature->removeFlag(Effect::EFFECT_CLASS_FADE_OUT);

	if (m_isSniping) 
	{
		if(m_Mode == 2)
		{
			Assert(pCreature->isSlayer());
			Assert(pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2) == false);
			Assert(pCreature->findEffect(Effect::EFFECT_CLASS_SNIPING_MODE_2) == NULL);

			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		
			EffectSnipingMode2* pEffect = new EffectSnipingMode2(pSlayer);
			pEffect->setDeadline(m_Duration);
			pSlayer->addEffect(pEffect);
			pSlayer->setFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2);

			cout<<"is sniping 2222222"<<endl;

			pEffect->affect(pSlayer);

			pSlayer->initAllStatAndSend();
		}
		else
		{
			Assert(pCreature->isSlayer());
			Assert(pCreature->isFlag(Effect::EFFECT_CLASS_SNIPING_MODE) == false);
			Assert(pCreature->findEffect(Effect::EFFECT_CLASS_SNIPING_MODE) == NULL);

			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
		
			EffectSnipingMode* pEffect = new EffectSnipingMode(pSlayer);
			pEffect->setDeadline(m_Duration);
			pSlayer->addEffect(pEffect);
			pSlayer->setFlag(Effect::EFFECT_CLASS_SNIPING_MODE);

			cout<<"is sniping 11111111111"<<endl;
			pEffect->affect(pSlayer);

			pSlayer->initAllStatAndSend();
		}
	} 
	else 
	{
			Assert(pCreature->isVampire() || pCreature->isMonster());
			Assert(pCreature->isFlag(Effect::EFFECT_CLASS_INVISIBILITY) == false);
			Assert(pCreature->findEffect(Effect::EFFECT_CLASS_INVISIBILITY) == NULL);
		
			EffectInvisibility* pEffect = new EffectInvisibility(pCreature);
			pEffect->setNextTime(10);
			pEffect->setDeadline(99999999);
			pEffect->setLevel( pCreature->getLevel() );
			pCreature->addEffect(pEffect);
			pCreature->setFlag( Effect::EFFECT_CLASS_INVISIBILITY );

			cout<<"is INVISIBILITY INVISIBILITYINVISIBILITY"<<endl;
			pEffect->affect( pCreature );
	}


	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFadeOut::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectFadeOut " << "unaffect BEGIN" << endl;

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);
	
	//cout << "EffectFadeOut " << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFadeOut::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectFadeOut::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectFadeOut("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

