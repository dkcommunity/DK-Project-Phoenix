//----------------------------------------------------------------------
// AddonDef.h
//----------------------------------------------------------------------
// Addon.cfpk에 들어 있는 순서를 의미한다.
//----------------------------------------------------------------------

#ifndef	__ADDONDEF_H__
#define	__ADDONDEF_H__


//----------------------------------------------------------------------
// Addon종류
//----------------------------------------------------------------------
/*
enum ADDONID
{
	ADDONID_HAIR1,		// 머리1
	ADDONID_HAIR2,		// 머리2
	ADDONID_HAIR3,		// 머리3
	ADDONID_HELM1,		// 모자1
	ADDONID_HELM2,		// 모자2	
	ADDONID_COAT1,		// 상의1
	ADDONID_COAT2,		// 상의2		
	ADDONID_TROUSER1,	// 하의1
	ADDONID_TROUSER2,	// 하의2	
	ADDONID_SWORD,		// 검	
	ADDONID_BLADE,		// 도	
	ADDONID_GUN_TR,		// 총1 - 저격용총(TR)
	ADDONID_GUN_SG,		// 총2 - 샷건(SG)	
	ADDONID_GUN_AR,		// 총3 - 소총(AR)
	ADDONID_GUN_SMG,		// 총4 - 자동소총(SMG)	
	ADDONID_SHIELD,		// 방패	
	ADDONID_CROSS,		// 십자가
	ADDONID_MOTORCYCLE,	// 오토바이	
	MAX_ADDONID
};
*/
	

#ifdef __EXPO_CLIENT__
	//----------------------------------------------------------------------
	// 남자 Slayer
	//----------------------------------------------------------------------
	enum ADDONID_MALE
	{
		ADDONID_HELM1_MALE,			// 모자1
		ADDONID_COAT1_MALE,			// 상의1
		ADDONID_TROUSER1_MALE,		// 하의1
		ADDONID_GUN_SMG_MALE,			// 총4 - 자동소총(SMG)	
		ADDONID_BLADE_MALE,		// 도
		ADDONID_SHIELD1_MALE,		// 방패
		ADDONID_GUN_SG_MALE,			// 총2 - 샷건(SG)
		ADDONID_SWORD_MALE,		// 검	
		ADDONID_GUN_AR_MALE,			// 총3 - 소총(AR)
		ADDONID_HAIR1_MALE,			// 머리1
		ADDONID_HAIR2_MALE,			// 머리2
		ADDONID_HAIR3_MALE,			// 머리3
		ADDONID_HELM2_MALE,			// 모자2
		ADDONID_COAT2_MALE,			// 상의2
		ADDONID_TROUSER2_MALE,		// 하의2
		ADDONID_CROSS_MALE,			// 십자가
		ADDONID_GUN_TR_MALE,			// 총1 - 저격용총(TR)		
		ADDONID_MOTORCYCLE_MALE,	// 오토바이
		MAX_ADDONID_MALE
	};

	//----------------------------------------------------------------------
	// 여자 Slayer
	//----------------------------------------------------------------------
	enum ADDONID_FEMALE
	{
		ADDONID_HELM1_FEMALE,			// 모자1
		ADDONID_COAT1_FEMALE,			// 상의1
		ADDONID_TROUSER1_FEMALE,		// 하의1
		ADDONID_GUN_SMG_FEMALE,			// 총4 - 자동소총(SMG)	
		ADDONID_BLADE_FEMALE,		// 도
		ADDONID_SHIELD_FEMALE,		// 방패
		ADDONID_GUN_SG_FEMALE,			// 총2 - 샷건(SG)
		ADDONID_SWORD_FEMALE,		// 검	
		ADDONID_GUN_AR_FEMALE,			// 총3 - 소총(AR)
		ADDONID_HAIR1_FEMALE,			// 머리1
		ADDONID_HAIR2_FEMALE,			// 머리2
		ADDONID_HAIR3_FEMALE,			// 머리3
		ADDONID_HELM2_FEMALE,			// 모자2
		ADDONID_COAT2_FEMALE,			// 상의2
		ADDONID_TROUSER2_FEMALE,		// 하의2
		ADDONID_CROSS_FEMALE,			// 십자가
		ADDONID_GUN_TR_FEMALE,			// 총1 - 저격용총(TR)		
		ADDONID_MOTORCYCLE_FEMALE,	// 오토바이
		MAX_ADDONID_FEMALE
	};

#else


	enum ADDONID_SLAYER
	{
		//----------------------------------------------------------------------
		// Slayer
		//----------------------------------------------------------------------
		ADDONID_SWORD,			// 검
		ADDONID_BLADE,			// 도

		ADDONID_GUN_SR,
		ADDONID_GUN_AR, 
		ADDONID_GUN_SG,
		ADDONID_GUN_SMG,		// 자동소총(SMG)
		
		ADDONID_CROSS,
		ADDONID_MACE,

		ADDONID_SHIELD1,
		ADDONID_SHIELD2,		// 드래곤방패

		ADDONID_HAIR1,
		ADDONID_HAIR2,
		ADDONID_HAIR3,

		ADDONID_MOTORCYCLE,

		ADDONID_HELM1,
		ADDONID_HELM2,
		ADDONID_HELM3,

		ADDONID_COAT1,
		ADDONID_TROUSER1,

		ADDONID_COAT2,
		ADDONID_TROUSER2,

		ADDONID_COAT3,
		ADDONID_TROUSER3,

		ADDONID_COAT4,
		ADDONID_TROUSER4, 
		
#if __CONTENTS(__FAST_TRANSFORTER)
		ADDONID_WING_BIKE,
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		ADDONID_HOVER_VEHICLE,
		ADDONID_AIR_MOTO,
		ADDONID_APQ_VEHICLE,
#endif //__SECOND_TRANSFORTER
	  // ADDONID_APQ_VEHICLE,
		MAX_ADDONID_SLAYER
	};
	

	enum ADDONID_VAMPIRE
	{
		ADDONID_VAMPIRE_WEAPON_MALE,
		ADDONID_VAMPIRE_WEAPON_FEMALE,
		ADDONID_VAMPIRE_COAT1_MALE,
		ADDONID_VAMPIRE_COAT1_FEMALE,
		ADDONID_VAMPIRE_COAT2_MALE,
		ADDONID_VAMPIRE_COAT2_FEMALE,
		ADDONID_VAMPIRE_COAT3_MALE,
		ADDONID_VAMPIRE_COAT3_FEMALE,
		ADDONID_VAMPIRE_COAT4_MALE,
		ADDONID_VAMPIRE_COAT4_FEMALE,

		MAX_ADDONID_VAMPIRE,
	};

#endif


#endif