#ifndef __RACETYPE_H__
#define __RACETYPE_H__
//////////////////////////////////////////////////////////////////////////////
// Race
//////////////////////////////////////////////////////////////////////////////
enum Race
{
	RACE_SLAYER = 0,
	RACE_VAMPIRE,
	RACE_OUSTERS,

	RACE_MAX
};

#define	FLAG_RACE_SLAYER	1
#define	FLAG_RACE_VAMPIRE	2
#define	FLAG_RACE_OUSTERS	4

typedef unsigned char Race_t;
const unsigned int szRace = sizeof(Race_t);

// 2005.12.23 chyaya
// 통합이전의 출신 서버를 나타내기 위함
enum Origin
{	
	ORIGIN_MOLDAVIA		= 1,
	ORIGIN_TRANSILVANIA = 2,
	ORIGIN_WALAKIA		= 3,
	ORIGIN_HISTRIA		= 4,
	ORIGIN_NEWWOLRD		= 5,

	ORIGIN_MAX
};

#endif
