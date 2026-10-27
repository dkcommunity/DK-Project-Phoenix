// Crypt Manager by Park Jun Yong 2006.04.03
//

#ifndef __CRYPT_MANAGER_HEADER__
#define __CRYPT_MANAGER_HEADER__



#define MAX_CRYPT_TABLE_SIZE	16		// 최대 64개까지
#define MAX_TABLE_DATA_SIZE		256		// 테이블당 데이타 크기
#define MAX_SOCKBUFFER_SIZE		10240	// 소켓버퍼 최대 크기
#define MAX_PACKET_SIZE			512		// 패킷 최대 크기
#define MSG_HEADER_SIZE			10
#define PACKETBUFFER_SIZE		(MAX_SOCKBUFFER_SIZE - MSG_HEADER_SIZE)

#define CRYPTION_KEY			0x35867361
#define CRYPT_TYPE1				1
#define CRYPT_TYPE2				3
#define CRYPT_TYPE3				8

#ifdef __LINUX__
#pragma pack(1)
#else
#pragma pack(push, 1)
//#pragma pack(show)
#endif
typedef union _KEY_TYPE 
{
	struct 
	{
		DWORD key1 : 5;
		DWORD key2 : 5;
		DWORD key3 : 5;
		DWORD key4 : 5;
		DWORD index : 8;
		DWORD type : 4;
	} KeyValue;

	struct
	{
		DWORD wKey1 : 16;
		DWORD wKey2 : 16;
	} wKeyValue;

	DWORD dwKey;
} KEY_TYPE;

typedef union _ORDER_TYPE
{
	struct 
	{
		DWORD order : 28;
		DWORD type : 4;
	} value;
	DWORD dwOrderType;
} ORDER_TYPE;

typedef struct _PACKETDATA
{
	DWORD	m_wMsgSize;
	WORD	m_wMsgType;
	KEY_TYPE m_Key;
	char*	m_Buffer;
}PACKETDATA, *LPPACKETDATA;

typedef union _TABLE_VALUE 
{
	struct 
	{
		DWORD value1 : 8;
		DWORD value2 : 8;
		DWORD value3 : 8;
		DWORD value4 : 8;
	} Value;
	DWORD dwValue;
} TABLE_VALUE;
#ifdef __LINUX__
#pragma pack()
#else
#pragma pack(pop)
//#pragma pack(show)
#endif
/*
Generator: gsl_rng_rand 
This is the BSD rand() generator. Its sequence is 

x_{n+1} = (a x_n + c) mod m

with a = 1103515245, c = 12345 and m = 2^31. The seed specifies the initial value, x_1. The period of this generator is 2^31, and it uses 1 word of storage per generator. 
*/
class CRNGRand
{
	DWORD	m_dwRandom;
public:
	DWORD rand();
	void srand(DWORD seed);
	//////////////////////////////////////////////////////////////////
};

////////////////////////////////////////////////////////////////////////////////////////////////
//	MACRO
////////////////////////////////////////////////////////////////////////////////////////////////
#define LENTH_OF_STATE_VECTOR              (624)  // length of state std::vector

#define M              (397)                 // a period parameter
#define K              (0x9908B0DFU)         // a magic constant
#define hiBit(u)       ((u) & 0x80000000U)   // mask all but highest   bit of u
#define loBit(u)       ((u) & 0x00000001U)   // mask all but lowest    bit of u
#define loBits(u)      ((u) & 0x7FFFFFFFU)   // mask     the highest   bit of u
#define mixBits(u, v)  (hiBit(u)|loBits(v))  // move hi bit of u to hi bit of v

//----------------------------------------------------------------------------------------------
//		Name	:	CRandom
//		Desc	:	
//----------------------------------------------------------------------------------------------
class CRandom
{
private:
	/*	Variable    */

	DWORD   m_state[LENTH_OF_STATE_VECTOR+1];			// m_state std::vector + 1 extra to not violate ANSI C
	DWORD   *m_pNext;									// m_pNext random value is computed from here
	int     m_left;										// can *m_pNext++ this many times before reloading
public:

	CRandom();
	~CRandom();


	//----------------------------------------------------------------------------------------------
	//		Name	:	SeedMT
	//		Desc	:	* 시드 값을 초기화 한다.
	//					- 시드 값이 같으면 같은 난수를 발생 시킨다.
	//----------------------------------------------------------------------------------------------
	void SeedMT( DWORD seed );

	//----------------------------------------------------------------------------------------------
	//		Name	:	ReloadMT
	//		Desc	:	* 시드가 셋팅된 상태로 다시 초기화를 하여 난수를 발생시키고 싶을때 호출
	//					- 예를 들면 게임 시작전에 모두 시드값을 같은 값으로 초기화 하고 게임을 진행했을때,
	//					  로직에 따라서 렌덤 함수를 더 호출한 사람과 들 호출한 사람이 있을수 있다.
	//					  다음 게임을 시작할때 고정 인원이라고 하면 다시 시드값을 줄필요 없이 이 함수만 호출하여 주면 된다.
	//----------------------------------------------------------------------------------------------
	DWORD ReloadMT(void);

	//----------------------------------------------------------------------------------------------
	//		Name	:	RandomMT
	//		Desc	:	* 난수 발생
	//----------------------------------------------------------------------------------------------
	DWORD RandomMT(void);

	//----------------------------------------------------------------------------------------------
	//		Name	:	FrandomMT
	//		Desc	:	* float 형 난수 발생
	//----------------------------------------------------------------------------------------------
	inline float FrandomMT(void);
};


class CCryptManager 
//	: public CSingleton<CCryptManager>
{
	BYTE*	m_pCryptTable;
	int		m_nTableSize;			// 로딩된 테이블 사이즈 
	int		m_nTotalSize;			// 전체 데이타 사이즈
	BYTE*	m_pRandomTable;
	DWORD	m_dwMakeRandKey;		// 
	CRNGRand*	m_Rand1;
	CRandom*	m_Rand2;
protected:
	int		GetRandKey();
	BOOL	LoadTable();
	void	LoadTable2(DWORD seed = 0xabcd9835);
public:
	CCryptManager();
	~CCryptManager();
	void 	EncryptHeader(PACKETDATA* data);
	void	Encrypt(PACKETDATA *pPacket);
	void	Encrypt1(PACKETDATA *pPacket);
	void	Encrypt2(PACKETDATA *pPacket);
	void	Encrypt3(PACKETDATA* data);
	void 	DecryptHeader(PACKETDATA* data);
	int		Decrypt(PACKETDATA *pPacket);
	void	Decrypt1(PACKETDATA *pPacket);
	void	Decrypt2(PACKETDATA *pPacket);
	void	Decrypt3(PACKETDATA *pPacket);
	int		InitCryptManager(bool bMakeRandomTable = false);
	int		MakeRandomKeyTable();
	inline DWORD GetMakeRandKey();
	void StopManager(void);
	void CryptBuf(char* buf , int len, KEY_TYPE key);
	DWORD GetKey();
};

//#ifdef __LINUX__
// external variable declaration
extern CCryptManager * g_pCryptManager;
//#else
//#define CRYPT_MANAGER (CCryptManager::get_instance())
//#endif

#endif
