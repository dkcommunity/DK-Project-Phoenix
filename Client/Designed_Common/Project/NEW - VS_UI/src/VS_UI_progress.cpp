#include "client_PCH.h"
#include "crash_trace_tool.h"

#include "VS_UI_progress.h"
#include "VS_UI_filepath.h"
#include "VS_UI_description.h"
#include "UserInformation.h"
#include "VS_UI.h"
#include <time.h>

#include "MGameStringTable.H"
#include "VS_UI_GPS.H"

#include <vector>	// 2026-09-03 Renewal loading — dosya kesif listesi

#define PROGRESS_MAX 25

//-----------------------------------------------------------------------------
// 2026-09-03 Renewal loading ekrani (Reborn paritesi)
//
// data\ui\spk\progresseventrenewal klasorundeki standalone spk'lar:
// - Buyuk dosyalar (>=512KB) tam ekran loading gorselleri (cozunurluk blogu
//   YOK — Reborn gibi rastgele secilir, ekrana ortalanir).
// - 00013.spk = bar zemini (670x22), 00014.spk = bar dolgusu (663x16).
// Kesif Reborn DiscoverProgressEventLoadingSpriteBases ile ayni kurali izler:
// *.spk + 5 haneli numerik govde + boyut >= 512KB. Liste process omrunde bir
// kez cikarilir (klasor icerigi runtime'da degismez).
//-----------------------------------------------------------------------------
namespace
{
	const int  kRenewalBarBgBase   = 13;	// Reborn kProgressEventLoadingBarBackgroundSprite
	const int  kRenewalBarFillBase = 14;	// Reborn kProgressEventLoadingBarFillSprite

	void Phoenix_RenewalSpkPath(char* pBuf, size_t bufLen, int base)
	{
		sprintf_s(pBuf, bufLen, SPK_PROGRESS_EVENT_RENEWAL_DIR "%05d.spk", base);
	}

	const std::vector<int>& Phoenix_RenewalImageBases()
	{
		static std::vector<int> s_bases;
		static bool s_bScanned = false;
		if (s_bScanned)
			return s_bases;
		s_bScanned = true;

		WIN32_FIND_DATAA fd;
		HANDLE hFind = FindFirstFileA(SPK_PROGRESS_EVENT_RENEWAL_DIR "*.spk", &fd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			CT_Done("RenewalProgress: klasor bulunamadi/bos: %s", SPK_PROGRESS_EVENT_RENEWAL_DIR);
			return s_bases;
		}
		do
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			// Reborn kurali: loading gorseli icin min 512KB (bar/kucuk spk'lar elenir)
			if (fd.nFileSizeHigh == 0 && fd.nFileSizeLow < 512u * 1024u)
				continue;
			// 5 haneli numerik govde ("00015.spk")
			const char* pName = fd.cFileName;
			int value = 0;
			bool bNumeric = true;
			int digits = 0;
			for (; pName[digits] != '\0' && pName[digits] != '.'; ++digits)
			{
				if (pName[digits] < '0' || pName[digits] > '9') { bNumeric = false; break; }
				value = value * 10 + (pName[digits] - '0');
			}
			if (!bNumeric || digits == 0 || digits > 5)
				continue;
			s_bases.push_back(value);
		} while (FindNextFileA(hFind, &fd));
		FindClose(hFind);

		CT_Done("RenewalProgress: %d adet loading gorseli bulundu", (int)s_bases.size());
		return s_bases;
	}
}


//-----------------------------------------------------------------------------
// C_VS_UI_PROGRESS
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_PROGRESS::C_VS_UI_PROGRESS()
{
	g_RegisterWindow(this);

	AttrTopmost(true);

	srand(time(0));

	m_pC_progress = NULL;
	m_pC_event_progress = NULL;
	m_pC_character = NULL;
	m_pC_ppl = NULL;
	m_pC_ci = NULL;

	// 2026-09-03 Renewal loading — InitEventProgress icinde denenir
	m_pC_renewal_image = NULL;
	m_pC_renewal_bar_bg = NULL;
	m_pC_renewal_bar_fill = NULL;
	m_renewal_base = -1;

	// �ݸ����� ���ӳ� ���� ����
//	if(g_pUserInformation->IsNetmarble ||
//	   g_pUserInformation->IsTestServer)
//	{
		InitEventProgress();

		/* ���� �ε�ȭ���� ���� �׸�~ chyaya
		if(rand() % 2)
		{
			InitEventProgress();
		}
		else
		{
			InitMonsterInfoProgress();
		}
		*/
//	}
//	else
//	{
//		if(rand() % 2)
//		{
//			InitPPLProgress();
//		}
//		else
//		{
//			InitEventProgress();

			/* ���� �ε�ȭ���� ���� �׸�~ chyaya
			if(rand() % 2)
			{
				InitEventProgress();
			}
			else
			{
				InitMonsterInfoProgress();
			}
			*/

//			m_pC_ci = new C_SPRITE_PACK(SPK_PROGRESS_CI);
//		}
//	}
	

	m_world_name = "";
	m_server_name = "";
	m_server_status = C_VS_UI_SERVER_SELECT::STATUS_DOWN;

#if __CONTENTS(__TIPOJYU_CASTLE)
	m_wZoneID		= 0;
#endif //  __TIPOJYU_CASTLE
}

