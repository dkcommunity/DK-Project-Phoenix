#ifndef __VIEW_EFFECT_H__
#define __VIEW_EFFECT_H__

#include "Types.h"

// 기본 복사생성자를 사용한다. 반드시 STL에 값으로 넣을 수 있는 형태여야 한다. Viewable 클래스 참조.
class ViewEffect
{
	EffectID_t m_EffectType;
	Level_t m_Level;

public:
	ViewEffect(EffectID_t eType, Level_t level) : m_EffectType(eType), m_Level(level) { }

	EffectID_t getEffectType() const { return m_EffectType; }
	Level_t getLevel() const { return m_Level; }
};

#endif

