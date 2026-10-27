#ifndef __ITEM_POLICIES_H__
#define __ITEM_POLICIES_H__

#include "Types.h"
#include <list>

template <typename T>
class OneValuePolicy
{
public:
	// 2026-05-13 BUG FIX: m_Value() value-initialize edilmeli.
	// Eski default ctor m_Value'yu init etmiyordu; primitive T (char/int) icin
	// garbage. EnchantLevel_t=char Mace shop'ta create edildiginde -19 gibi
	// rastgele degerler DB'ye yaziliyordu (MaceObject.EnchantLevel UNSIGNED
	// SMALLINT NOT NULL -> "Out of range" reddi). T() POD'lar icin 0/false/empty
	// garanti eder, class T icin default ctor. Tum OneValuePolicy<T>
	// kullanimlari (HasEnchantLevel/Stackable/HasDurability) defansif init.
	OneValuePolicy() : m_Value() { }
	OneValuePolicy(T init) : m_Value(init) { }

	bool	hasValue() const { return true; }
	T		getValue() const { return m_Value; }
	void	setValue(T value) { m_Value = value; }

private:
	T		m_Value;
};

template <typename T, T initValue>
class NoValuePolicy
{
public:
	bool	hasValue() const { return false; }
	T		getValue() const { return initValue; }
	void	setValue(T value) { }
};

typedef NoValuePolicy<ItemNum_t, 1> NoStack;
typedef OneValuePolicy<ItemNum_t> Stackable;

// Stack Policies
/*class NoStack
{
public:
	bool isStackable() const { return false; }
	ItemNum_t getNum() const { return 1; }
	void setNum(ItemNum_t num) { }
};

class Stackable
{
public:
	Stackable() : m_Num(1) { }
	bool isStackable() const { return true; }
	ItemNum_t getNum() const { return m_Num; }
	void setNum(ItemNum_t num) { m_Num = num; }

private:
	ItemNum_t m_Num;
};*/

typedef NoValuePolicy<Durability_t, 1> NoDurability;
typedef OneValuePolicy<Durability_t> HasDurability;

// Durability Policies
/*class NoDurability
{
public:
	bool hasDurability() const { return false; }
	Durability_t getDurability() const { return 1; }
	void setDurability(Durability_t dur) { }
};

class HasDurability
{
public:
	bool hasDurability() const { return true; }
	Durability_t getDurability() const { return m_Durability; }
	void setDurability(Durability_t dur) { m_Durability = dur; }

private:
	Durability_t m_Durability;
};*/

// Option Policies
class NoOption
{
public:
	bool						hasOptionType() const throw() { return false; }
	int							getOptionTypeSize() const throw() { return 0; }
	int							getRandomOptionType() const throw() { return 0; }
	const list<OptionType_t>&	getOptionTypeList() const throw() { static list<OptionType_t> nullList; return nullList; }
	OptionType_t				getFirstOptionType() const throw() { return 0; }
	void						removeOptionType(OptionType_t OptionType) throw() { }
	void						changeOptionType(OptionType_t currentOptionType, OptionType_t newOptionType) throw() { }
	void						addOptionType(OptionType_t OptionType) throw() { }
	void						setOptionType(const list<OptionType_t>& OptionType) throw() { }

	bool						hasGTwoOptionType() const throw() { return false; }
	int							getGTwoOptionTypeSize() const throw() { return 0; }
	int							getGTwoRandomOptionType() const throw() { return 0; }
	const list<OptionType_t>&	getGTwoOptionTypeList() const throw() { static list<OptionType_t> nullList; return nullList; }
	OptionType_t				getGTwoFirstOptionType() const throw() { return 0; }
	void						removeGTwoOptionType(OptionType_t OptionType) throw() { }
	void						changeGTwoOptionType(OptionType_t currentOptionType, OptionType_t newOptionType) throw() { }
	void						addGTwoOptionType(OptionType_t OptionType) throw() { }
	void						setGTwoOptionType(const list<OptionType_t>& OptionType) throw() { }

#ifdef VERSION_THIRD_ENCHANT_1
	bool						hasThirdOptionType() const throw() { return false; }
	int							getThirdOptionTypeSize() const throw() { return 0; }
	const list<OptionType_t>&	getThirdOptionTypeList() const throw() { static list<OptionType_t> nullList; return nullList; }
	void						setThirdOptionType(const list<OptionType_t>& OptionType) throw() { }
	BYTE						getThirdEnchantType() const throw() { return 0; }
	void						setThirdEnchantType(const BYTE thirdEnchantType) throw() { }
	void						clearThirdOption() throw() { }
#endif
};

