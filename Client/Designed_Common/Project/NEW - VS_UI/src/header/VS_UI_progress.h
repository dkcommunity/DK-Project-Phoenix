/*-----------------------------------------------------------------------------

	VS_UI_progress.h

	Progress class.

	2000.6.8. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __VS_UI_PROGRESS_H__
#define __VS_UI_PROGRESS_H__

// FAZ 4 (2026-04-29 Phase B): ContentsFilter.h zorunlu — bu header'da
// __CONTENTS(__TIPOJYU_CASTLE) guard'i var (line 99-104) ve struct
// layout'u etkiliyor. ContentsFilter.h dahil edilmezse vs_ui_lib
// tarafinda macro tanimsiz, conditional field kapali, Client.exe ile
// layout farki olusuyordu (m_Draw_Num garbage, m_pC_progress yanlis
// offset → DeleteNew heap corruption).
#include "ContentsFilter.h"
// FAZ 4 (2026-05-03): CrashTrace silent macro — header'a direkt include.
#include "crash_trace_tool.h"

#include <cstddef>  // FAZ 4 forensic: offsetof icin
#include "VS_UI_Base.h"
#include "VS_UI_util.h"
#include "VS_UI_extradialog.h"	//VS_UI_DESC�� �ҷ����� extradialog�� �������� �ֱ׷���-_-a
/*
enum PROGRESS_SPK_INDEX
{
	TUTORIAL_HELIPORT,
	DUNGEON_EXIT,
	SLAYER_PROGRESS_1,
	SLAYER_PROGRESS_2,	
	VAMPIRE_PROGRESS_1,
	VAMPIRE_PROGRESS_2,
	VAMPIRE_PROGRESS_3,
	BAR,
};
*/

//----------------------------------------------------------------------------
// Class Progress
//----------------------------------------------------------------------------
class C_VS_UI_PROGRESS : public Window, public C_VS_UI_DESC
{
private:
//	enum 
//	{ 
//		TOTAL_BAR_LINE = 413,//248, 
//		BAR_GAP_X = 192, //49,//44,
//		BAR_GAP_Y = 557, //558,//144,
//		BAR_COLORKEY = 0xFF,

		//VAMPIRE_RAND_MAX = 6, // progress_vampire_rand.spk + original vampire back image
//	};

	enum PROGRESS_RESOLUTION
	{
		RESOLUTION_800_600,
		RESOLUTION_1024_768,
		RESOLUTION_1280_720,
		RESOLUTION_1280_960,
		RESOLUTION_1280_1024,
	};

	enum PROGRESS_TYPE
	{
		TYPE_MONSTER_INFO,
		TYPE_EVENT,	
		TYPE_PPL,
		TYPE_MAX
	};

	enum PROGRESS_MONSTER_INFO
	{
		BACK,
		CHAR_NAME_ALPHA,
		CHAR_NAME_BACK,
		PROGRESS_BAR,
		PROGRESS_ICON,
	};

	enum PROGRESS_EVENT
	{
		EVENT_OUSTERS,
		EVENT_REVENGE_OF_THE_MASTERS,
		EVENT_REVENGE_OF_THE_MASTERS_2,
		EVENT_SLAYER_2005,
		EVENT_VAMPIRE_2005,
		EVENT_OUSTERS_2005,
		EVENT_MAXX = 8,//modify by hays
	};

	enum PROGRESS_PPL
	{
		PPL_IVY_CLUB,
	};

	enum PROGRESS_CI
	{
		CI_DARKEDEN,
		CI_IVY_CLUB,
		CI_MAX
	};

#if __CONTENTS(__TIPOJYU_CASTLE)
	enum PROGRESS_EVENT_TIPOJYU
	{
		PROGREES_LIFE_IMAGE		= 6,			//���� �̹���	(�̹����� �׸��� �κп��� *2���ش�.) �׷��Ƿ� �ε����� �ݰ�
		PROGREES_HADES_IMAGE	= 7,			//���� �̹���
	};

	WORD				m_wZoneID;
#endif // __TIPOJYU_CASTLE
	
	int					m_progress_type;

	C_SPRITE_PACK *		m_pC_event_progress;
	C_SPRITE_PACK *		m_pC_ppl;
	C_SPRITE_PACK *		m_pC_ci;

