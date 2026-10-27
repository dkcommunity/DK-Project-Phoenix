/*
	G-Audio header definitions

	written by Odin/gOds, Marsh'99
*/


#ifndef	GAUDIO
#include <dsound.h>

#define	GAUDIO
#define	GAUDIO_VERSION					short
#define	GAUDIO_CLASSVersion				0x010

#define	GAUDIO_OUTPUTMODE				long
#define	GAUDIO_FLAG						long
#define	GAUDIO_STRSRC					long
#define	GAUDIO_FREQ						long
#define	GAUDIO_FLAG						long

#define GAUDIO_STRSRC_DISK				0x1
#define GAUDIO_STRSRC_MEMORY			0x2

#define	GAUDIO_SAMPFLG_STOP				0x1
#define	GAUDIO_SAMPFLG_LOOP				0x2

#define	GAUDIO_OK						true
#define	GAUDIO_ERROR					false

#define	GAUDIO_INITDIRECTSOUND			0x1

#define	GAUDIO_STREAMSTATUS_NOTHING		0x0
#define	GAUDIO_STREAMSTATUS_PLAYING		0x1
#define	GAUDIO_STREAMSTATUS_FINISHED	0x2

#define	GAUDIO_OUTPUTMODE_WAVEMAPPER	0x1
#define	GAUDIO_OUTPUTMODE_DIRECTSOUND	0x2

#define	GAUDIO_DSPRIMBUF_MAXIMUMFREQ	0x1

struct	st_GAUDIO_GeneralInformation
{
		long	DLLVersion;						// Version of current G-Audio DLL
		long	nbrStreamPlayed;					// Number of stream actually playing
};

struct	st_GAUDIO_UserInformation
{
		LPDIRECTSOUNDBUFFER	lpDSBuffer;				// Active DirectSound Buffer
		GAUDIO_FLAG			SampleStatus;
};

class	GAUDIO_MP3STREAM
{
		void						*Reserved;

	public:

		GAUDIO_OUTPUTMODE			OutputMode;
		GAUDIO_STRSRC				Source;
		long						Lenght;
		char						*Name;
		void						*Pointer;
		GAUDIO_FLAG					SampleFlag;

		st_GAUDIO_UserInformation	UserInformation;

		bool						Init();
		bool						Play();
		bool						Reset();
		bool						Stop();
		bool						Erase();
		bool						SetPrimaryDSBuffer(GAUDIO_FLAG);

};

bool	GAUDIO_Init(GAUDIO_FLAG);
bool	GAUDIO_SetPointerToDirectSound(LPDIRECTSOUND);
void	GAUDIO_ShutDown();

st_GAUDIO_GeneralInformation *GAUDIO_GetGeneralInformations();

#endif