//-----------------------------------------------------------------------------
// ~C_VS_UI_PROGRESS
//
// 
//-----------------------------------------------------------------------------
C_VS_UI_PROGRESS::~C_VS_UI_PROGRESS()
{
	g_UnregisterWindow(this);

	if(m_pC_event_progress != NULL)
		DeleteNew(m_pC_event_progress);

	if(m_pC_progress != NULL)
		DeleteNew(m_pC_progress);
	
	if(m_pC_character != NULL)
		DeleteNew(m_pC_character);

	if(m_pC_ci != NULL)
		DeleteNew(m_pC_ci);

	if(m_pC_ppl != NULL)
		DeleteNew(m_pC_ppl);

	// 2026-09-03 Renewal loading kaynaklari
	if(m_pC_renewal_image != NULL)
		DeleteNew(m_pC_renewal_image);

	if(m_pC_renewal_bar_bg != NULL)
		DeleteNew(m_pC_renewal_bar_bg);

	if(m_pC_renewal_bar_fill != NULL)
		DeleteNew(m_pC_renewal_bar_fill);

//	DeleteNewArray(m_p_bar_buf);
}

//-----------------------------------------------------------------------------
// C_VS_UI_PROGRESS::SetProgressStyle
//
// 
//-----------------------------------------------------------------------------
//void	C_VS_UI_PROGRESS::SetProgressStyle(PROGRESS_SPK_INDEX style)
//{
//	if (style >= BAR)
//	{
//		style = SLAYER_PROGRESS_1;
//	}

//	m_progress_back_image = style; 

	/*
	if (m_progress_back_image == VAMPIRE_PROGRESS)
	{
		int index = rand()%VAMPIRE_RAND_MAX;
		if (index > 0)
		{
			DeleteNew(m_pC_progress);
			m_pC_progress = new C_SPRITE_PACK(SPK_PROGRESS_VAMPIRE_RAND);

			m_progress_back_image = index-1;
		}
	}
	*/
//}

//-----------------------------------------------------------------------------
// InitMonsterInfoProgress
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_PROGRESS::InitMonsterInfoProgress()
{
	m_progress_type = TYPE_MONSTER_INFO;

	static bool Check[PROGRESS_MAX];
	static int	CheckNum = 0;
	int num, i;
	
	for(int i = 0; i < PROGRESS_MAX; i++)
		Check[i] = false;
	
	CheckNum = PROGRESS_MAX;
	
	if(g_pUserInformation->GoreLevel == false)
	{
		// ƾ���� �ϵ��ڵ�
		Check[4] = true;
		Check[9] = true;
		Check[11] = true;
		Check[13] = true;
		Check[14] = true;
		Check[15] = true;
		Check[21] = true;
		Check[22] = true;
		Check[23] = true;
		
		CheckNum -= 9;
	}
	
	num = rand() % CheckNum;
	
	for(int i = 0; i <= num; i++)
	{
		if(Check[i] == true)num++;
	}
	
	Check[num] = true;
	CheckNum--;
	
	
	m_pC_progress = new CSpritePack;

	bool bHighResolution = g_pUserInformation->iResolution_x > 800;	
	
	if(!bHighResolution)
		m_pC_progress->LoadFromFile(SPK_PROGRESS);
	else
		m_pC_progress->LoadFromFile(SPK_PROGRESS_1024);
	
	m_pC_character = new CSpritePack;
	
	m_pC_character->Init(2);
	m_pC_character->LoadFromFileData(0, num*2, SPK_PROGRESS_CHARACTER, SPKI_PROGRESS_CHARACTER);
	m_pC_character->LoadFromFileData(1, num*2+1, SPK_PROGRESS_CHARACTER, SPKI_PROGRESS_CHARACTER);
	
	m_pack_file.SetRAR(RPK_PROGRESS, RPK_PASSWORD);
	
	PrintInfo pi, title_pi;
	COLORREF color, title_color;
	
	title_pi = gpC_base->m_char_value_pi;
	title_color = RGB(178,140,121);
	pi = gpC_base->m_char_name_pi;
	color = RGB(200,200,200);
	
	static char *Monster[PROGRESS_MAX] =
	{
		(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_TNDEAD].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_ARKHAN].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_ESTROIDER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_GOLEMER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_DARKSCREAMER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_DEADBODY].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_MODERAS].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_VANDALIZER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_DIRTYSTRIDER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_HELLWIZARD].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_TNSOUL].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_IRONTEETH].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_REDEYE].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_CRIMSONSLAUGTHER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_HELLGUARDIAN].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_SOLDIER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_RIPPER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_BIGFANG].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_LORDCHAOS].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_CHAOSGUARDIAN].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_HOBBLE].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_CHAOSNIGHT].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_WIDOWS].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_KID].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_NAME_SHADOWWING].GetString(),
	};
	
	static char *MonsterFilename[PROGRESS_MAX] =
	{
		(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_TNDEAD].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_ARKHAN].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_ESTROIDER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_GOLEMER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_DARKSCREAMER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_DEADBODY].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_MODERAS].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_VANDALIZER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_DIRTYSTRIDER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_HELLWIZARD].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_TNSOUL].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_IRONTEETH].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_REDEYE].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_CRIMSONSLAUGTHER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_HELLGUARDIAN].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_SOLDIER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_RIPPER].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_BIGFANG].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_LORDCHAOS].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_CHAOSGUARDIAN].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_HOBBLE].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_CHAOSNIGHT].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_WIDOWS].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_KID].GetString(),
			(*g_pGameStringTable)[UI_STRING_MESSAGE_MONSTER_ENAME_SHADOWWING].GetString(),
	};
	
	std::string filename = MonsterFilename[num];
	filename += ".txt";
	LoadDesc(filename.c_str(), 43, 50, false);

	int convx = (g_pUserInformation->iResolution_x - 800) / 2;
	int convy = (g_pUserInformation->iResolution_y - 600) / 2;
	
	SetDesc(convx + 400, convy + 150, color, pi);
	SetDescTitle(convx + 400, convy + 120, title_color, title_pi);
	SetDescTitle(Monster[num]);
	Set(g_pUserInformation->iResolution_x/2 - (*m_pC_progress)[BACK].GetWidth()/2 ,
		g_pUserInformation->iResolution_y/2 - (*m_pC_progress)[BACK].GetHeight()/2 ,
		(*m_pC_progress)[BACK].GetWidth(), (*m_pC_progress)[BACK].GetHeight());

}