	// 2026-09-03 Renewal loading ekrani (Reborn paritesi) —
	// data\ui\spk\progresseventrenewal klasorundeki standalone spk'lar.
	// m_pC_renewal_image: rastgele secilen buyuk gorsel (>=512KB dosyalar);
	// bar zemini 00013 (670x22), bar dolgusu 00014 (663x16). Yukleme
	// basarisiz olursa hepsi NULL kalir ve legacy ProgressEvent.spk yolu
	// aynen calisir (fallback — silme yok, kural 4.1).
	C_SPRITE_PACK *		m_pC_renewal_image;
	C_SPRITE_PACK *		m_pC_renewal_bar_bg;
	C_SPRITE_PACK *		m_pC_renewal_bar_fill;
	int					m_renewal_base;		// secili gorselin dosya numarasi (log/reshuffle icin)

	CSpritePack *		m_pC_progress;
	CSpritePack	*		m_pC_character;

	WORD				m_percent;
	int					m_Draw_Num;

	std::string			m_world_name;
	std::string			m_server_name;
	int					m_server_status;

public:
	C_VS_UI_PROGRESS();
	~C_VS_UI_PROGRESS();

	void	InitMonsterInfoProgress();
	void	InitEventProgress();
	void	InitPPLProgress();

	void	WindowEventReceiver(id_t event);
	bool	IsPixel(int _x, int _y);

	void	Start();
	void	Finish();
	void	Show();
	void	SetProgress(WORD cur_val, WORD max_val);
	void	AcquireDisappear() {}
	// Zone hop reuse: ProgressEvent SPK yeniden yuklenmeden rastgele gorsel sec.
	void	ReshuffleEventImage();
	// 2026-09-03 Renewal loading — bar spk'larini yukle + rastgele gorsel sec.
	// true donerse renewal aktif; false ise legacy ProgressEvent yolu kullanilir.
	bool	InitRenewalProgress();
	// Rastgele bir renewal gorseli (yeniden) yukler; reshuffle'da da cagrilir.
	bool	PickRenewalImage();

#if __CONTENTS(__TIPOJYU_CASTLE)
	void	SetZoneID(WORD wZoneID);
	WORD	GetZoneID() { return m_wZoneID;}
	BOOL	LoadingSleep();
#endif // __TIPOJYU_CASTLE
	void	SetServerName(const char *world_name, const char *server_name, int status)
	{
		if(world_name)
			m_world_name = world_name;
		else
			m_world_name = "";
		
		if(server_name)
			m_server_name = server_name;
		else 
			m_server_name = "";
		
		m_server_status = status; 
	}

	const char*	GetServerName() { return m_server_name.c_str(); }
	const char*	GetWorldName() { return m_world_name.c_str(); }
	int GetServerStatus() { return m_server_status; }

	// FAZ 4 forensic (2026-05-01): Client.exe ile vs_ui_lib arasi ABI/struct
	// layout mismatch var (m_Draw_Num garbage, EndProgress destructor crash).
	// Bu helper SIZE ve OFFSET'leri her iki taraftan da log'lar; sayilar farkli
	// gelirse define asimetrisi kanitlanir ve hangi member kaydigi gorulur.
	// Header'da static inline → her translation unit kendi sizeof'unu hesaplar.
	// Class icinde tanimli oldugu icin private member offsetof'una erisebilir.
	static inline void DebugLogLayout(const char* tag) {
		// FAZ 4 (2026-05-03): CrashTrace artık macro (silent default) — extern decl
		// kaldırıldı, header include edildi. crash_trace_tool.h zaten PCH'de var.
		CT_Done("PROGRESS_LAYOUT %s sizeof_progress=%u sizeof_window=%u sizeof_desc=%u sizeof_string=%u off_pcprogress=%u off_drawnum=%u off_worldname=%u",
			tag,
			(unsigned)sizeof(C_VS_UI_PROGRESS),
			(unsigned)sizeof(Window),
			(unsigned)sizeof(C_VS_UI_DESC),
			(unsigned)sizeof(std::string),
			(unsigned)offsetof(C_VS_UI_PROGRESS, m_pC_progress),
			(unsigned)offsetof(C_VS_UI_PROGRESS, m_Draw_Num),
			(unsigned)offsetof(C_VS_UI_PROGRESS, m_world_name));
	}

	//
	// !gC_vs_ui.StartProgress() �Ŀ� �����Ѵ�.
	//
//	void	SetProgressStyle(PROGRESS_SPK_INDEX style);
};

#endif