class HasOption
{
public:
	bool						hasOptionType() const throw() { return !m_OptionType.empty(); }
	int							getOptionTypeSize() const throw() { return m_OptionType.size(); }
	int							getRandomOptionType() const throw()
	{
		if (m_OptionType.empty())
			return 0;
		int pos = rand()%m_OptionType.size();
		list<OptionType_t>::const_iterator itr = m_OptionType.begin();
		for (int i=0; i<pos; i++) itr++;
		return *itr;
	}

	const	list<OptionType_t>& getOptionTypeList() const throw() { return m_OptionType; }
	OptionType_t				getFirstOptionType() const throw() { if (m_OptionType.empty()) return 0; return m_OptionType.front(); }
	void						removeOptionType(OptionType_t OptionType) throw() { list<OptionType_t>::iterator itr = find(m_OptionType.begin(), m_OptionType.end(), OptionType); if (itr!=m_OptionType.end()) m_OptionType.erase(itr); }
	void						changeOptionType(OptionType_t currentOptionType, OptionType_t newOptionType) throw() { list<OptionType_t>::iterator itr = find(m_OptionType.begin(), m_OptionType.end(), currentOptionType); if (itr!=m_OptionType.end()) *itr=newOptionType; }
	void						addOptionType(OptionType_t OptionType) throw() { m_OptionType.push_back(OptionType); }
	void						setOptionType(const list<OptionType_t>& OptionType) throw() { m_OptionType = OptionType; }


/////////////////gai zao///////////////////////////////////////////////////////////////////
	bool						hasGTwoOptionType() const throw() { return !m_GTwoOptionType.empty(); }
	int						getGTwoOptionTypeSize() const throw() { return m_GTwoOptionType.size(); }

	const	list<OptionType_t>& getGTwoOptionTypeList() const throw() { return m_GTwoOptionType; }
	void						removeGTwoOptionType(OptionType_t OptionType) throw() { list<OptionType_t>::iterator itr = find(m_GTwoOptionType.begin(), m_GTwoOptionType.end(), OptionType); if (itr!=m_GTwoOptionType.end()) m_GTwoOptionType.erase(itr); }
	void						changeGTwoOptionType(OptionType_t currentOptionType, OptionType_t newOptionType) throw() { list<OptionType_t>::iterator itr = find(m_GTwoOptionType.begin(), m_GTwoOptionType.end(), currentOptionType); if (itr!=m_GTwoOptionType.end()) *itr=newOptionType; }
	void						addGTwoOptionType(OptionType_t OptionType) throw() { m_GTwoOptionType.push_back(OptionType); }
	void						setGTwoOptionType(const list<OptionType_t>& OptionType) throw() { m_GTwoOptionType = OptionType; }

#ifdef VERSION_THIRD_ENCHANT_1
	bool						hasThirdOptionType() const throw() { return !m_ThirdOptionType.empty(); }
	int						getThirdOptionTypeSize() const throw() { return m_ThirdOptionType.size(); }
	
	const	list<OptionType_t>& getThirdOptionTypeList() const throw() { return m_ThirdOptionType; }
	void						removeThirdOptionType(OptionType_t OptionType) throw() { list<OptionType_t>::iterator itr = find(m_ThirdOptionType.begin(), m_ThirdOptionType.end(), OptionType); if (itr!=m_ThirdOptionType.end()) m_ThirdOptionType.erase(itr); }
	void						changeThirdOptionType(OptionType_t currentOptionType, OptionType_t newOptionType) throw() { list<OptionType_t>::iterator itr = find(m_ThirdOptionType.begin(), m_ThirdOptionType.end(), currentOptionType); if (itr!=m_ThirdOptionType.end()) *itr=newOptionType; }
	void						addThirdOptionType(OptionType_t OptionType) throw() { m_ThirdOptionType.push_back(OptionType); }
	void						setThirdOptionType(const list<OptionType_t>& OptionType) throw() { m_ThirdOptionType = OptionType; }
	void						clearThirdOption() throw() { m_ThirdOptionType.clear(); }
	BYTE						getThirdEnchantType() const throw() { return m_ThirdEnchantType; }
	void						setThirdEnchantType(const BYTE thirdEnchantType) throw() { m_ThirdEnchantType = thirdEnchantType; }
#endif

private:
	list<OptionType_t>   m_OptionType;
	list<OptionType_t>   m_GTwoOptionType;
#ifdef VERSION_THIRD_ENCHANT_1
	list<OptionType_t>   m_ThirdOptionType;
	BYTE				 m_ThirdEnchantType;
#endif
};

