#include "Alignment.h"

Alignment getAlignmentType(Alignment_t Alignment)
{
	if (Alignment <= -10000)
	{
		return LESS_EVIL;
	}
	if (Alignment >= -10000 && Alignment < -7500) 
	{
		return LESS_EVIL;
	} 
	else if (Alignment >= -7500 && Alignment < -2500) 
	{
		return EVIL;
	} 
	else if (Alignment >= -2500 && Alignment < 2500) 
	{
		return NEUTRAL;
	}
	else if (Alignment >= 2500 && Alignment < 7500) 
	{
		return GOOD;
	} 
	else if (Alignment >= 7500 && Alignment <= 10000) 
	{
		return MORE_GOOD;
	} 
	else 
	{
		return MORE_GOOD;
	}
}
