
#ifndef	__MMOVINGEFFECT_H__
#define	__MMOVINGEFFECT_H__

#include "MEffect.h"

class MMovingEffect : public MEffect {
	public :
		MMovingEffect(BYTE bltType);
		~MMovingEffect();

		virtual EFFECT_TYPE		GetEffectType()	const	{ return EFFECT_MOVING; }

		//--------------------------------------------------------
		// 한 번의 Update에 호출될 함수..
		//--------------------------------------------------------
		virtual bool	Update();
};

#endif