// Grade Policies
class NoGrade : public NoValuePolicy<Grade_t, -1>
{
public:
//	Grade_t	getGrade() const { return -1; }
//	void	setGrade(Grade_t Grade) { }

public:
	// �������� �Ӽ��� ��ġ�� ����
	int		getDurabilityOffset() const { return 0; }
	int		getDamageOffset() const { return 0; }
	int		getCriticalOffset() const { return 0; }
	int		getDefenseOffset() const { return 0; }
	int		getProtectionOffset() const { return 0; }
	Luck_t	getLuck() const { return 0; }
};

class HasGrade : public OneValuePolicy<Grade_t>
{
public:
	HasGrade() : OneValuePolicy<Grade_t>(4) { }

//	Grade_t	getGrade() const { return m_Grade; }
//	void	setGrade(Grade_t Grade) { m_Grade = Grade; }

public:
	// �������� �Ӽ��� ��ġ�� ����
	int		getDurabilityOffset() const { return 0; }
	int		getDamageOffset() const { return 0; }
	int		getCriticalOffset() const { return 0; }
	int		getDefenseOffset() const { return 0; }
	int		getProtectionOffset() const { return 0; }
	Luck_t	getLuck() const { return 0; }

private:
	Grade_t	m_Grade;
};

class WeaponGrade : public HasGrade
{
public:
	// �������� �Ӽ��� ��ġ�� ����
	int		getDurabilityOffset() const { return (getValue()-4)*1000; }
	int		getDamageOffset() const { return (getValue()-4); }
	int		getCriticalOffset() const { return (getValue()-4)*2; }
};

template <int DefensePitch, int DefenseGrade, int ProtectionPitch, int ProtectionGrade, int DurabilityPitch>
class ArmorGrade : public HasGrade
{
public:
	// �������� �Ӽ��� ��ġ�� ����
	int		getDurabilityOffset() const { return (getValue()-4)*DurabilityPitch; }
	int		getDefenseOffset() const { return (getValue()-4)/DefenseGrade*DefensePitch; }
	int		getProtectionOffset() const { return (getValue()-4)/ProtectionGrade*ProtectionPitch; }
};

typedef ArmorGrade<2,1,1,1,1000> ClothGrade;
typedef ArmorGrade<1,1,1,2,500> GroceryGrade;

class AccessoryGrade : public HasGrade
{
public:
	// �������� �Ӽ��� ��ġ�� ����
	int		getDurabilityOffset() const { return (getValue()-4)*1000; }
	Luck_t	getLuck() const { return (getValue()-4); }
};

// Attacking Stats Policies
class NoAttacking
{
public:
	BYTE		getBulletCount() const throw() { return 0; }
	void		setBulletCount(BYTE bulletCount) throw() { }

	bool		isSilverWeapon() const throw() { return false; }
	Silver_t	getSilver() const throw() { return 0; }
	void		setSilver(Silver_t amount) throw() { }

	bool		isGun() const throw() { return false; }

	Damage_t	getBonusDamage() const throw() { return 0; }
	void		setBonusDamage(Damage_t BonusDamage) throw() { }
};

class Weapon
{
public:
	Weapon() : m_BonusDamage(0) { }

	BYTE		getBulletCount() const throw() { return 0; }
	void		setBulletCount(BYTE bulletCount) throw() { }

	bool		isGun() const throw() { return false; }

	bool		isSilverWeapon() const throw() { return false; }
	Silver_t	getSilver() const throw() { return 0; }
	void		setSilver(Silver_t amount) throw() { }

	Damage_t	getBonusDamage() const throw() { return m_BonusDamage; }
	void		setBonusDamage(Damage_t BonusDamage) throw() { m_BonusDamage = BonusDamage; }

private:
	Damage_t	m_BonusDamage;
};

class SlayerWeapon : public Weapon
{
public:
	SlayerWeapon() : m_Silver(0) { }

	bool		isSilverWeapon() const throw() { return true; }
	Silver_t	getSilver() const throw() { return m_Silver; }
	void		setSilver(Silver_t amount) throw() { m_Silver = amount; }

private:
	Silver_t	m_Silver;
};

class SlayerGun : public SlayerWeapon
{
public:
	SlayerGun() : m_Bullet(0) { }

	BYTE	getBulletCount() const throw() { return m_Bullet; }
	void	setBulletCount(BYTE bulletCount) throw() { m_Bullet = bulletCount; }

	bool	isGun() const throw() { return true; }

private:
	BYTE	m_Bullet;
};

typedef OneValuePolicy<EnchantLevel_t> HasEnchantLevel;
typedef NoValuePolicy<EnchantLevel_t, 0> NoEnchantLevel;

#endif