//-----------------------------------------------------------------------------
// InitMonsterInfoProgress
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_PROGRESS::InitEventProgress()
{
	m_progress_type = TYPE_EVENT;

	m_Draw_Num = 0;

	// 2026-09-03 Renewal loading — once yeni progresseventrenewal klasoru
	// denenir (Reborn paritesi: rastgele gorsel + bar 13/14). Basarili ise
	// legacy ProgressEvent.spk hic yuklenmez (bosuna ~MB'lik pack yok).
	// Basarisiz ise asagidaki legacy yol aynen calisir (fallback).
	if (InitRenewalProgress())
	{
		Set(0, 0, g_pUserInformation->iResolution_x, g_pUserInformation->iResolution_y);
		return;
	}

	int iEventStartID = 0 ; int iEventCount = 0; 

// 	if(bHighResolution)
// 		m_pC_event_progress = new C_SPRITE_PACK(SPK_PROGRESS_EVENT_1024);
// 	else
// 		m_pC_event_progress = new C_SPRITE_PACK(SPK_PROGRESS_EVENT);
// 
// 	do{
// 		m_Draw_Num = (rand() % (m_pC_event_progress->GetSize() - 4)) / 2; // -4�� Ƽ���� �ε� �̹��� ����.
// 	}while(m_Draw_Num == EVENT_REVENGE_OF_THE_MASTERS ||
// 			m_Draw_Num == EVENT_REVENGE_OF_THE_MASTERS_2);		// �� �����°� �׳� ����� �ߴ�.

	m_pC_event_progress = new C_SPRITE_PACK(SPK_PROGRESS_EVENT);

	do 
	{
		m_Draw_Num = (rand() % (EVENT_MAXX*2 - 4));	// �� �ε� �̹��� ���� - Ƽ���� �ε� �̹��� ����.
	} while(m_Draw_Num % 2 == 1);

	bool bHighResolution = g_pUserInformation->iResolution_x > 800;

	if(bHighResolution)
	{
		if(g_pUserInformation->iResolution_x <= 1024)
			m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1024_768;
		else // if(g_pUserInformation->iResolution_x > 1024)
		{
			if(g_pUserInformation->iResolution_y < 960)
				m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1280_720;
			else if(g_pUserInformation->iResolution_y < 1024)
				m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1280_960;
			else
				m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1280_1024;
		}
	}

// 	//while(	!g_pUserInformation->GoreLevel   && 
// 	//		( m_Event_Num == 1 || m_Event_Num == 2 ) ); // ƾ������ ����ϴµ� �ǰ� ������ �ٽ� �̴´�.
	
	Set(0, 0, m_pC_event_progress->GetWidth(), m_pC_event_progress->GetHeight());
}

//-----------------------------------------------------------------------------
// InitRenewalProgress — 2026-09-03 Renewal loading ekrani (Reborn paritesi)
//
// Bar spk'larini (00013 zemin, 00014 dolgu) yukler ve rastgele bir loading
// gorseli secer. Herhangi biri basarisiz olursa tum renewal kaynaklari
// birakilir ve false doner (caller legacy yola duser). Standalone spk'lar
// standart pack formatindadir (count=1) — CSpritePack::LoadFromFile okur.
//-----------------------------------------------------------------------------
bool C_VS_UI_PROGRESS::InitRenewalProgress()
{
	if (Phoenix_RenewalImageBases().empty())
		return false;

	char path[MAX_PATH];

	if (m_pC_renewal_bar_bg == NULL)
	{
		Phoenix_RenewalSpkPath(path, sizeof(path), kRenewalBarBgBase);
		m_pC_renewal_bar_bg = new C_SPRITE_PACK(path);
		if (m_pC_renewal_bar_bg->GetSize() <= 0)
		{
			CT_Done("RenewalProgress: bar zemini yuklenemedi: %s", path);
			DeleteNew(m_pC_renewal_bar_bg);
			m_pC_renewal_bar_bg = NULL;
			return false;
		}
	}

	if (m_pC_renewal_bar_fill == NULL)
	{
		Phoenix_RenewalSpkPath(path, sizeof(path), kRenewalBarFillBase);
		m_pC_renewal_bar_fill = new C_SPRITE_PACK(path);
		if (m_pC_renewal_bar_fill->GetSize() <= 0)
		{
			CT_Done("RenewalProgress: bar dolgusu yuklenemedi: %s", path);
			DeleteNew(m_pC_renewal_bar_fill);
			m_pC_renewal_bar_fill = NULL;
			return false;
		}
	}

	return PickRenewalImage();
}

