// VS_UI_DESC.h: interface for the C_VS_UI_DESC class.
//
//////////////////////////////////////////////////////////////////////

// FAZ 4 (2026-04-29 Phase B): ContentsFilter.h zorunlu — bu sinifta
// __CONTENTS(__TUNING_ITEM) guard'i (L33-36) var, member layout'a
// etki ediyor (SysImg, isTuning).
#include "ContentsFilter.h"

#include "VS_UI_Util.h"
//#include "VS_UI_GameCommon.h"
#include "RarFile.h"
#include <vector>

#ifndef _VS_UI_DESC_H_
#define _VS_UI_DESC_H_

#define PICTURE_INDENT 20

struct DESC_SPRITE
{
	int pack_num;
	int sprite_num;
	int pos;
};

// FAZ 4 (2026-05-04) ROOT CAUSE COZULDU: 4 byte sizeof asimetrisi (Client=248
// vs vs_ui_lib=252) iki farkli RarFile.h'in include path'lerinde resolve
// olmasindan kaynakliydi:
//   - Designed_Common/Lib/ui/RarFile.h (legacy, 5 alan, m_pTempFile YOK)
//   - Designed_Common/Project/Client/RarLib/RarFile.h (guncel, 6 alan, m_pTempFile VAR)
// Client.exe → Lib/ui/RarFile.h (CRarFile sizeof=X), vs_ui_lib → RarLib/RarFile.h
// (CRarFile sizeof=X+4). VS_UI_DESC `CRarFile m_pack_file` alani araciligi ile
// 4 byte sizeof farki uretiyordu. Fix: Lib/ui/RarFile.h, RarLib versiyonu ile
// senkron edildi (ayni private layout).
//
// Eski Brief F pack(8) denemesi etkisizdi cunku problem padding degil ABI mismatch
// idi. pack(push, 8) yine de korundu — defansif izolasyon, ABI deterministik.
#pragma pack(push, 8)

class C_VS_UI_DESC
{
private:
	std::vector<DESC_SPRITE>	m_Sprite;
	int							m_desc_size;
	PrintInfo					m_pi;
	COLORREF					m_color;

	PrintInfo					m_title_pi;
	COLORREF					m_title_color;

	// FAZ 4 (2026-05-01): Tuning item field'lari KOSULSUZ. Phase B header
	// ContentsFilter include yetmedi — Client.exe TU'larindan biri compile
	// zamani __TUNING_ITEM 0 goruyor, lib_side 1, layout farki 4 byte
	// (sizeof_desc 248 vs 252) -> m_pC_progress/m_Draw_Num offset kayiyor
	// -> EndProgress destructor crash. Referansta CHS region hep __ON, hic
	// asimetri yoktu. Field'lari koşulsuz tutmak ABI'yi sabit yapar.
	int							SysImg;
	bool						isTuning;

protected:
	int							m_delimiter_pack, m_delimiter_sprite;
	int							m_desc_x, m_desc_y, m_desc_title_x, m_desc_title_y;
	int							m_desc_row, m_desc_col, m_desc_scroll;

	std::vector<std::string>	m_desc;
	std::vector<CSpritePack *>	m_pC_inpicture;
	CRarFile					m_pack_file;

	int							fontx;
	int							m_desc_y_distance;

	std::vector<std::string>			m_ori_string;
	std::vector<std::string>			m_rep_string;

	std::string						m_desc_title;

	// description�� ����Ѵ�
	void	ShowDesc(int x = 0, int y = 0);

	// description�� �ҷ��´�
	bool	LoadDesc(const char *szFilename, int row, int col, bool bl_title = false, int CoreZapID = -1);
	bool	LoadDescFromString(const char *szString, int row, int col, bool bl_title = false, int CoreZapID = -1);
	// description�� �����ǥ�� ���Ѵ�. 
	void	SetDesc(int dx, int dy, COLORREF color = BLACK, PrintInfo &pi = gpC_base->m_desc_msg_pi)
	{
		m_desc_x = dx;
		m_desc_y = dy;
		SetDescColor(color);
		SetDescPi(pi);
	}

	void	SetDescTitle(const char* str)	{ m_desc_title = str; }

	void	SetDescTitle(int dx, int dy, COLORREF color = RGB_WHITE, PrintInfo &pi = gpC_base->m_desc_menu_pi)
	{
		m_desc_title_x = dx;
		m_desc_title_y = dy; 
		m_title_color = color;
		m_title_pi = pi;
	}

	// FAZ 4: Field'lar koşulsuz oldugu icin method'lar da koşulsuz
	void	SetSysImg(int si) { SysImg = si; }
	void	SetTuning() { isTuning = true; }

	//
	void	SetDescColor(COLORREF color)	{ m_color = color; }
	void	SetDescPi(PrintInfo &pi)		{ m_pi = pi; }

	COLORREF	GetDescColor()				{ return m_color; }
	PrintInfo	SetDescPi()					{ return m_pi; }
	//
	void	SetSprite(int pack, int num, int line);

	// scroll
	void	ScrollDescUp()					{ if(m_desc_scroll > 0)m_desc_scroll--; }
	void	ScrollDescDown()				{ if(m_desc_scroll  + m_desc_col < GetDescSize() )m_desc_scroll++; }
	void	SetDescScrollPos(int pos)		{ m_desc_scroll = pos; }
	int		GetDescScrollPos()				{ return m_desc_scroll; }
	int		GetDescPage()					{ return (m_desc.size()+m_desc_col-1)/m_desc_col; }
	int		GetDescSize()					{ return m_desc.size(); }
	int		GetDescCol()					{ return m_desc_col; }

public:
	C_VS_UI_DESC();
	~C_VS_UI_DESC();


};

#pragma pack(pop)  // FAZ 4 Brief F ABI izole bitiş

// FAZ 4 (2026-05-04) ABI SENTINEL — Cross-TU sizeof guarantee.
// Bu static_assert hem Client.exe hem vs_ui_lib hedeflerinde compile-time'da
// kontrol edilir. Eger ileride biri 252'den farkli bir sayi uretirse build
// hemen kirilir, ABI mismatch sessizce gecmez (8 deneme boyunca eksik
// olan kanit mekanizmasi).
//
// Beklenen: 252 byte (RarFile.h sync sonrasi).
// Eger fail ederse: hangi member 4 byte fark uretiyor diye CRarFile, std::string,
// std::vector<DESC_SPRITE>, PrintInfo (Fl2.h:23 — 20 byte sabit) sirasiyla
// dogrulanmali. Iki TU'nun gordugu header farkliligi en muhtemel sebep.
static_assert(sizeof(C_VS_UI_DESC) == 252,
	"C_VS_UI_DESC ABI mismatch — bekleniyor 252 byte. RarFile.h cross-TU "
	"sync edildi; eger hala fail ediyorsa Lib/ui ve Project/Client/RarLib "
	"icindeki header'lari karsilastir.");

// FAZ 4 forensic gecmis kaydi (2026-05-02 .. 2026-05-04)
//   1-6: TUNING_ITEM kosulsuz / /permissive / PCH-basi ContentsFilter /
//        force obj delete / clean rebuild / duplicate ContentsFilter kaldirma
//   7: Brief F #pragma pack(push, 8) ABI izole — etkisiz
//   8: Sentinel kanit (49-81 cpp) — 248 vs 252 dogrulandi
//   9 (2026-05-04): ROOT CAUSE — Lib/ui/RarFile.h (5 alan) vs Project/Client/
//        RarLib/RarFile.h (6 alan, m_pTempFile). Lib/ui sync edildi → resolved.

#endif
