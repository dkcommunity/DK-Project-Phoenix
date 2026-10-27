//// kim sung yong Premium Card ////
 
#ifndef __EFFECT_PREMIUM_SERVICE__
#define __EFFECT_PREMIUM_SERVICE__
 
#include "Effect.h"
#include "EffectLoader.h"
 
//////////////////////////////////////////////////////////////////////////////
// class EffectPremiumService
//////////////////////////////////////////////////////////////////////////////
 
class EffectPremiumService : public Effect
{
public:
    EffectPremiumService(Creature* pCreature) throw(Error);
 
public:
    EffectClass getEffectClass() const throw() { return EFFECT_CLASS_JOYMAX_PREMIUM_SERVICE; }
 
    void affect() throw(Error) { }
    void affect(Creature* pCreature) throw(Error);
 
    void unaffect() throw(Error);
    void unaffect(Creature* pCreature) throw(Error);
 
    string toString() const throw();
};
 
//////////////////////////////////////////////////////////////////////////////
// class EffectPremiumServiceLoader
//////////////////////////////////////////////////////////////////////////////
 
class EffectPremiumServiceLoader : public EffectLoader
{
public:
    virtual Effect::EffectClass getEffectClass() const throw() { return Effect::EFFECT_CLASS_JOYMAX_PREMIUM_SERVICE; }
    virtual string getEffectClassName() const throw() { return "EffectPremiumService"; }
 
public:
     virtual void load(Creature* pCreature) throw(Error);
 
};
 
extern EffectPremiumServiceLoader* g_pEffectPremiumServiceLoader;
 
#endif // __EFFECT_PREMIUM_SERVICE__