//-----------------------------------------------------------------------------
// PickRenewalImage — rastgele bir renewal gorseli (yeniden) yukler.
//
// Zone hop'ta da cagrilir; onceki gorsel birakilip yenisi yuklenir (gorsel
// basina ~2.5-4MB pack, hop basina tek dosya okuma — kabul edilebilir).
// Mumkunse bir onceki gorselden farkli bir base secilir.
//-----------------------------------------------------------------------------
bool C_VS_UI_PROGRESS::PickRenewalImage()
{
	const std::vector<int>& bases = Phoenix_RenewalImageBases();
	if (bases.empty())
		return false;

	int base = bases[rand() % bases.size()];
	if ((int)bases.size() > 1)
	{
		// Ayni gorseli ust uste gosterme (en fazla birkac deneme yeter)
		for (int retry = 0; retry < 8 && base == m_renewal_base; ++retry)
			base = bases[rand() % bases.size()];
	}

	char path[MAX_PATH];
	Phoenix_RenewalSpkPath(path, sizeof(path), base);

	if (m_pC_renewal_image != NULL)
	{
		DeleteNew(m_pC_renewal_image);
		m_pC_renewal_image = NULL;
	}

	m_pC_renewal_image = new C_SPRITE_PACK(path);
	if (m_pC_renewal_image->GetSize() <= 0)
	{
		CT_Done("RenewalProgress: gorsel yuklenemedi: %s", path);
		DeleteNew(m_pC_renewal_image);
		m_pC_renewal_image = NULL;
		return false;
	}

	m_renewal_base = base;
	CT_Done("RenewalProgress: gorsel secildi base=%05d %dx%d", base,
		m_pC_renewal_image->GetWidth(0), m_pC_renewal_image->GetHeight(0));
	return true;
}

void C_VS_UI_PROGRESS::ReshuffleEventImage()
{
	// 2026-09-03 Renewal loading — aktifse yeni rastgele gorsel sec, legacy
	// index hesabina girme. Secim basarisiz olursa eski gorsel yerinde kalir.
	if (m_pC_renewal_image != NULL)
	{
		PickRenewalImage();
		Set(0, 0, g_pUserInformation->iResolution_x, g_pUserInformation->iResolution_y);
		return;
	}

	if (m_progress_type != TYPE_EVENT || m_pC_event_progress == NULL)
		return;

	do
	{
		m_Draw_Num = (rand() % (EVENT_MAXX * 2 - 4));
	} while (m_Draw_Num % 2 == 1);

	bool bHighResolution = g_pUserInformation->iResolution_x > 800;
	if (bHighResolution)
	{
		if (g_pUserInformation->iResolution_x <= 1024)
			m_Draw_Num = m_Draw_Num + EVENT_MAXX * 2 * RESOLUTION_1024_768;
		else
		{
			if (g_pUserInformation->iResolution_y < 960)
				m_Draw_Num = m_Draw_Num + EVENT_MAXX * 2 * RESOLUTION_1280_720;
			else if (g_pUserInformation->iResolution_y < 1024)
				m_Draw_Num = m_Draw_Num + EVENT_MAXX * 2 * RESOLUTION_1280_960;
			else
				m_Draw_Num = m_Draw_Num + EVENT_MAXX * 2 * RESOLUTION_1280_1024;
		}
	}

	Set(0, 0, m_pC_event_progress->GetWidth(), m_pC_event_progress->GetHeight());
}

#if __CONTENTS(__TIPOJYU_CASTLE)
void	C_VS_UI_PROGRESS::SetZoneID(WORD wZoneID)
{
	m_wZoneID = wZoneID;

	bool bHighResolution = g_pUserInformation->iResolution_x > 800;
	
	switch(m_wZoneID)
	{
	case 1801:	case 1803:
		{
			m_Draw_Num	= PROGREES_LIFE_IMAGE;
			
			if(bHighResolution)
			{
				if(g_pUserInformation->iResolution_x <= 1024)
					m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1024_768;
				else // if(g_pUserInformation->iResolution_x > 1024)
				{
					if(g_pUserInformation->iResolution_y < 960)
						m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1280_720;
					else if(g_pUserInformation->iResolution_y < 1024)
						m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1280_960;
					else
						m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1280_1024;
				}
			}
			break;
		}
		
	
	case 1802:	case 1804:
		{
			m_Draw_Num	= PROGREES_HADES_IMAGE;

			if(bHighResolution)
			{
				if(g_pUserInformation->iResolution_x <= 1024)
					m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1024_768;
				else // if(g_pUserInformation->iResolution_x > 1024)
				{
					if(g_pUserInformation->iResolution_y < 960)
						m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1280_720;
					else if(g_pUserInformation->iResolution_y < 1024)
						m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1280_960;
					else
						m_Draw_Num = m_Draw_Num + EVENT_MAXX*2*RESOLUTION_1280_1024;
				}
			}
			break;
		}
	}
}


BOOL	C_VS_UI_PROGRESS::LoadingSleep()
{
	switch(m_wZoneID)
	{
		case 1801:
		case 1803:
		case 1802:
		case 1804:
			return TRUE;
		default:
			return FALSE;
	}
}

#endif // __TIPOJYU_CASTLE

