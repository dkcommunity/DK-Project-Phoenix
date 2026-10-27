//----------------------------------------------------------------------
// MStopZoneRectRandomPixelEffectGenerator.h
//----------------------------------------------------------------------
// Zone의 한 지점에서 3x3 크기만큼 이팩트를 픽셀 단위로 랜덤하게 뿌려준다.
//----------------------------------------------------------------------

#ifndef	__MSTOPZONERECTRANDOMPIXELEFFECTGENERATOR_H__
#define	__MSTOPZONERECTRANDOMPIXELEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MStopZoneRectRandomPixelEffectGenerator : public MEffectGenerator {
	public :
		MStopZoneRectRandomPixelEffectGenerator() {}
		~MStopZoneRectRandomPixelEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_STOP_ZONE_RECT_RANDOM_PIXEL; }

		// Effect생성
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MRippleZoneEffectGenerator	g_StopZoneRectRandomPixelEffectGenerator;

#endif

