#ifndef __PET_H__
#define __PET_H__

#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Timeval.h"

class PetInfo;

class Pet
{
public:
	Pet(PlayerCreature* pOwner, PetInfo* pPetInfo) : m_pOwner( pOwner ), m_pPetInfo(pPetInfo) { getCurrentTime( m_SkillDelay ); }
	virtual ~Pet() {}

	virtual void heartbeat(const Timeval& currentTime);

	virtual PlayerCreature* getOwner() const { return m_pOwner; }
	PetInfo* getPetInfo() const { return m_pPetInfo; }

	static Pet* makePet( PlayerCreature* pOwner, PetInfo* pPetInfo );

	virtual int getDelay() const = 0;
	virtual bool useSkill() = 0;
	virtual bool canUseSkill() const;

private:
	PlayerCreature* m_pOwner;
	Timeval m_SkillDelay;
	PetInfo* m_pPetInfo;
};

class AttackingPet : public Pet
{
protected:
	AttackingPet( PlayerCreature* pOwner, PetInfo* pPetInfo ) : Pet( pOwner, pPetInfo ) { }

public:
	virtual int getDamage() const = 0;
	virtual int getRange() const = 0;
	bool useSkill();
};

class Degula : public AttackingPet
{
public:
	Degula( PlayerCreature* pOwner, PetInfo* pPetInfo ) : AttackingPet( pOwner, pPetInfo ) { }
	int getDelay() const;
	int getDamage() const;
	int getRange() const;
	
	bool useSkill ();
};
class Centauro : public AttackingPet
{
public:
	Centauro( PlayerCreature* pOwner, PetInfo* pPetInfo ) : AttackingPet( pOwner, pPetInfo ) { }
	int getDelay() const;
	int getDamage() const;
	int getRange() const;

	Slayer* getOwner() const { return dynamic_cast<Slayer*>(Pet::getOwner()); }
};

//20081128 wlzzi - 공격기능을 가진 인큐버스 추가
class Incubus : public AttackingPet
{
public:
	Incubus( PlayerCreature* pOwner, PetInfo* pPetInfo ) : AttackingPet( pOwner, pPetInfo ) { }
	int getDelay() const;
	int getDamage() const;
	int getRange() const;
	
	bool useSkill ();
};
//-

class Stirge : public AttackingPet
{
public:
	Stirge( PlayerCreature* pOwner, PetInfo* pPetInfo ) : AttackingPet( pOwner, pPetInfo ) { }
	int getDelay() const;
	int getDamage() const;
	int getRange() const;

	Vampire* getOwner() const { return dynamic_cast<Vampire*>(Pet::getOwner()); }
};

class Pixie : public Pet
{
public:
	Pixie( PlayerCreature* pOwner, PetInfo* pPetInfo ) : Pet( pOwner, pPetInfo ) { }
	int getDelay() const;
	int getRange() const;
	bool useSkill();

	Ousters* getOwner() const { return dynamic_cast<Ousters*>(Pet::getOwner()); }
};

#endif