//-----------------------------------------------------------------------------
// InitPPLProgress
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_PROGRESS::InitPPLProgress()
{
	m_progress_type = TYPE_PPL;

	bool bHighResolution = g_pUserInformation->iResolution_x > 800;
	
	if(bHighResolution)
		m_pC_ppl = new C_SPRITE_PACK(SPK_PROGRESS_PPL_1024);
	else
		m_pC_ppl = new C_SPRITE_PACK(SPK_PROGRESS_PPL);
	
	m_Draw_Num = rand() % (m_pC_ppl->GetSize() / 2);

	Set(0, 0, m_pC_ppl->GetWidth(), m_pC_ppl->GetHeight());
}


//-----------------------------------------------------------------------------
// IsPixel
//
// 
//-----------------------------------------------------------------------------
bool C_VS_UI_PROGRESS::IsPixel(int _x, int _y)
{
//	return m_pC_progress->IsPixel(SCR2WIN_X(_x), SCR2WIN_Y(_y), SLAYER_PROGRESS_1);
	return true;//(*m_pC_progress)[BACK].IsColorPixel(SCR2WIN_X(_x), SCR2WIN_Y(_y));
}

//-----------------------------------------------------------------------------
// WindowEventReceiver
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_PROGRESS::WindowEventReceiver(id_t event)
{

}

//-----------------------------------------------------------------------------
// Start
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_PROGRESS::Start()
{
	PI_Processor::Start();

	gpC_window_manager->AppearWindow(this);

	m_world_name = "";
	m_server_name = "";
	m_server_status = C_VS_UI_SERVER_SELECT::STATUS_DOWN;


//	m_cur_line = 0;
	m_percent = 0;
//	m_progress_back_image = SLAYER_PROGRESS_1;

	g_descriptor_manager.Unset();
}

//-----------------------------------------------------------------------------
// Finish
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_PROGRESS::Finish()
{
	PI_Processor::Finish();

	gpC_window_manager->DisappearWindow(this);
}

//-----------------------------------------------------------------------------
// SetProgress
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_PROGRESS::SetProgress(WORD cur_val, WORD max_val)
{
	if (max_val == 0 || cur_val > max_val)
		return;

	if (max_val > 0)
	{
		float f = cur_val / (float)max_val;

		m_percent = (WORD)(f*100);

//		f = (m_percent / 100.) * TOTAL_BAR_LINE;

//		WORD n = f;
//		float t = n;
//		float r = f - t;
//		if (r > 0.)
//			n += 1; // �ݿø�.
//
//		if (m_percent < 100 && n >= TOTAL_BAR_LINE)
//			n = TOTAL_BAR_LINE - 1;
//
//		m_cur_line = n;
	}
}

//-----------------------------------------------------------------------------
// DrawBar (Based on Vampire Energy interface)
//
//
//-----------------------------------------------------------------------------
/*
void C_VS_UI_PROGRESS::DrawBar()
{
	if (m_cur_line < 1 || m_cur_line > TOTAL_BAR_LINE)
		return;

	if (gpC_base->m_p_DDSurface_back->Lock())
	{
		S_SURFACEINFO	src_surface, dest_surface;
		S_RECT			src_rect, dest_rect;
		SetSurfaceInfo(&dest_surface, gpC_base->m_p_DDSurface_back->GetDDSD());
		SetSurfaceInfo(src_surface, m_p_bar_buf, m_bar_width, m_bar_height);
		SetRect(src_rect, 0, 0, m_cur_line, m_bar_height);
		SetRect(dest_rect, x+BAR_GAP_X, y+BAR_GAP_Y, m_cur_line, m_bar_height);

		CkBltz(&dest_surface, &src_surface, &dest_rect, &src_rect, BAR_COLORKEY);

		gpC_base->m_p_DDSurface_back->Unlock();
	}
}
*/

//-----------------------------------------------------------------------------
// Show
//
// 
//-----------------------------------------------------------------------------
void C_VS_UI_PROGRESS::Show()
{
	CT_Done("WnSh: C_VS_UI_PROGRESS::Show ENTER pthis=%p", (void*)this);
#if __CONTENTS(__MOVE_ZONE)
	if(gpC_mouse_pointer->IsMoveZoneEffect())
	{
		string namecc  = "���̵� ���Դϴ�.";
		
		for(int i=-1; i<=1; i++)
		for(int j=-1; j<=1; j++)
		{
			g_PrintColorStr(g_pUserInformation->iResolution_x / 2 - g_GetStringWidth(namecc.c_str(), gpC_base->m_dialog_menu_pi.hfont)/2+i,
			(int)(g_pUserInformation->iResolution_y * 0.70f)+j, namecc.c_str(), gpC_base->m_desc_menu_pi, RGB(165,0,0));
		}
		
		g_PrintColorStr(g_pUserInformation->iResolution_x / 2 - g_GetStringWidth(namecc.c_str(), gpC_base->m_dialog_menu_pi.hfont)/2,
			(int)(g_pUserInformation->iResolution_y * 0.70f), namecc.c_str(), gpC_base->m_desc_menu_pi, RGB(0,0,0));

		return;
	}
#endif //__MOVE_ZONE

	gpC_base->m_p_DDSurface_back->FillSurface(0);
	
	POINT p = {0, 0};
	

	if(gpC_base->m_p_DDSurface_back->Lock())
	{
		if(m_progress_type == TYPE_PPL)
		{
			Rect rect;

			m_pC_ppl->BltLocked(0, 0, (m_Draw_Num*2));

			rect.Set(0, m_pC_ppl->GetHeight(m_Draw_Num)*(100-m_percent)/100,
				m_pC_ppl->GetWidth(m_Draw_Num), m_pC_ppl->GetHeight(m_Draw_Num)*(m_percent)/100);

			m_pC_ppl->BltLockedClip(0, 0, rect, (m_Draw_Num*2)+1);
		}
		else
		{
			if(m_progress_type == TYPE_EVENT)
			{
				Rect rect;

				// 2026-09-03 Renewal loading — Reborn paritesi: rastgele
				// gorsel ekrana ortalanir (buyukse kenarlar BltSprite clip
				// guard'lariyla kirpilir, kucukse siyah bant kalir; legacy
				// CPU yolunda arbitrary scale yok). Bar alt-ortada: Reborn
				// referansi 1280x960 kanvasta zemin y=890 (alttan 70px),
				// dolgu zemine gore +4,+3 offset ve genislik=percent orani.
				// Renewal aktifken m_pC_event_progress NULL'dur; asagidaki
				// legacy blok kendi NULL check'iyle no-op olur (fallback
				// yapisi korunur, kural 4.1).
				if (m_pC_renewal_image != NULL)
				{
					const int resx = g_pUserInformation->iResolution_x;
					const int resy = g_pUserInformation->iResolution_y;

					m_pC_renewal_image->BltLocked(
						(resx - m_pC_renewal_image->GetWidth(0)) / 2,
						(resy - m_pC_renewal_image->GetHeight(0)) / 2, 0);

					if (m_pC_renewal_bar_bg != NULL && m_pC_renewal_bar_fill != NULL)
					{
						const int bx = (resx - m_pC_renewal_bar_bg->GetWidth(0)) / 2;
						const int by = resy - 70;
						m_pC_renewal_bar_bg->BltLocked(bx, by, 0);

						const int fw = m_pC_renewal_bar_fill->GetWidth(0) * m_percent / 100;
						if (fw > 0)
						{
							rect.Set(0, 0, fw, m_pC_renewal_bar_fill->GetHeight(0));
							m_pC_renewal_bar_fill->BltLockedClip(bx + 4, by + 3, rect, 0);
						}
					}
				}

				// FAZ 4 (2026-04-28): C_VS_UI_PROGRESS struct layout mismatch — m_Draw_Num garbage gibi
				// (346009168 görüldü, pointer-sized). m_pC_event_progress da garbage olabilir.
				// Defansif: pointer + size + index check, herhangi biri suspicious ise Show'u atla.
				CT_Done("DIAG: PROGRESS::Show TYPE_EVENT enter, this=%p, event_progress=%p, m_Draw_Num=%d, m_percent=%d",
					(void*)this, (void*)m_pC_event_progress, m_Draw_Num, (int)m_percent);
				if (m_pC_event_progress == NULL) {
					CT_Done("DIAG: PROGRESS::Show event_progress=NULL, skipping");
				} else {
					int spkSize = 0;
					try { spkSize = m_pC_event_progress->GetSize(); }
					catch (...) { spkSize = -1; }
					CT_Done("DIAG: PROGRESS::Show event_progress GetSize()=%d", spkSize);
					if (spkSize <= 0 || spkSize > 1000) {
						CT_Done("DIAG: PROGRESS::Show suspicious spkSize=%d, skipping draw (likely garbage event_progress pointer)", spkSize);
					} else {
						int safeDrawNum = m_Draw_Num;
						if (safeDrawNum < 0 || safeDrawNum >= spkSize - 1) {
							CT_Done("DIAG: PROGRESS::Show m_Draw_Num=%d OOB (spkSize=%d) -> clamping to 0", m_Draw_Num, spkSize);
							safeDrawNum = 0;
						}
						int safePercent = (int)m_percent;
						if (safePercent < 0 || safePercent > 100) {
							CT_Done("DIAG: PROGRESS::Show m_percent=%d OOB -> clamping to 0", safePercent);
							safePercent = 0;
						}
						const bool suspectState = (m_Draw_Num < 0 || m_Draw_Num > 1000);
						CT_Done("DIAG: PROGRESS::Show pre BltLocked safeDrawNum=%d suspect=%d", safeDrawNum, (int)suspectState);
						// FAZ 4 (2026-05-03): suspect state'te BltLocked(0,0,safeDrawNum=0)
						// ghost window'dan loading background sprite'ını ekrana basıyordu
						// ("loading sprite asılı kaldı" semptomu). Suspect ise tüm BltLocked'i
						// de skip et — defense in depth (EndProgress'in DisappearWindow fix'i
						// asıl çözüm, bu fallback).
						if (suspectState) {
							CT_Done("DIAG: PROGRESS::Show suspect state -> skip BltLocked (avoid ghost loading sprite)");
						} else {
							m_pC_event_progress->BltLocked(0, 0, safeDrawNum);
							CT_Done("DIAG: PROGRESS::Show post BltLocked");
						}

						if (suspectState) {
							CT_Done("DIAG: PROGRESS::Show suspect state -> skip BltLockedClip (corrupt class state)");
						} else {
							rect.Set(0, m_pC_event_progress->GetHeight(safeDrawNum)*(100-safePercent)/100,
								m_pC_event_progress->GetWidth(safeDrawNum), m_pC_event_progress->GetHeight(safeDrawNum)*(safePercent)/100);
							CT_Done("DIAG: PROGRESS::Show pre BltLockedClip");
							m_pC_event_progress->BltLockedClip(0, 0, rect, safeDrawNum+1);
							CT_Done("DIAG: PROGRESS::Show post BltLockedClip");
						}
					}
				}
				CT_Done("DIAG: PROGRESS::Show TYPE_EVENT branch end");
				
//				Sleep(500); // ���� �׽�Ʈ�� ���� ��� ����� �ڵ� ���� �Ҷ��� �ݵ�� ó�� �ȵǰ� �ؾ� �ϴ� �ڵ� �̴�. 2008.09.09 - shootkj
				
#if __CONTENTS(__TIPOJYU_CASTLE)
				if(LoadingSleep())
					Sleep(250);
#endif
			}
			else
			{
				int convx = (g_pUserInformation->iResolution_x - 800) / 2;
				int convy = (g_pUserInformation->iResolution_y - 600) / 2;

				//BACK���
				gpC_base->m_p_DDSurface_back->BltSprite(&p, &(*m_pC_progress)[BACK]);

				p.x = convx + 200 - (*m_pC_character)[0].GetWidth()/2;
				p.y = convy + 300 - (*m_pC_character)[0].GetHeight()/2;
			
				//ĳ���� ���
				gpC_base->m_p_DDSurface_back->s_Value1 = m_percent*32/100; 	// 0�̸� ���� gray, 32�̸� ��������
				
				gpC_base->m_p_DDSurface_back->SetEffect( CSpriteSurface::EFFECT_GRAY_SCALE );//EFFECT_NET);//
				
				
				Rect rect;
				RECT rt;
				S_SURFACEINFO surface_info;
				SetSurfaceInfo(&surface_info, gpC_base->m_p_DDSurface_back->GetDDSD());
				
				rect.Set(0, 0, (*m_pC_character)[0].GetWidth(), (*m_pC_character)[0].GetHeight()*(100-m_percent)/100);
				rt.left = max(-p.x, rect.x);
				rt.top = max(-p.y, rect.y);

				rt.right = min(rect.x+rect.w, g_pUserInformation->iResolution_x-p.x);
				rt.bottom = min(rect.y+rect.h, g_pUserInformation->iResolution_y-p.y);
				
				if(rt.left < rt.right && rt.top < rt.bottom)
				{
					WORD * p_dest = (WORD *)surface_info.p_surface+p.x+rt.left;
					p_dest = (WORD *)((BYTE *)p_dest+(p.y+rt.top)*surface_info.pitch);
					
					(*m_pC_character)[0].BltEffectClipHeight(p_dest, surface_info.pitch, &rt);
				}
				
				SetRect((RECT *)&rect, 0, (*m_pC_character)[0].GetHeight()*(100-m_percent)/100, (*m_pC_character)[0].GetWidth(), (*m_pC_character)[0].GetHeight() - (*m_pC_character)[0].GetHeight()*(100-m_percent)/100);
				rt.left = max(-p.x, rect.x);
				rt.top = max(-p.y, rect.y);

				rt.right = min(rect.x+rect.w, g_pUserInformation->iResolution_x-p.x);
				rt.bottom = min(rect.y+rect.h, g_pUserInformation->iResolution_y-p.y);
								
				if(rt.left < rt.right && rt.top < rt.bottom)
				{
					
					WORD * p_dest = (WORD *)surface_info.p_surface+p.x+rt.left;
					p_dest = (WORD *)((BYTE *)p_dest+(p.y+rt.top)*surface_info.pitch);
					
					(*m_pC_character)[0].BltClipHeight(p_dest, surface_info.pitch, &rt);
				}

				p.x = convx + 200 - (*m_pC_progress)[PROGRESS_BAR].GetWidth()/2;
				p.y += (*m_pC_character)[0].GetHeight()*(100-m_percent)/100-(*m_pC_progress)[PROGRESS_BAR].GetHeight()/2;
				
				
				gpC_base->m_p_DDSurface_back->SetEffect( CSpriteSurface::EFFECT_SCREEN );
				gpC_base->m_p_DDSurface_back->BltSpriteEffect(&p, &(*m_pC_progress)[PROGRESS_BAR]);
				
				p.x = g_pUserInformation->iResolution_x - (*m_pC_progress)[CHAR_NAME_BACK].GetWidth()-10;
				p.y = g_pUserInformation->iResolution_y - (*m_pC_progress)[CHAR_NAME_BACK].GetHeight()-45;
				
				//ĳ�����̸����(�ϴ�ū��)
				
				gpC_base->m_p_DDSurface_back->SetEffect( CSpriteSurface::EFFECT_GRAY_SCALE_VARIOUS );
				gpC_base->m_p_DDSurface_back->BltSpriteEffect(&p, &(*m_pC_progress)[CHAR_NAME_BACK]);
				
				p.x = g_pUserInformation->iResolution_x - (*m_pC_progress)[CHAR_NAME_BACK].GetWidth()/2 -(*m_pC_character)[1].GetWidth()/2-10;
				p.y = g_pUserInformation->iResolution_y - (*m_pC_progress)[CHAR_NAME_BACK].GetHeight()/2 -(*m_pC_character)[1].GetHeight()/2-25;
				
				gpC_base->m_p_DDSurface_back->BltSprite(&p, &(*m_pC_character)[1]);
			}

			// ������ ��� PPL ȸ�� CI�� ����Ѵ�.
			if(m_pC_ci)
			{
				const int ci_gap	= 15;
				const int ci_y		= h - m_pC_ci->GetHeight() - ci_gap;

				m_pC_ci->BltLocked(ci_gap, ci_y, CI_IVY_CLUB);
				m_pC_ci->BltLocked(w - m_pC_ci->GetWidth(CI_DARKEDEN) - ci_gap, ci_y, CI_DARKEDEN);
			}
			CT_Done("DIAG: PROGRESS::Show post inner branches, pre Unlock");
		}

		gpC_base->m_p_DDSurface_back->Unlock();
		CT_Done("DIAG: PROGRESS::Show post Unlock");
	}

	//���� �����̸� ǥ��
	COLORREF focus_color;
	focus_color = RGB_BLACK;
	// 2026-09-03 Renewal loading — gorsel ustune beyaz yazi
	if(m_pC_event_progress != NULL || m_pC_renewal_image != NULL)
		focus_color = RGB_WHITE;
	CT_Done("DIAG: PROGRESS::Show pre server name string build");

	// FAZ 4 (2026-04-28): m_world_name / m_server_name std::string'ler de struct layout mismatch'ten dolayı garbage olabilir.
	// .c_str() çağrısı internal data pointer'ını deref eder; corrupt ise crash. m_Draw_Num bu sebeple > spkSize ise skip.
	if (m_Draw_Num < 0 || m_Draw_Num > 1000) {
		CT_Done("DIAG: PROGRESS::Show m_Draw_Num=%d still suspicious -> skip server name display (likely corrupt strings)", m_Draw_Num);
		return;
	}

	std::string name = GetWorldName();
	CT_Done("DIAG: PROGRESS::Show post GetWorldName, len=%u", (unsigned)name.size());
	name += " - ";
	name += GetServerName();
	CT_Done("DIAG: PROGRESS::Show post GetServerName, len=%u", (unsigned)name.size());
	name += " (";

	
	switch(GetServerStatus())
	{
	case C_VS_UI_SERVER_SELECT::STATUS_VERY_GOOD:
		name +=(*g_pGameStringTable)[UI_STRING_MESSAGE_SERVER_STATUS_VERY_GOOD].GetString();
		break;
		
	case C_VS_UI_SERVER_SELECT::STATUS_GOOD:
		name +=(*g_pGameStringTable)[UI_STRING_MESSAGE_SERVER_STATUS_GOOD].GetString();
		break;
		
	case C_VS_UI_SERVER_SELECT::STATUS_NORMAL:
		name +=(*g_pGameStringTable)[UI_STRING_MESSAGE_SERVER_STATUS_NORMAL].GetString();
		break;
		
	case C_VS_UI_SERVER_SELECT::STATUS_BAD:
		name +=(*g_pGameStringTable)[UI_STRING_MESSAGE_SERVER_STATUS_BAD].GetString();
		break;
		
	case C_VS_UI_SERVER_SELECT::STATUS_VERY_BAD:
		name += (*g_pGameStringTable)[UI_STRING_MESSAGE_SERVER_STATUS_VERY_BAD].GetString();
		break;
		
	case C_VS_UI_SERVER_SELECT::STATUS_DOWN:
		name += (*g_pGameStringTable)[UI_STRING_MESSAGE_SERVER_STATUS_DOWN].GetString();
		break;
	}
	name += ")";
	
	CT_Done("DIAG: PROGRESS::Show pre PrintColorStr (server name)");
	// 2026-09-03 Renewal loading — yazi loading bar'in USTUNE gelmesin:
	// bar alt-orta (resy-70..resy-48), 0.95f carpani 1080p'de tam bar
	// uzerine dusuyordu. Renewal aktifken bar'in 30px ustune sabitlenir.
	int name_y = (int)(g_pUserInformation->iResolution_y * 0.95f);
	if (m_pC_renewal_image != NULL)
		name_y = g_pUserInformation->iResolution_y - 100;
	g_PrintColorStr(g_pUserInformation->iResolution_x / 2 - g_GetStringWidth(name.c_str(), gpC_base->m_dialog_menu_pi.hfont)/2,
		name_y, name.c_str(), gpC_base->m_desc_menu_pi, focus_color);//by larosel
	CT_Done("DIAG: PROGRESS::Show post PrintColorStr");

	if(m_progress_type == TYPE_MONSTER_INFO)
	{
		ShowDesc(x, y);
		p.x = m_desc_title_x + g_GetStringWidth(m_desc_title.c_str())/2 - (*m_pC_progress)[CHAR_NAME_ALPHA].GetWidth()/2;

		//ĳ���� �̸����� ����
		if(gpC_base->m_p_DDSurface_back->Lock())
		{
			p.y = m_desc_title_y-4;
			gpC_base->m_p_DDSurface_back->SetEffect( CSpriteSurface::EFFECT_SCREEN );
			gpC_base->m_p_DDSurface_back->BltSpriteEffect(&p, &(*m_pC_progress)[CHAR_NAME_ALPHA]);
			
			gpC_base->m_p_DDSurface_back->Unlock();
		}
	}

//	DrawBar();

	// percent ���
#ifndef _LIB
//	char str[5];
//	sprintf(str, "%d%%", m_percent);
//	g_Print(380, 500, str);
#endif

}
