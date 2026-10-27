//----------------------------------------------------------------------
// CSpriteSurface.cpp
//----------------------------------------------------------------------
#include "CSprite.h"
#include "CAlphaSprite.h"
#include "CIndexSprite.h"
#include "CShadowSprite.h"
#include "CSpriteOutlineManager.h"
#include "CFilter.h"
#include "CSpriteSurface.h"

// FULL DX11 gecis Faz G1 (2026-09-01) - dx11gpu GPU quad yolu + dirty mark.
// D3D9/dx11 modlarinda tum G1 yardimcilari no-op (IsCompositionTarget false).
#include "D3D11QuadPipeline.h"
#include "D3D11SpriteCommands.h"

#include <vector>

namespace
{

using phoenix::render::D3D11QuadPipeline;
using phoenix::render::D3D11SpriteCommands;
using phoenix::render::D3D11SpriteTextureCache;

//----------------------------------------------------------------------
// G1 dirty mark - CPU yolunda kalan cizimler komut katmanina bolge
// bildirir (sirali segment cikarimi icin). Composition disi surface veya
// dx11gpu disi modda no-op.
//----------------------------------------------------------------------
inline void G1_Mark(void* lpSurface, int x, int y, int w, int h)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface))
		return;
	RECT r;
	r.left = x;
	r.top = y;
	r.right = x + w;
	r.bottom = y + h;
	sc.MarkDirty(&r);
}

inline void G1_MarkRect(void* lpSurface, const RECT* pRect)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (sc.IsCompositionTarget(lpSurface))
		sc.MarkDirty(pRect);
}

//----------------------------------------------------------------------
// CSprite RLE -> RGBA8 decode (texture cache miss'inde bir kez kosar).
// Satir formati: (repeat)(skipCount, colorCount, colors...)* - skip
// pikselleri seffaf (alpha 0), color pikselleri opak (alpha 255).
//----------------------------------------------------------------------
inline DWORD G1_565ToRGBA(WORD px)
{
	DWORD r = (px >> 11) & 0x1F;
	DWORD g = (px >> 5)  & 0x3F;
	DWORD b =  px        & 0x1F;
	r = (r << 3) | (r >> 2);
	g = (g << 2) | (g >> 4);
	b = (b << 3) | (b >> 2);
	return 0xFF000000u | (b << 16) | (g << 8) | r;
}

// Icerik anahtari: sprite pointer'i yeniden kullanilirsa (pack unload +
// yeni load ayni adrese) bayat texture donmesin - boyut + ilk satir RLE
// iceriginden ucuz FNV hash'i generation olarak kullanilir.
inline unsigned int G1_SpriteGeneration(const CSprite* pSprite)
{
	unsigned int h = 2166136261u;
	h = (h ^ (unsigned int)pSprite->GetWidth()) * 16777619u;
	h = (h ^ (unsigned int)pSprite->GetHeight()) * 16777619u;
	if (pSprite->GetHeight() > 0)
	{
		// RLE yapisini guvenli yuru (satir sonu otesine okuma yok):
		// (count)(skip, colorCount, colors...)* - run basliklarini ve her
		// run'in ilk rengini hash'e kat, en fazla 8 run.
		const WORD* p = pSprite->GetPixelLine(0);
		if (p != NULL)
		{
			int count = (int)*p++;
			h = (h ^ (unsigned int)count) * 16777619u;
			for (int run = 0; run < count && run < 8; ++run)
			{
				const WORD skip = *p++;
				const WORD colorCount = *p++;
				h = (h ^ (unsigned int)skip) * 16777619u;
				h = (h ^ (unsigned int)colorCount) * 16777619u;
				if (colorCount > 0)
					h = (h ^ (unsigned int)p[0]) * 16777619u;
				p += colorCount;
			}
		}
	}
	return h;
}

ID3D11ShaderResourceView* G1_GetSpriteSRV(CSprite* pSprite, int* pW, int* pH)
{
	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	if (w <= 0 || h <= 0)
		return NULL;

	D3D11SpriteTextureCache& cache = D3D11SpriteTextureCache::Instance();
	const unsigned int gen = G1_SpriteGeneration(pSprite);

	ID3D11ShaderResourceView* pSRV = cache.Lookup(pSprite, gen, pW, pH);
	if (pSRV != NULL)
		return pSRV;

	// Miss - RLE decode (seffaf = alpha 0)
	std::vector<DWORD> rgba((size_t)w * h, 0u);
	for (int y = 0; y < h; ++y)
	{
		const WORD* pPixels = pSprite->GetPixelLine((WORD)y);
		if (pPixels == NULL)
			continue;
		DWORD* pDst = &rgba[(size_t)y * w];

		int count = (int)*pPixels++;
		int x = 0;
		while (count-- > 0)
		{
			x += (int)*pPixels++;               // seffaf atlama
			int colorCount = (int)*pPixels++;   // opak pikseller
			for (int i = 0; i < colorCount && x < w; ++i, ++x)
				pDst[x] = G1_565ToRGBA(pPixels[i]);
			pPixels += colorCount;
		}
	}

	pSRV = cache.InsertRGBA(pSprite, gen, &rgba[0], w, h);
	if (pSRV != NULL)
	{
		if (pW) *pW = w;
		if (pH) *pH = h;
	}
	return pSRV;
}

// CShadowSprite RLE -> RGBA8 maske (opak SIYAH - CShadowSprite::Blt
// memset(0) paritesi; seffaf = alpha 0).
inline unsigned int G1_ShadowGeneration(const CShadowSprite* pSprite)
{
	unsigned int h = 2166136261u;
	h = (h ^ 0x53480000u) * 16777619u;   // 'SH' etiketi - CSprite ile ayrissin
	h = (h ^ (unsigned int)pSprite->GetWidth()) * 16777619u;
	h = (h ^ (unsigned int)pSprite->GetHeight()) * 16777619u;
	if (pSprite->GetHeight() > 0)
	{
		// Golge RLE: (count)(skip, shadowCount)* - renk verisi yok.
		const WORD* p = pSprite->GetPixelLine(0);
		if (p != NULL)
		{
			int count = (int)*p++;
			h = (h ^ (unsigned int)count) * 16777619u;
			for (int run = 0; run < count && run < 8; ++run)
			{
				h = (h ^ (unsigned int)p[0]) * 16777619u;
				h = (h ^ (unsigned int)p[1]) * 16777619u;
				p += 2;
			}
		}
	}
	return h;
}

ID3D11ShaderResourceView* G1_GetShadowSRV(CShadowSprite* pSprite, int* pW, int* pH)
{
	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	if (w <= 0 || h <= 0)
		return NULL;

	D3D11SpriteTextureCache& cache = D3D11SpriteTextureCache::Instance();
	const unsigned int gen = G1_ShadowGeneration(pSprite);

	ID3D11ShaderResourceView* pSRV = cache.Lookup(pSprite, gen, pW, pH);
	if (pSRV != NULL)
		return pSRV;

	std::vector<DWORD> rgba((size_t)w * h, 0u);
	for (int y = 0; y < h; ++y)
	{
		const WORD* pPixels = pSprite->GetPixelLine((WORD)y);
		if (pPixels == NULL)
			continue;
		DWORD* pDst = &rgba[(size_t)y * w];

		int count = (int)*pPixels++;
		int x = 0;
		while (count-- > 0)
		{
			x += (int)*pPixels++;               // seffaf atlama
			int shadowCount = (int)*pPixels++;  // golge pikselleri (renk verisi yok)
			for (int i = 0; i < shadowCount && x < w; ++i, ++x)
				pDst[x] = 0xFF000000u;          // opak siyah
		}
	}

	pSRV = cache.InsertRGBA(pSprite, gen, &rgba[0], w, h);
	if (pSRV != NULL)
	{
		if (pW) *pW = w;
		if (pH) *pH = h;
	}
	return pSRV;
}

//----------------------------------------------------------------------
// Ortak quad kaydi: dest = (pt, sprite boyutu veya scaled boyut) kesisim
// clip. true = GPU devraldi (caller return etmeli); false = CPU yoluna
// devam (caller kendi G1_Mark'ini yapar - hook zinciri boyle kurulur).
//----------------------------------------------------------------------
bool G1_RecordSprite(void* lpSurface,
                     int clipL, int clipT, int clipR, int clipB,
                     const POINT* pPoint, int sprW, int sprH,
                     int destW, int destH,
                     ID3D11ShaderResourceView* pSRV, int texW, int texH,
                     DWORD colorRGBA, D3D11QuadPipeline::BlendMode blend)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive() || pSRV == NULL)
		return false;

	// dest = hedef kesisim clip
	RECT dest;
	dest.left = pPoint->x;
	dest.top = pPoint->y;
	dest.right = pPoint->x + destW;
	dest.bottom = pPoint->y + destH;
	if (dest.left < clipL)   dest.left = clipL;
	if (dest.top < clipT)    dest.top = clipT;
	if (dest.right > clipR)  dest.right = clipR;
	if (dest.bottom > clipB) dest.bottom = clipB;
	if (dest.left >= dest.right || dest.top >= dest.bottom)
		return true;   // tamamen clip disi - legacy de hicbir sey cizmezdi

	// src: dest'in sprite icindeki karsiligi (scale destekli oran)
	RECT src;
	src.left   = (LONG)((dest.left - pPoint->x) * (double)sprW / destW);
	src.top    = (LONG)((dest.top - pPoint->y) * (double)sprH / destH);
	src.right  = (LONG)((dest.right - pPoint->x) * (double)sprW / destW);
	src.bottom = (LONG)((dest.bottom - pPoint->y) * (double)sprH / destH);
	if (src.right > sprW)  src.right = sprW;
	if (src.bottom > sprH) src.bottom = sprH;

	return sc.RecordQuad(pSRV, texW, texH, dest, src, colorRGBA, blend);
}

//----------------------------------------------------------------------
// Fonksiyon basina tek satirlik hook'lar. true = GPU devraldi.
// destW/destH -1 = sprite boyutu (scale yok).
//----------------------------------------------------------------------
bool G1_TryBltSprite(void* lpSurface,
                     int clipL, int clipT, int clipR, int clipB,
                     const POINT* pPoint, CSprite* pSprite,
                     DWORD colorRGBA, D3D11QuadPipeline::BlendMode blend,
                     int destW = -1, int destH = -1)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	int texW = 0, texH = 0;
	ID3D11ShaderResourceView* pSRV = G1_GetSpriteSRV(pSprite, &texW, &texH);
	if (pSRV == NULL)
		return false;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
	                       w, h,
	                       (destW > 0) ? destW : w,
	                       (destH > 0) ? destH : h,
	                       pSRV, texW, texH, colorRGBA, blend);
}

bool G1_TryBltShadow(void* lpSurface,
                     int clipL, int clipT, int clipR, int clipB,
                     const POINT* pPoint, CShadowSprite* pSprite,
                     int destW = -1, int destH = -1)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	int texW = 0, texH = 0;
	ID3D11ShaderResourceView* pSRV = G1_GetShadowSRV(pSprite, &texW, &texH);
	if (pSRV == NULL)
		return false;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	// Maske texture'i opak siyah - CShadowSprite::Blt memset(0) paritesi
	return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
	                       w, h,
	                       (destW > 0) ? destW : w,
	                       (destH > 0) ? destH : h,
	                       pSRV, texW, texH, 0xFFFFFFFFu,
	                       D3D11QuadPipeline::BLEND_ALPHA);
}

//----------------------------------------------------------------------
// Faz G2 (2026-09-01) - Color/Darkness/ColorSet/AlphaSprite GPU yollari
//----------------------------------------------------------------------
// BltSpriteColor: dest = src & MASK_RGB[rgb] (kanal maskeleme). Modulate
// katsayisi 255 = kanal aynen gecer, 0 = sifirlanir -> birebir esdeger.
inline DWORD G2_ColorMaskToModulate(BYTE rgb)   // 0..5: R,G,B,RG,RB,GB
{
	switch (rgb)
	{
	case 0: return D3D11QuadPipeline::MakeColor(255,   0,   0, 255);
	case 1: return D3D11QuadPipeline::MakeColor(  0, 255,   0, 255);
	case 2: return D3D11QuadPipeline::MakeColor(  0,   0, 255, 255);
	case 3: return D3D11QuadPipeline::MakeColor(255, 255,   0, 255);
	case 4: return D3D11QuadPipeline::MakeColor(255,   0, 255, 255);
	case 5: return D3D11QuadPipeline::MakeColor(  0, 255, 255, 255);
	}
	return 0;   // gecersiz index -> caller CPU yoluna duser
}

//----------------------------------------------------------------------
// BltSpriteColorSet: per-pixel LUT remap (ColorToGradation -> ColorSet).
// Modulate ile ifade edilemez; decode sirasinda LUT uygulanip sonuc
// (sprite, colorSet) ciftine ozel texture olarak cache'lenir.
// Anahtar: sprite adresi + (colorSet+1)<<20 - dusuk bitlerde komsu sprite
// adresleriyle cakismaz; olasi uzak cakismalari generation hash'i yakalar.
//----------------------------------------------------------------------
inline const void* G2_ColorSetKey(const CSprite* pSprite, WORD colorSet)
{
	return (const void*)((const BYTE*)pSprite + (((size_t)colorSet + 1) << 20));
}

inline unsigned int G2_ColorSetGeneration(const CSprite* pSprite, WORD colorSet)
{
	return G1_SpriteGeneration(pSprite) ^ (0x43530000u + (unsigned int)colorSet * 2654435761u);
}

ID3D11ShaderResourceView* G2_GetSpriteColorSetSRV(CSprite* pSprite, WORD colorSet,
                                                  int* pW, int* pH)
{
	if (colorSet >= MAX_COLORSET)
		return NULL;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	if (w <= 0 || h <= 0)
		return NULL;

	D3D11SpriteTextureCache& cache = D3D11SpriteTextureCache::Instance();
	const void* key = G2_ColorSetKey(pSprite, colorSet);
	const unsigned int gen = G2_ColorSetGeneration(pSprite, colorSet);

	ID3D11ShaderResourceView* pSRV = cache.Lookup(key, gen, pW, pH);
	if (pSRV != NULL)
		return pSRV;

	// Miss - RLE decode + memcpyEffectGradation LUT paritesi:
	// r+g+b (5'er bit) -> ColorToGradation -> ColorSet[colorSet][grad]
	std::vector<DWORD> rgba((size_t)w * h, 0u);
	for (int y = 0; y < h; ++y)
	{
		const WORD* pPixels = pSprite->GetPixelLine((WORD)y);
		if (pPixels == NULL)
			continue;
		DWORD* pDst = &rgba[(size_t)y * w];

		int count = (int)*pPixels++;
		int x = 0;
		while (count-- > 0)
		{
			x += (int)*pPixels++;
			int colorCount = (int)*pPixels++;
			for (int i = 0; i < colorCount && x < w; ++i, ++x)
			{
				const WORD s = pPixels[i];
				const int r = (int)(s >> CDirectDraw::s_bSHIFT_R);
				const int g = (int)((s >> CDirectDraw::s_bSHIFT_G) & 0x1F);
				const int b = (int)(s & 0x1F);
				const BYTE grad = CIndexSprite::ColorToGradation[r + g + b];
				pDst[x] = G1_565ToRGBA(CIndexSprite::ColorSet[colorSet][grad]);
			}
			pPixels += colorCount;
		}
	}

	pSRV = cache.InsertRGBA(key, gen, &rgba[0], w, h);
	if (pSRV != NULL)
	{
		if (pW) *pW = w;
		if (pH) *pH = h;
	}
	return pSRV;
}

bool G2_TryBltColorSet(void* lpSurface,
                       int clipL, int clipT, int clipR, int clipB,
                       const POINT* pPoint, CSprite* pSprite, WORD colorSet)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	int texW = 0, texH = 0;
	ID3D11ShaderResourceView* pSRV = G2_GetSpriteColorSetSRV(pSprite, colorSet, &texW, &texH);
	if (pSRV == NULL)
		return false;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
	                       w, h, w, h, pSRV, texW, texH,
	                       0xFFFFFFFFu, D3D11QuadPipeline::BLEND_ALPHA);
}

//----------------------------------------------------------------------
// CAlphaSprite RLE -> RGBA8 decode. Satir formati:
// (count)(skip, colorCount, [alphaWord, colorWord] x colorCount)*
// bMask=false: per-pixel alpha (alpha = word>>8, 0..32 -> a8=alpha*255/32)
//              BltAlphaSprite/memcpyAlpha lerp paritesi.
// bMask=true : per-pixel alpha YOK SAYILIR (a8=255) - BltAlphaSpriteAlpha/
//              memcpyAlphaValue uniform alpha'yi modulate ile uygular.
//----------------------------------------------------------------------
inline unsigned int G2_AlphaSpriteGeneration(const CAlphaSprite* pSprite, bool bMask)
{
	unsigned int h = 2166136261u;
	h = (h ^ (bMask ? 0x414D0000u : 0x41530000u)) * 16777619u;   // 'AM' / 'AS'
	h = (h ^ (unsigned int)pSprite->GetWidth()) * 16777619u;
	h = (h ^ (unsigned int)pSprite->GetHeight()) * 16777619u;
	if (pSprite->GetHeight() > 0)
	{
		const WORD* p = pSprite->GetPixelLine(0);
		if (p != NULL)
		{
			int count = (int)*p++;
			h = (h ^ (unsigned int)count) * 16777619u;
			for (int run = 0; run < count && run < 8; ++run)
			{
				const WORD skip = *p++;
				const WORD colorCount = *p++;
				h = (h ^ (unsigned int)skip) * 16777619u;
				h = (h ^ (unsigned int)colorCount) * 16777619u;
				if (colorCount > 0)
				{
					h = (h ^ (unsigned int)p[0]) * 16777619u;   // ilk alpha
					h = (h ^ (unsigned int)p[1]) * 16777619u;   // ilk renk
				}
				p += (colorCount << 1);   // (alpha,renk) cifti
			}
		}
	}
	return h;
}

ID3D11ShaderResourceView* G2_GetAlphaSpriteSRV(CAlphaSprite* pSprite, bool bMask,
                                               int* pW, int* pH)
{
	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	if (w <= 0 || h <= 0)
		return NULL;

	D3D11SpriteTextureCache& cache = D3D11SpriteTextureCache::Instance();
	// Mask varyanti ayri anahtar (ayni sprite iki icerikle cache'lenebilir)
	const void* key = bMask ? (const void*)((const BYTE*)pSprite + ((size_t)1 << 20))
	                        : (const void*)pSprite;
	const unsigned int gen = G2_AlphaSpriteGeneration(pSprite, bMask);

	ID3D11ShaderResourceView* pSRV = cache.Lookup(key, gen, pW, pH);
	if (pSRV != NULL)
		return pSRV;

	std::vector<DWORD> rgba((size_t)w * h, 0u);
	for (int y = 0; y < h; ++y)
	{
		const WORD* pPixels = pSprite->GetPixelLine((WORD)y);
		if (pPixels == NULL)
			continue;
		DWORD* pDst = &rgba[(size_t)y * w];

		int count = (int)*pPixels++;
		int x = 0;
		while (count-- > 0)
		{
			x += (int)*pPixels++;
			int colorCount = (int)*pPixels++;
			for (int i = 0; i < colorCount && x < w; ++i, ++x)
			{
				const WORD alphaWord = pPixels[(size_t)i << 1];
				const WORD colorWord = pPixels[((size_t)i << 1) + 1];
				DWORD px = G1_565ToRGBA(colorWord);
				if (!bMask)
				{
					// alpha 0..32 -> 0..255 (32 = tam opak, legacy >>5 lerp)
					DWORD a = (DWORD)(alphaWord >> 8);
					a = (a >= 32) ? 255u : (a * 255u) / 32u;
					px = (px & 0x00FFFFFFu) | (a << 24);
				}
				pDst[x] = px;
			}
			pPixels += (colorCount << 1);
		}
	}

	pSRV = cache.InsertRGBA(key, gen, &rgba[0], w, h);
	if (pSRV != NULL)
	{
		if (pW) *pW = w;
		if (pH) *pH = h;
	}
	return pSRV;
}

bool G2_TryBltAlphaSprite(void* lpSurface,
                          int clipL, int clipT, int clipR, int clipB,
                          const POINT* pPoint, CAlphaSprite* pSprite,
                          bool bMask, DWORD colorRGBA)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	int texW = 0, texH = 0;
	ID3D11ShaderResourceView* pSRV = G2_GetAlphaSpriteSRV(pSprite, bMask, &texW, &texH);
	if (pSRV == NULL)
		return false;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
	                       w, h, w, h, pSRV, texW, texH,
	                       colorRGBA, D3D11QuadPipeline::BLEND_ALPHA);
}

//----------------------------------------------------------------------
// Faz G6 (2026-09-01) - CAlphaSpritePal GPU yolu (KRITIK katman fix'i)
//----------------------------------------------------------------------
// Zone/skill efekt bulutlari (m_EffectAlphaPPK) BltAlphaSpritePal ile
// cizilir. CPU yolu composition'daki BAYAT dst ile lerp yapar (yaratik
// GPU quad'inda, composition'da yok) -> diff segmenti bayat blend'i OPAK
// basar -> efektin arkasindaki yaratik/karakter gorunmez olur (kullanici
// raporu 2026-09-01: "animasyonlarin arkasindaysa gorunmuyorlar").
// GPU quad kaydi sirali komut listesinde dogru katmana oturur.
//
// RLE satir formati (BYTE bazli, CAlphaSpritePal::SetPixel):
// (count)(skip, colorCount, [alphaByte, palIndexByte] x colorCount)*
// memcpyAlpha lerp: out = dst + (pal[idx]-dst)*alpha/32  (alpha 1..32)
//   -> straight alpha blend, a8 = alpha*255/32.
// memcpyAlphaValue (BltAlphaSpritePalAlpha): per-pixel alpha YOK SAYILIR,
// uniform s_Value1 uygulanir -> mask texture (a8=255) + modulate alpha.
//----------------------------------------------------------------------
inline unsigned int G6_AlphaSpritePalGeneration(const CAlphaSpritePal* pSprite,
                                                const MPalette& pal, bool bMask)
{
	unsigned int h = 2166136261u;
	h = (h ^ (bMask ? 0x504D0000u : 0x50530000u)) * 16777619u;   // 'PM' / 'PS'
	h = (h ^ (unsigned int)pSprite->GetWidth()) * 16777619u;
	h = (h ^ (unsigned int)pSprite->GetHeight()) * 16777619u;

	// Palet icerigi (animasyonlu paletlerde invalidation sart; <=256 WORD)
	const int palSize = (int)pal.GetSize();
	h = (h ^ (unsigned int)palSize) * 16777619u;
	for (int i = 0; i < palSize; ++i)
		h = (h ^ (unsigned int)pal[(BYTE)i]) * 16777619u;

	// Ilk satirin run yapisi (sprite icerigi sabit ama pointer reuse'a karsi)
	if (pSprite->GetHeight() > 0)
	{
		const BYTE* p = pSprite->GetPixelLine(0);
		if (p != NULL)
		{
			int count = (int)*p++;
			h = (h ^ (unsigned int)count) * 16777619u;
			for (int run = 0; run < count && run < 8; ++run)
			{
				const BYTE skip = *p++;
				const BYTE colorCount = *p++;
				h = (h ^ (unsigned int)skip) * 16777619u;
				h = (h ^ (unsigned int)colorCount) * 16777619u;
				if (colorCount > 0)
				{
					h = (h ^ (unsigned int)p[0]) * 16777619u;   // ilk alpha
					h = (h ^ (unsigned int)p[1]) * 16777619u;   // ilk indeks
				}
				p += ((size_t)colorCount << 1);   // (alpha,indeks) cifti
			}
		}
	}
	return h;
}

ID3D11ShaderResourceView* G6_GetAlphaSpritePalSRV(CAlphaSpritePal* pSprite,
                                                  MPalette& pal, bool bMask,
                                                  int* pW, int* pH)
{
	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	if (w <= 0 || h <= 0)
		return NULL;

	D3D11SpriteTextureCache& cache = D3D11SpriteTextureCache::Instance();
	// Anahtar sprite+palet ikilisine baglanir (ayni sprite farkli paletle
	// ayni frame'de cizilebilir — tek anahtar gen ping-pong'u yapardi).
	// Mask varyanti ek offset ile ayrisir.
	uintptr_t keyBits = (uintptr_t)pSprite ^ ((uintptr_t)&pal << 1);
	if (bMask)
		keyBits ^= ((uintptr_t)1 << 21);
	const void* key = (const void*)keyBits;
	const unsigned int gen = G6_AlphaSpritePalGeneration(pSprite, pal, bMask);

	ID3D11ShaderResourceView* pSRV = cache.Lookup(key, gen, pW, pH);
	if (pSRV != NULL)
		return pSRV;

	std::vector<DWORD> rgba((size_t)w * h, 0u);
	for (int y = 0; y < h; ++y)
	{
		const BYTE* pPixels = pSprite->GetPixelLine((WORD)y);
		if (pPixels == NULL)
			continue;
		DWORD* pDst = &rgba[(size_t)y * w];

		int count = (int)*pPixels++;
		int x = 0;
		while (count-- > 0)
		{
			x += (int)*pPixels++;
			int colorCount = (int)*pPixels++;
			for (int i = 0; i < colorCount && x < w; ++i, ++x)
			{
				const BYTE alphaByte = pPixels[(size_t)i << 1];
				const BYTE palIndex  = pPixels[((size_t)i << 1) + 1];
				DWORD px = G1_565ToRGBA(pal[palIndex]);
				if (!bMask)
				{
					// alpha 0..32 -> 0..255 (32 = tam opak, legacy >>5 lerp)
					DWORD a = (DWORD)alphaByte;
					a = (a >= 32) ? 255u : (a * 255u) / 32u;
					px = (px & 0x00FFFFFFu) | (a << 24);
				}
				pDst[x] = px;
			}
			pPixels += ((size_t)colorCount << 1);
		}
	}

	pSRV = cache.InsertRGBA(key, gen, &rgba[0], w, h);
	if (pSRV != NULL)
	{
		if (pW) *pW = w;
		if (pH) *pH = h;
	}
	return pSRV;
}

bool G6_TryBltAlphaSpritePal(void* lpSurface,
                             int clipL, int clipT, int clipR, int clipB,
                             const POINT* pPoint, CAlphaSpritePal* pSprite,
                             MPalette& pal, bool bMask, DWORD colorRGBA)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	int texW = 0, texH = 0;
	ID3D11ShaderResourceView* pSRV =
	    G6_GetAlphaSpritePalSRV(pSprite, pal, bMask, &texW, &texH);
	if (pSRV == NULL)
		return false;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
	                       w, h, w, h, pSRV, texW, texH,
	                       colorRGBA, D3D11QuadPipeline::BLEND_ALPHA);
}

//----------------------------------------------------------------------
// Faz G3 (2026-09-01) - Effect blend + IndexSprite GPU yollari
//----------------------------------------------------------------------
// Fixed-function blend ile birebir ifade edilebilen effect'ler.
// Kalanlar (ColorDodge, Different, SimpleOutline, WipeOut, Net,
// GrayScaleVarious, ScreenAlpha) CPU'da kalir.
bool G3_EffectToBlend(int effect, D3D11QuadPipeline::BlendMode* pBlend)
{
	switch (effect)
	{
	case CSpriteSurface::EFFECT_DARKER:      // d*s>>5
		*pBlend = D3D11QuadPipeline::BLEND_MULTIPLY; return true;
	case CSpriteSurface::EFFECT_LIGHTEN:     // max(d,s)
		*pBlend = D3D11QuadPipeline::BLEND_MAX;      return true;
	case CSpriteSurface::EFFECT_DARKEN:      // min(d,s)
		*pBlend = D3D11QuadPipeline::BLEND_MIN;      return true;
	case CSpriteSurface::EFFECT_SCREEN:      // s+d-sd (tablo formulu)
		*pBlend = D3D11QuadPipeline::BLEND_SCREEN;   return true;
	case CSpriteSurface::EFFECT_DODGE_BURN:  // d*(32-s)>>5
		*pBlend = D3D11QuadPipeline::BLEND_INVMULT;  return true;
	}
	return false;
}

// EFFECT_GRAY_SCALE dst-BAGIMSIZ: dest = gri(src). Decode varyanti.
// Legacy 565 kelime birebir kurulur (yesil 5-bit deger 6-bit alana
// s_bSHIFT_G ile yazilir - legacy'nin kendine has karanlik yesili korunur).
ID3D11ShaderResourceView* G3_GetSpriteGraySRV(CSprite* pSprite, int* pW, int* pH)
{
	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	if (w <= 0 || h <= 0)
		return NULL;

	D3D11SpriteTextureCache& cache = D3D11SpriteTextureCache::Instance();
	const void* key = (const void*)((const BYTE*)pSprite + (size_t)0x40000000u);
	const unsigned int gen = G1_SpriteGeneration(pSprite) ^ 0x47524159u;   // 'GRAY'

	ID3D11ShaderResourceView* pSRV = cache.Lookup(key, gen, pW, pH);
	if (pSRV != NULL)
		return pSRV;

	std::vector<DWORD> rgba((size_t)w * h, 0u);
	for (int y = 0; y < h; ++y)
	{
		const WORD* pPixels = pSprite->GetPixelLine((WORD)y);
		if (pPixels == NULL)
			continue;
		DWORD* pDst = &rgba[(size_t)y * w];

		int count = (int)*pPixels++;
		int x = 0;
		while (count-- > 0)
		{
			x += (int)*pPixels++;
			int colorCount = (int)*pPixels++;
			for (int i = 0; i < colorCount && x < w; ++i, ++x)
			{
				const WORD s = pPixels[i];
				const int avg = (CDirectDraw::Red(s) + CDirectDraw::Green(s) +
				                 CDirectDraw::Blue(s)) / 3;
				const WORD out = (WORD)((avg << CDirectDraw::s_bSHIFT_R) |
				                        (avg << CDirectDraw::s_bSHIFT_G) | avg);
				pDst[x] = G1_565ToRGBA(out);
			}
			pPixels += colorCount;
		}
	}

	pSRV = cache.InsertRGBA(key, gen, &rgba[0], w, h);
	if (pSRV != NULL)
	{
		if (pW) *pW = w;
		if (pH) *pH = h;
	}
	return pSRV;
}

// BltSpriteEffect GPU dali. dst-okuyan blend'ler GPU RT uzerinde calisir
// (CPU fallback'in aksine GPU quad'larin SONUCUNU gorur - dogruluk artar).
bool G3_TryBltSpriteEffect(void* lpSurface,
                           int clipL, int clipT, int clipR, int clipB,
                           const POINT* pPoint, CSprite* pSprite)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	const int effect = (int)CSpriteSurface::s_CurrentEffect;
	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	int texW = 0, texH = 0;

	D3D11QuadPipeline::BlendMode blend;
	if (G3_EffectToBlend(effect, &blend))
	{
		ID3D11ShaderResourceView* pSRV = G1_GetSpriteSRV(pSprite, &texW, &texH);
		if (pSRV == NULL)
			return false;
		return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
		                       w, h, w, h, pSRV, texW, texH, 0xFFFFFFFFu, blend);
	}
	if (effect == CSpriteSurface::EFFECT_GRAY_SCALE)
	{
		ID3D11ShaderResourceView* pSRV = G3_GetSpriteGraySRV(pSprite, &texW, &texH);
		if (pSRV == NULL)
			return false;
		return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
		                       w, h, w, h, pSRV, texW, texH, 0xFFFFFFFFu,
		                       D3D11QuadPipeline::BLEND_ALPHA);
	}
	if (effect == CSpriteSurface::EFFECT_GRADATION)
	{
		// memcpyEffectGradation = ColorSet remap (s_Value1 = colorset no)
		const int cs = CSpriteSurface::s_Value1;
		if (cs < 0 || cs >= MAX_COLORSET)
			return false;
		ID3D11ShaderResourceView* pSRV =
			G2_GetSpriteColorSetSRV(pSprite, (WORD)cs, &texW, &texH);
		if (pSRV == NULL)
			return false;
		return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
		                       w, h, w, h, pSRV, texW, texH, 0xFFFFFFFFu,
		                       D3D11QuadPipeline::BLEND_ALPHA);
	}
	return false;   // ifade edilemeyen effect -> CPU
}

//----------------------------------------------------------------------
// CIndexSprite RLE: (transPair)(skip, indexCount, [idxWord]xN,
//                    colorCount, [renk565]xM)*
// idxWord = (colorSetSlot<<8)|gradation.
//
// PLAIN decode: idx -> ColorSet[s_IndexValue[slot]][grad] (creature renk
// remap'i). s_IndexValue global oldugu icin hash'i hem anahtara hem
// generation'a girer - farkli creature'lar ayni sprite'i farkli renkte
// cache'ler (thrash yok).
//
// RAW decode (BltIndexSpriteEffect paritesi): legacy, effect fonksiyonuna
// idxWord'u OLDUGU GIBI 565 renk diye verir - ayni davranis korunur.
//----------------------------------------------------------------------
inline unsigned int G3_IndexValueHash()
{
	unsigned int h = 2166136261u;
	for (int n = 0; n < MAX_COLORSET_USE; ++n)
		h = (h ^ (unsigned int)CIndexSprite::GetUsingColorSet((BYTE)n)) * 16777619u;
	return h;
}

inline unsigned int G3_IndexContentHash(const CIndexSprite* pSprite)
{
	unsigned int h = 2166136261u;
	h = (h ^ 0x49445800u) * 16777619u;   // 'IDX'
	h = (h ^ (unsigned int)pSprite->GetWidth()) * 16777619u;
	h = (h ^ (unsigned int)pSprite->GetHeight()) * 16777619u;
	if (pSprite->GetHeight() > 0)
	{
		const WORD* p = pSprite->GetPixelLine(0);
		if (p != NULL)
		{
			int transPair = (int)*p++;
			h = (h ^ (unsigned int)transPair) * 16777619u;
			for (int run = 0; run < transPair && run < 6; ++run)
			{
				const WORD skip = *p++;
				const WORD idxCount = *p++;
				h = (h ^ (unsigned int)skip) * 16777619u;
				h = (h ^ (unsigned int)idxCount) * 16777619u;
				if (idxCount > 0)
					h = (h ^ (unsigned int)p[0]) * 16777619u;
				p += idxCount;
				const WORD colorCount = *p++;
				h = (h ^ (unsigned int)colorCount) * 16777619u;
				if (colorCount > 0)
					h = (h ^ (unsigned int)p[0]) * 16777619u;
				p += colorCount;
			}
		}
	}
	return h;
}

ID3D11ShaderResourceView* G3_GetIndexSpriteSRV(CIndexSprite* pSprite, bool bRaw,
                                               int* pW, int* pH)
{
	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	if (w <= 0 || h <= 0)
		return NULL;

	D3D11SpriteTextureCache& cache = D3D11SpriteTextureCache::Instance();
	const unsigned int idxHash = bRaw ? 0u : G3_IndexValueHash();
	const void* key = bRaw
		? (const void*)((const BYTE*)pSprite + (size_t)0x20000000u)
		: (const void*)((const BYTE*)pSprite + (((size_t)idxHash & 0x3FFu) << 20));
	const unsigned int gen = G3_IndexContentHash(pSprite) ^ idxHash ^ (bRaw ? 0x52415700u : 0u);

	ID3D11ShaderResourceView* pSRV = cache.Lookup(key, gen, pW, pH);
	if (pSRV != NULL)
		return pSRV;

	std::vector<DWORD> rgba((size_t)w * h, 0u);
	for (int y = 0; y < h; ++y)
	{
		const WORD* pPixels = pSprite->GetPixelLine((WORD)y);
		if (pPixels == NULL)
			continue;
		DWORD* pDst = &rgba[(size_t)y * w];

		int transPair = (int)*pPixels++;
		int x = 0;
		while (transPair-- > 0)
		{
			x += (int)*pPixels++;                 // seffaf atlama
			int idxCount = (int)*pPixels++;       // index pikselleri
			for (int i = 0; i < idxCount && x < w; ++i, ++x)
			{
				const WORD iw = pPixels[i];
				if (bRaw)
				{
					// Legacy BltEffect paritesi: idxWord dogrudan 565 muamelesi
					pDst[x] = G1_565ToRGBA(iw);
				}
				else
				{
					const int slot = (iw >> 8) & 0xFF;
					const int grad = iw & 0xFF;
					const int cs = CIndexSprite::GetUsingColorSet((BYTE)slot);
					if (cs >= 0 && cs < MAX_COLORSET && grad < MAX_COLORGRADATION)
						pDst[x] = G1_565ToRGBA(CIndexSprite::ColorSet[cs][grad]);
				}
			}
			pPixels += idxCount;
			int colorCount = (int)*pPixels++;     // ham 565 renkler
			for (int i = 0; i < colorCount && x < w; ++i, ++x)
				pDst[x] = G1_565ToRGBA(pPixels[i]);
			pPixels += colorCount;
		}
	}

	pSRV = cache.InsertRGBA(key, gen, &rgba[0], w, h);
	if (pSRV != NULL)
	{
		if (pW) *pW = w;
		if (pH) *pH = h;
	}
	return pSRV;
}

bool G3_TryBltIndexSprite(void* lpSurface,
                          int clipL, int clipT, int clipR, int clipB,
                          const POINT* pPoint, CIndexSprite* pSprite)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	int texW = 0, texH = 0;
	ID3D11ShaderResourceView* pSRV = G3_GetIndexSpriteSRV(pSprite, false, &texW, &texH);
	if (pSRV == NULL)
		return false;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
	                       w, h, w, h, pSRV, texW, texH,
	                       0xFFFFFFFFu, D3D11QuadPipeline::BLEND_ALPHA);
}

bool G3_TryBltIndexSpriteEffect(void* lpSurface,
                                int clipL, int clipT, int clipR, int clipB,
                                const POINT* pPoint, CIndexSprite* pSprite)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	D3D11QuadPipeline::BlendMode blend;
	if (!G3_EffectToBlend((int)CSpriteSurface::s_CurrentEffect, &blend))
		return false;   // gray/gradation/digerleri index sprite'ta CPU'da

	int texW = 0, texH = 0;
	ID3D11ShaderResourceView* pSRV = G3_GetIndexSpriteSRV(pSprite, true, &texW, &texH);
	if (pSRV == NULL)
		return false;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
	                       w, h, w, h, pSRV, texW, texH, 0xFFFFFFFFu, blend);
}

//----------------------------------------------------------------------
// Faz G8 (2026-09-03) — BltSpriteAlphaFilter GPU yolu ("yarim siyah obje"
// fix'i).
//
// Legacy semantik (CSprite::memcpyAlphaFilter aktif kodu): filter deger
// haritasi bir MASKE'dir — filter!=0 olan pikselde sprite OPAK kopyalanir,
// filter==0 olan pikselde dest'e DOKUNULMAZ (oyuncu cevresindeki delik).
// Filter siniri disindaki sprite pikselleri her zaman opak cizilir.
//
// dx11gpu'da bu yol CPU'da kalinca kok problem su: oyuncu bir objenin
// arkasina gecince obje GPU quad yolundan (BltSprite hook) bu CPU yoluna
// duser; icerik composition'a yazilir ama obje sabit oldugu icin sonraki
// frame'lerde pikseller degismez -> diff segment cikmaz -> obje yalniz
// BASELINE'da kalir ve baseline'in USTUNE cizilen world tile quad'lari
// onu orter (gecede karartilmis tile ~= siyah). Diff'e yakalanan kismi
// bolgeler ustte kalir -> "yarisi siyah yarisi obje". Cozum: maske
// semantigi birebir texture alpha'sina islenip quad olarak SIRALI komut
// listesine girer — obje her frame tile'larin ustunde, dogru katmanda.
//
// Cache: anahtar sprite adresinden ayri bolgeye kaydirilir (plain G1
// entry'siyle cakismasin); filter pozisyonu/icerigi generation'a girer.
// Oyuncu hareket ettikce ayni anahtar yeni gen ile REPLACE edilir
// (entry birikimi yok; strip texture'lari kucuk, decode ucuz).
//----------------------------------------------------------------------
inline unsigned int G8_FilterStateHash(const CFilter* pFilter, short fx, short fy)
{
	unsigned int h = 2166136261u;
	h = (h ^ 0x41465800u) * 16777619u;   // 'AFX' etiketi
	h = (h ^ (unsigned int)(unsigned short)fx) * 16777619u;
	h = (h ^ (unsigned int)(unsigned short)fy) * 16777619u;
	const int fw = (int)pFilter->GetWidth();
	const int fh = (int)pFilter->GetHeight();
	h = (h ^ (unsigned int)fw) * 16777619u;
	h = (h ^ (unsigned int)fh) * 16777619u;
	// Icerik ornekleme: filter oyuncu shape'inden uretilir, animasyonla
	// degisebilir — 8 satirdan 4'er nokta hash'e girer (tam tarama yerine
	// ucuz ornekleme; pozisyon degisimi zaten dominant sinyal).
	const int stepY = (fh > 8) ? (fh / 8) : 1;
	const int stepX = (fw > 4) ? (fw / 4) : 1;
	for (int y = 0; y < fh; y += stepY)
	{
		const BYTE* pRow = pFilter->GetFilter((WORD)y);
		if (pRow == NULL)
			continue;
		for (int x = 0; x < fw; x += stepX)
			h = (h ^ (unsigned int)pRow[x]) * 16777619u;
	}
	return h;
}

ID3D11ShaderResourceView* G8_GetSpriteAlphaFilterSRV(CSprite* pSprite,
                                                     int* pW, int* pH)
{
	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	if (w <= 0 || h <= 0)
		return NULL;

	CFilter* pFilter = CSprite::GetFilter();
	if (pFilter == NULL || pFilter->IsNotInit())
		return NULL;

	const short fx = CSprite::s_X;
	const short fy = CSprite::s_Y;
	const int fw = (int)pFilter->GetWidth();
	const int fh = (int)pFilter->GetHeight();

	D3D11SpriteTextureCache& cache = D3D11SpriteTextureCache::Instance();
	// Anahtar plain G1 entry'sinden ayri bolgede (0x30000000 ofset)
	const void* key = (const void*)((const BYTE*)pSprite + (size_t)0x30000000u);
	const unsigned int gen = G1_SpriteGeneration(pSprite)
	                         ^ G8_FilterStateHash(pFilter, fx, fy);

	ID3D11ShaderResourceView* pSRV = cache.Lookup(key, gen, pW, pH);
	if (pSRV != NULL)
		return pSRV;

	// Miss — G1 RLE decode + maske: filter kapsaminda ve degeri 0 olan
	// pikseller seffaf birakilir (legacy "if (*pFilter) *pDest = *pSource"
	// birebir karsiligi), kapsam disi ve filter!=0 pikseller opak.
	std::vector<DWORD> rgba((size_t)w * h, 0u);
	for (int y = 0; y < h; ++y)
	{
		const WORD* pPixels = pSprite->GetPixelLine((WORD)y);
		if (pPixels == NULL)
			continue;
		DWORD* pDst = &rgba[(size_t)y * w];

		const int fyi = y - (int)fy;   // filter satiri (sprite satiri - s_Y)
		const BYTE* pFRow = (fyi >= 0 && fyi < fh)
		                    ? pFilter->GetFilter((WORD)fyi) : NULL;

		int count = (int)*pPixels++;
		int x = 0;
		while (count-- > 0)
		{
			x += (int)*pPixels++;               // seffaf atlama
			int colorCount = (int)*pPixels++;   // opak pikseller
			for (int i = 0; i < colorCount && x < w; ++i, ++x)
			{
				if (pFRow != NULL)
				{
					const int fxi = x - (int)fx;
					if (fxi >= 0 && fxi < fw && pFRow[fxi] == 0)
						continue;   // delik: dest korunur (alpha 0 kalir)
				}
				pDst[x] = G1_565ToRGBA(pPixels[i]);
			}
			pPixels += colorCount;
		}
	}

	pSRV = cache.InsertRGBA(key, gen, &rgba[0], w, h);
	if (pSRV != NULL)
	{
		if (pW) *pW = w;
		if (pH) *pH = h;
	}
	return pSRV;
}

bool G8_TryBltSpriteAlphaFilter(void* lpSurface,
                                int clipL, int clipT, int clipR, int clipB,
                                const POINT* pPoint, CSprite* pSprite)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	int texW = 0, texH = 0;
	ID3D11ShaderResourceView* pSRV = G8_GetSpriteAlphaFilterSRV(pSprite, &texW, &texH);
	if (pSRV == NULL)
		return false;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
	                       w, h, w, h, pSRV, texW, texH,
	                       0xFFFFFFFFu, D3D11QuadPipeline::BLEND_ALPHA);
}

} // namespace


//----------------------------------------------------------------------
// Static member
//----------------------------------------------------------------------
int	CSpriteSurface::s_Value1 = 1;
int	CSpriteSurface::s_Value2 = 31;
int	CSpriteSurface::s_Value3 = 1;

// memcpy Effect
// Faz G3 — SetEffect ile guncellenen aktif effect kimligi (GPU esleme icin)
enum CSpriteSurface::FUNCTION_EFFECT CSpriteSurface::s_CurrentEffect = CSpriteSurface::EFFECT_DARKER;
// Faz G7 (2026-09-03) — SetPalEffect ile senkron aktif pal-effect kimligi
enum CSpriteSurface::FUNCTION_EFFECT CSpriteSurface::s_CurrentPalEffect = CSpriteSurface::EFFECT_DARKER;

FUNCTION_MEMCPYEFFECT 	CSpriteSurface::s_pMemcpyEffectFunction = memcpyEffectDarker;
FUNCTION_MEMCPYEFFECT	CSpriteSurface::s_pMemcpyEffectFunctionTable[MAX_EFFECT] =
{
	memcpyEffectDarker,
	memcpyEffectGrayScale,
	memcpyEffectLighten,
	memcpyEffectDarken,
	memcpyEffectColorDodge,
	memcpyEffectScreen,
	memcpyEffectDodgeBurn,
	memcpyEffectDifferent,
	memcpyEffectGradation,
	memcpyEffectSimpleOutline,
	memcpyEffectWipeOut,
	memcpyEffectNet,
	memcpyEffectGrayScaleVarious,
	memcpyEffectScreenAlpha,
};

FUNCTION_MEMCPYPALEFFECT 	CSpriteSurface::s_pMemcpyPalEffectFunction = memcpyPalEffectDarker;
FUNCTION_MEMCPYPALEFFECT	CSpriteSurface::s_pMemcpyPalEffectFunctionTable[MAX_EFFECT] =
{
	memcpyPalEffectDarker,
	memcpyPalEffectGrayScale,
	memcpyPalEffectLighten,
	memcpyPalEffectDarken,
	memcpyPalEffectColorDodge,
	memcpyPalEffectScreen,
	memcpyPalEffectDodgeBurn,
	memcpyPalEffectDifferent,
	memcpyPalEffectGradation,
	memcpyPalEffectSimpleOutline,
	memcpyPalEffectWipeOut,
	memcpyPalEffectNet,
	memcpyPalEffectGrayScaleVarious,
	memcpyPalEffectScreenAlpha,
};

WORD		CSpriteSurface::s_EffectScreenTableR[32][32];
WORD		CSpriteSurface::s_EffectScreenTableG[32][32];
WORD		CSpriteSurface::s_EffectScreenTableB[32][32];


//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CSpriteSurface::CSpriteSurface()
{
}

CSpriteSurface::~CSpriteSurface()
{

}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// VLine
//----------------------------------------------------------------------
// 세로 선 그리기 
//----------------------------------------------------------------------
void
CSpriteSurface::VLine(int x, int y, int length, WORD color)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x+1;
	rect.bottom = y+length;

	FillRect(&rect, color);
}

//----------------------------------------------------------------------
// HLine
//----------------------------------------------------------------------
// 가로선 그리기 
//----------------------------------------------------------------------
void
CSpriteSurface::HLine(int x, int y, int length, WORD color)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x+length;
	rect.bottom = y+1;

	FillRect(&rect, color);
}

//----------------------------------------------------------------------
// DrawRect
//----------------------------------------------------------------------
// 속이 빈 상자 그리기 
//----------------------------------------------------------------------
void
CSpriteSurface::DrawRect(RECT* pRect, WORD color)
{
	if (pRect->left > pRect->right) { int temp=pRect->left; pRect->left=pRect->right; pRect->right=temp; }
	if (pRect->top > pRect->bottom) { int temp=pRect->top; pRect->top=pRect->bottom; pRect->bottom=temp; }

	int width = abs(pRect->right - pRect->left)+1,
		height = abs(pRect->bottom - pRect->top)+1;

	HLine(pRect->left, pRect->top, width, color);
	HLine(pRect->left, pRect->bottom, width, color);

	VLine(pRect->left, pRect->top, height, color);
	VLine(pRect->right, pRect->top, height, color);
}

//----------------------------------------------------------------------
// 직선 그리기 
//----------------------------------------------------------------------
void
CSpriteSurface::Line(int x1, int y1, int x2, int y2, WORD color)
{
	// Faz G1 (2026-09-01) - dx11gpu dirty (dogru bounding box)
	G1_Mark(m_ddsd.lpSurface,
	        (x1 < x2) ? x1 : x2, (y1 < y2) ? y1 : y2,
	        ((x1 < x2) ? (x2 - x1) : (x1 - x2)) + 1,
	        ((y1 < y2) ? (y2 - y1) : (y1 - y2)) + 1);

	// 한 점씩 찍어서 직선을 그린다.	
	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	lPitch		= m_ddsd.lPitch;

	WORD* lpSurfaceTemp;

	//
	// Bresenham algolithm
	//
	int x, y;
	int d, dx, dy;
	int Aincr, Bincr, xincr, yincr;
	int t;

    if (abs(x2 - x1) > abs(y2 - y1)) 
    {
        if (x1 > x2)
        {          
            t = x1;
            x1 = x2;
            x2 = t;
            t = y1;
            y1 = y2;
            y2 = t;
        }

        if (y2 > y1)
			yincr = 1;
        else 
			yincr = -1;

        dx = x2 - x1;
        dy = abs(y2 - y1);
        d = 2 * dy - dx;
        Aincr = 2 * (dy - dx);
        Bincr = 2 * dy;

        x = x1;
        y = y1;
        
		// PutPixel(x,y, color)
		if(x > m_ClipLeft && x < m_ClipRight && y > m_ClipTop && y < m_ClipBottom)
		{
			lpSurfaceTemp = (WORD*)((BYTE*)lpSurface + lPitch*y + (x<<1));
			*lpSurfaceTemp = color;
		}

        for (x++; x <= x2; x++)
        {
			if (d >= 0) 
            {
                y += yincr;
                d += Aincr;
            }
            else 
				d += Bincr;

            // PutPixel(x,y, color)
			if(x > m_ClipLeft && x < m_ClipRight && y > m_ClipTop && y < m_ClipBottom)
			{
				lpSurfaceTemp = (WORD*)((BYTE*)lpSurface + lPitch*y + (x<<1));
				*lpSurfaceTemp = color;
			}
        }            
    }    
	else
    {
        if (y1 > y2)
        {          
            t = x1;
            x1 = x2;
            x2 = t;
            t = y1;
            y1 = y2;
            y2 = t;
        }

        if (x2 > x1)
            xincr = 1;
        else 
			xincr = -1;
		
        dx = abs(x2 - x1);
        dy = y2 - y1;
        d = 2*dx - dy;
        Aincr = 2 * (dx - dy);
        Bincr = 2 * dx;
        x = x1;
        y = y1;

        // PutPixel(x,y, color)
		if(x > m_ClipLeft && x < m_ClipRight && y > m_ClipTop && y < m_ClipBottom)
		{
	        lpSurfaceTemp = (WORD*)((BYTE*)lpSurface + lPitch*y + (x<<1));
			*lpSurfaceTemp = color;
		}

        for (y++; y <= y2; y++)
        {
            if (d >= 0)
            {
                x += xincr;
                d += Aincr;
            }
            else
				d += Bincr;

            // PutPixel(x,y, color)
			if(x > m_ClipLeft && x < m_ClipRight && y > m_ClipTop && y < m_ClipBottom)
			{
				lpSurfaceTemp = (WORD*)((BYTE*)lpSurface + lPitch*y + (x<<1));
				*lpSurfaceTemp = color;
			}
        }  
    }
}

//----------------------------------------------------------------------
// Rect영역을 읽어서 Point위치에 찍을 경우
// 화면 크기를 넘지 않도록 좌표를 다시 설정해준다.
//----------------------------------------------------------------------
bool
CSpriteSurface::ClippingRectToPoint(RECT*& pRect, POINT*& pPoint)
{
	// 영역을 넘어가는지에 대한 체크
	if (pPoint->x>=(int)m_ClipRight || pPoint->y>=(int)m_ClipBottom)		
		return false;

	// width clipping
	if (pPoint->x<m_ClipLeft) 
	{
		pRect->left = pRect->left-pPoint->x + m_ClipLeft; //abs(pPoint->x);
		pPoint->x = m_ClipLeft;
	} 
	else if ((int)m_ClipRight-pPoint->x < pRect->right-pRect->left) 
	{
		pRect->right = pRect->left+ m_ClipRight-pPoint->x;
	}

	// height clipping
	if (pPoint->y<m_ClipTop) 
	{
		pRect->top = pRect->top-pPoint->y + m_ClipTop;
		pPoint->y = m_ClipTop;
	} 
	else if ((int)m_ClipBottom-pPoint->y < pRect->bottom-pRect->top) 
	{
		pRect->bottom = pRect->top+ m_ClipBottom-pPoint->y;
	}

	// 화면 영역을 넘어가면 안된다->
	if (pRect->left >= pRect->right || pRect->top >= pRect->bottom)
		return false;

	return true;
}


//----------------------------------------------------------------------
// Blt Darkness
//----------------------------------------------------------------------
// SourceSurface를 DarkBits만큼 어둡게 만들어서 *this에 출력
//----------------------------------------------------------------------
void
CSpriteSurface::BltHalf(POINT* pPoint, 
							   CDirectDrawSurface* SourceSurface, RECT*  pRect)
{
	//------------------------
	// Clipping
	//------------------------
	// TargetSurface의 영역을 넘어가는지에 대한 체크
	if (pPoint->x>=(int)m_ClipRight || pPoint->y>=(int)m_ClipBottom) 
		return;

	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pRect->right - pRect->left, pRect->bottom - pRect->top);

	POINT pt = *pPoint;

	// width clipping
	if (pt.x<m_ClipLeft) 
	{
		pRect->left = pRect->left-pt.x + m_ClipLeft;
		pt.x = m_ClipLeft;
	} 
	else if ((int)m_ClipRight-pt.x < pRect->right-pRect->left) 
	{
		pRect->right = pRect->left+ m_ClipRight-pt.x;
	}

	// height clipping
	if (pt.y<m_ClipTop) 
	{
		pRect->top = pRect->top-pt.y + m_ClipTop;
		pt.y = m_ClipTop;
	} 
	else if ((int)m_ClipBottom-pt.y < pRect->bottom-pRect->top) 
	{
		pRect->bottom = pRect->top+ m_ClipBottom-pt.y;
	}
	
	
	void*	lpSurfaceTemp	= m_ddsd.lpSurface;
	long	lPitchTemp		= m_ddsd.lPitch;

	WORD*	lpTargetSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	lTargetPitch	= m_ddsd.lPitch;

	// 두 Surface에 Lock을 건다.	
	SourceSurface->Lock();

	WORD*	lpSourceSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	lSourcePitch	= m_ddsd.lPitch;

	lpTargetSurface = (WORD*)((BYTE*)lpTargetSurface + pt.y*lTargetPitch) + pt.x;
	lpSourceSurface = (WORD*)((BYTE*)lpSourceSurface + pRect->top*lSourcePitch) + pRect->left;

	int	len = pRect->right - pRect->left + 1;
	for (int i=pRect->top; i<pRect->bottom; i++)
	{
		memcpyHalf(lpTargetSurface, lpSourceSurface, len);
		
		lpTargetSurface = (WORD*)((BYTE*)lpTargetSurface + lTargetPitch);
		lpSourceSurface = (WORD*)((BYTE*)lpSourceSurface + lSourcePitch);
	}

	// unlock
	//Unlock();
	
	SourceSurface->Unlock();
	m_ddsd.lpSurface	= lpSurfaceTemp;
	m_ddsd.lPitch		= lPitchTemp;
}


//----------------------------------------------------------------------
// Blt Darkness
//----------------------------------------------------------------------
// SourceSurface를 DarkBits만큼 어둡게 만들어서 *this에 출력
//----------------------------------------------------------------------
void
CSpriteSurface::BltDarkness(POINT* pPoint, 
							   CDirectDrawSurface* SourceSurface, RECT*  pRect,
							   BYTE DarkBits)
{
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pRect->right - pRect->left, pRect->bottom - pRect->top);

	//------------------------
	// Clipping
	//------------------------
	// TargetSurface의 영역을 넘어가는지에 대한 체크
	if (pPoint->x>=(int)m_ClipRight || pPoint->y>=(int)m_ClipBottom) 
		return;

	POINT pt = *pPoint;

	// width clipping
	if (pt.x<m_ClipLeft) 
	{
		pRect->left = pRect->left-pt.x + m_ClipLeft;
		pt.x = m_ClipLeft;
	} 
	else if ((int)m_ClipRight-pt.x < pRect->right-pRect->left) 
	{
		pRect->right = pRect->left+ m_ClipRight-pt.x;
	}

	// height clipping
	if (pt.y<m_ClipTop) 
	{
		pRect->top = pRect->top-pt.y + m_ClipTop;
		pt.y = m_ClipTop;
	} 
	else if ((int)m_ClipBottom-pt.y < pRect->bottom-pRect->top) 
	{
		pRect->bottom = pRect->top+ m_ClipBottom-pt.y;
	}
	
	
	void*	lpSurfaceTemp	= m_ddsd.lpSurface;
	long	lPitchTemp		= m_ddsd.lPitch;

	WORD*	lpTargetSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	lTargetPitch	= m_ddsd.lPitch;

	// 두 Surface에 Lock을 건다.	
	SourceSurface->Lock();

	WORD*	lpSourceSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	lSourcePitch	= m_ddsd.lPitch;

	lpTargetSurface = (WORD*)((BYTE*)lpTargetSurface + pt.y*lTargetPitch) + pt.x;
	lpSourceSurface = (WORD*)((BYTE*)lpSourceSurface + pRect->top*lSourcePitch) + pRect->left;

	s_Value1 = DarkBits;
	int	len = pRect->right - pRect->left + 1;
	for (int i=pRect->top; i<pRect->bottom; i++)
	{
		memcpyDarkness(lpTargetSurface, lpSourceSurface, len);
		
		lpTargetSurface = (WORD*)((BYTE*)lpTargetSurface + lTargetPitch);
		lpSourceSurface = (WORD*)((BYTE*)lpSourceSurface + lSourcePitch);
	}

	// unlock
	//Unlock();
	//SourceSurface->Unlock();
	m_ddsd.lpSurface	= lpSurfaceTemp;
	m_ddsd.lPitch		= lPitchTemp;
}

//----------------------------------------------------------------------
// Blt Darkness
//----------------------------------------------------------------------
// SourceSurface를 DarkBits만큼 어둡게 만들어서 *this에 출력
//----------------------------------------------------------------------
void
CSpriteSurface::BltBrightness(POINT* pPoint, 
							   CDirectDrawSurface* SourceSurface, RECT*  pRect,
							   BYTE BrightBits)
{
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pRect->right - pRect->left, pRect->bottom - pRect->top);

	//------------------------
	// Clipping
	//------------------------
	// TargetSurface의 영역을 넘어가는지에 대한 체크
	if (pPoint->x>=(int)m_ClipRight || pPoint->y>=(int)m_ClipBottom) 
		return;

	POINT pt = *pPoint;

	// width clipping
	if (pt.x<m_ClipLeft) 
	{
		pRect->left = pRect->left-pt.x + m_ClipLeft;
		pt.x = m_ClipLeft;
	} 
	else if ((int)m_ClipRight-pt.x < pRect->right-pRect->left) 
	{
		pRect->right = pRect->left+ m_ClipRight-pt.x;
	}

	// height clipping
	if (pt.y<m_ClipTop) 
	{
		pRect->top = pRect->top-pt.y + m_ClipTop;
		pt.y = m_ClipTop;
	} 
	else if ((int)m_ClipBottom-pt.y < pRect->bottom-pRect->top) 
	{
		pRect->bottom = pRect->top+ m_ClipBottom-pt.y;
	}
	
			
	void*	lpSurfaceTemp	= m_ddsd.lpSurface;
	long	lPitchTemp		= m_ddsd.lPitch;

	WORD*	lpTargetSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	lTargetPitch	= m_ddsd.lPitch;

	// 두 Surface에 Lock을 건다.	
	SourceSurface->Lock();

	WORD*	lpSourceSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	lSourcePitch	= m_ddsd.lPitch;

	lpTargetSurface = (WORD*)((BYTE*)lpTargetSurface + pt.y*lTargetPitch) + pt.x;
	lpSourceSurface = (WORD*)((BYTE*)lpSourceSurface + pRect->top*lSourcePitch) + pRect->left;

	s_Value1 = BrightBits;
	int	len = pRect->right - pRect->left + 1;
	for (int i=pRect->top; i<pRect->bottom; i++)
	{
		memcpyBrightness(lpTargetSurface, lpSourceSurface, len);
		
		lpTargetSurface = (WORD*)((BYTE*)lpTargetSurface + lTargetPitch);
		lpSourceSurface = (WORD*)((BYTE*)lpSourceSurface + lSourcePitch);
	}

	// unlock
	//Unlock();
	//SourceSurface->Unlock();
	m_ddsd.lpSurface	= lpSurfaceTemp;
	m_ddsd.lPitch		= lPitchTemp;
}

//----------------------------------------------------------------------
// Blt DarknessFilter
//----------------------------------------------------------------------
// SourceSurface를 Filter로 생각해서
// 현재 Surface의 각 점(R,G,B)에서 
// SourceSurface의 각 점(R,G,B) 만큼을 빼주어서 어둡게 만든다.
//----------------------------------------------------------------------
void
CSpriteSurface::BltDarknessFilter(POINT* pPoint, 
							   CDirectDrawSurface* SourceSurface, RECT*  pRect,
							   WORD TransColor)
{
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pRect->right - pRect->left, pRect->bottom - pRect->top);

	//------------------------
	// Clipping
	//------------------------
	// TargetSurface의 영역을 넘어가는지에 대한 체크
	if (pPoint->x>=(int)m_ClipRight || pPoint->y>=(int)m_ClipBottom) 
		return;

	POINT pt = *pPoint;
	
	// width clipping
	if (pt.x<m_ClipLeft) 
	{
		pRect->left = pRect->left-pt.x + m_ClipLeft;
		pt.x = m_ClipLeft;
	} 
	else if ((int)m_ClipRight-pt.x < pRect->right-pRect->left) 
	{
		pRect->right = pRect->left+ m_ClipRight-pt.x;
	}

	// height clipping
	if (pt.y<m_ClipTop) 
	{
		pRect->top = pRect->top-pt.y + m_ClipTop;
		pt.y = m_ClipTop;
	} 
	else if ((int)m_ClipBottom-pt.y < pRect->bottom-pRect->top) 
	{
		pRect->bottom = pRect->top+ m_ClipBottom-pt.y;
	}


	
	
	WORD	*lpTargetSurface, *lpSourceSurface,
			*lpTargetTemp, *lpSourceTemp;
	WORD	lTargetPitch, lSourcePitch;

	// 두 Surface에 Lock을 건다.
	LockW(lpTargetSurface, lTargetPitch);
	SourceSurface->LockW(lpSourceSurface, lSourcePitch);

	lpTargetSurface = (WORD*)((BYTE*)lpTargetSurface + pt.y*lTargetPitch) + pt.x;
	lpSourceSurface = (WORD*)((BYTE*)lpSourceSurface + pRect->top*lSourcePitch) + pRect->left;

	//------------------------------------------------------------	
	// Darker : BG - SPR_Filter
	//------------------------------------------------------------	
	BYTE	filter;

	for (int i=pRect->top; i<pRect->bottom; i++)
	{
		lpTargetTemp = lpTargetSurface;
		lpSourceTemp = lpSourceSurface;

		for (int j=pRect->left; j<pRect->right; j++)
		{	
			// 투명색이 아닐때 출력
			if (*lpTargetTemp != TransColor)
			{				
				filter = *lpSourceTemp & 0x1F;	// Blue값

				*lpTargetTemp = 
					(((((*lpTargetTemp >> 11) & 0x1F) * filter) >> 5) << 11)
					| (((((*lpTargetTemp >> 5) & 0x3F) * (filter<<1)) >> 6) << 5)
					| (((*lpTargetTemp & 0x1F) * filter) >> 5);
			}
			
			lpTargetTemp ++;
			lpSourceTemp ++;
		}
		
		lpTargetSurface = (WORD*)((BYTE*)lpTargetSurface + lTargetPitch);
		lpSourceSurface = (WORD*)((BYTE*)lpSourceSurface + lSourcePitch);
	}

	// unlock
	Unlock();
	SourceSurface->Unlock();
}

//----------------------------------------------------------------------
// Blt SpriteOutline
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteOutline(CSpriteOutlineManager* pSOM, WORD color)
{
	POINT	point = pSOM->GetPosition();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (point.x >= m_ClipRight || point.y >= m_ClipBottom)
		return;

	// Faz G1 (2026-09-01) - dx11gpu dirty (outline bolgesi degisken: tum ekran)
	G1_MarkRect(m_ddsd.lpSurface, NULL);

	//WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	//WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSOM->GetWidth();
	rect.bottom = pSOM->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= point.x + pSOM->GetWidth();
	int yPlusSpriteHeight	= point.y + pSOM->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (point.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (point.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -point.x + m_ClipLeft;		
		point.x = m_ClipLeft;
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - point.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - point.y;					
				}				
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - point.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			//pSOM->BltClipWidth( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
			pSOM->BltClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
				}				
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
				}				
			}

			// 왼쪽 Clip			
			//pSOM->BltClipLeft( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
			pSOM->BltClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - point.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (point.x<<1));
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - point.y;
				}				
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*Pitch + (point.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - point.y;										
				}								
			}	
			
			// 오른쪽 Clip			
			//pSOM->BltClipRight( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
			pSOM->BltClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (point.x<<1));
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
				}
				
				// Height만 Clip
				//pSOM->BltClipHeight( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
				pSOM->BltClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*Pitch + (point.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
					// Height만 Clip
					//pSOM->BltClipHeight( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
					pSOM->BltClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
				}
				else
				{
					// No Clip					
					pSOM->Blt( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt SpriteOutline Only
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteOutlineOnly(CSpriteOutlineManager* pSOM, WORD color)
{
	POINT	point = pSOM->GetPosition();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (point.x >= m_ClipRight || point.y >= m_ClipBottom)
		return;

	// Faz G1 (2026-09-01) - dx11gpu dirty (outline bolgesi degisken: tum ekran)
	G1_MarkRect(m_ddsd.lpSurface, NULL);


	//WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	//WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSOM->GetWidth();
	rect.bottom = pSOM->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= point.x + pSOM->GetWidth();
	int yPlusSpriteHeight	= point.y + pSOM->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (point.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (point.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -point.x + m_ClipLeft;		
		point.x = m_ClipLeft;
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - point.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - point.y;					
				}				
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - point.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			//pSOM->BltOutlineClipWidth( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
			pSOM->BltOutlineClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
				}				
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
				}				
			}

			// 왼쪽 Clip			
			//pSOM->BltOutlineClipLeft( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
			pSOM->BltOutlineClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - point.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (point.x<<1));
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - point.y;
				}				
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*Pitch + (point.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - point.y;										
				}								
			}	
			
			// 오른쪽 Clip			
			//pSOM->BltOutlineClipRight( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
			pSOM->BltOutlineClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (point.x<<1));
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
				}
				
				// Height만 Clip
				//pSOM->BltOutlineClipHeight( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
				pSOM->BltOutlineClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*Pitch + (point.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
					// Height만 Clip
					//pSOM->BltOutlineClipHeight( (WORD*)m_ddsd.lpSurface, Pitch, color, &rect);
					pSOM->BltOutlineClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
				}
				else
				{
					// No Clip					
					pSOM->BltOutline( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt SpriteOutline
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteOutlineDarkness(CSpriteOutlineManager* pSOM, WORD color, BYTE DarkBits)
{
	POINT	point = pSOM->GetPosition();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (point.x >= m_ClipRight || point.y >= m_ClipBottom)
		return;

	// Faz G1 (2026-09-01) - dx11gpu dirty (outline bolgesi degisken: tum ekran)
	G1_MarkRect(m_ddsd.lpSurface, NULL);


	//WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	//WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSOM->GetWidth();
	rect.bottom = pSOM->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= point.x + pSOM->GetWidth();
	int yPlusSpriteHeight	= point.y + pSOM->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (point.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (point.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -point.x + m_ClipLeft;		
		point.x = m_ClipLeft;
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - point.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - point.y;					
				}				
			}
			else
			{
				//lpSurface = (WORD*)( (BYTE*)lpSurface + point.y*m_ddsd.lPitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - point.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			//pSOM->BltClipWidth( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
			pSOM->BltDarknessClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, DarkBits, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
				}				
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*m_ddsd.lPitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
				}				
			}

			// 왼쪽 Clip			
			//pSOM->BltClipLeft( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
			pSOM->BltDarknessClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, DarkBits, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - point.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (point.x<<1));
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - point.y;
				}				
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*m_ddsd.lPitch + (point.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - point.y;										
				}								
			}	
			
			// 오른쪽 Clip			
			//pSOM->BltClipRight( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
			pSOM->BltDarknessClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, DarkBits, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (point.x<<1));
				rect.top = -point.y + m_ClipTop;
				point.y = m_ClipTop;

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
				}
				
				// Height만 Clip
				//pSOM->BltClipHeight( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
				pSOM->BltDarknessClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, DarkBits, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + point.y*m_ddsd.lPitch + (point.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - point.y;					
					// Height만 Clip
					//pSOM->BltClipHeight( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, &rect);
					pSOM->BltDarknessClip( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, DarkBits, &rect);
				}
				else
				{
					// No Clip					
					pSOM->BltDarkness( (WORD*)m_ddsd.lpSurface, m_ddsd.lPitch, color, DarkBits);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Change Brightness
//----------------------------------------------------------------------
// m_pDDSurface의 pRect영역을 어둡게 한다. 
// DarkBits는 (0~5) bit단위로 1, 1/2, 1/4, 1/8, 1/16, 1/32, 0이다.
//----------------------------------------------------------------------
// pRect의 영역은 right, bottom의 좌표는 포함하지 않는다.  -1까지이다.
//----------------------------------------------------------------------
void	
CSpriteSurface::ChangeBrightnessBit(RECT*  pRect, BYTE DarkBits)
{
	// ClippingRect
	if (pRect->left < m_ClipLeft) pRect->left=m_ClipLeft;
	if ((WORD)pRect->right > m_ClipRight) pRect->right=m_ClipRight;

	if (pRect->top < m_ClipTop) pRect->top=m_ClipTop;
	if ((WORD)pRect->bottom > m_ClipBottom) pRect->bottom=m_ClipBottom;

	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu)
	G1_MarkRect(m_ddsd.lpSurface, pRect);

	// 영역을 벗어나는 경우
	if (pRect->left >= pRect->right || pRect->top >= pRect->bottom)
		return;


	QWORD	*lpTargetTemp;
	QWORD	*lpTargetSurface	= (QWORD*)m_ddsd.lpSurface;
	WORD	Pitch			= m_ddsd.lPitch;
	
	// 출력 시작 위치
	lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + pRect->top*Pitch + (pRect->left<<1));

	int i;
	int j;

	int rectBottom = pRect->bottom;

	int width = pRect->right - pRect->left;
	int qTimes = width >> 2;

	// Target Surface의 정해진 영역을 어둡게 한다.
	/*
	switch ( width & 0x03 )
	{
		//------------------
		// 4점씩
		//------------------
		case 0 :			
			for (int i =pRect->top; i<rectBottom; i++)
			{
				lpTargetTemp = lpTargetSurface;	

				// 네점씩 찍기
				for (int j =0; j<qTimes; j++)
				{
					*lpTargetTemp++ = (*lpTargetTemp >> DarkBits) & s_qwMASK_SHIFT[DarkBits];					
				}

				// 다음 줄
				lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + Pitch);
			}
		break;

		//------------------
		// 1점 + 4점씩
		//------------------
		case 1 :
			for (int i =pRect->top; i<rectBottom; i++)
			{
				lpTargetTemp = lpTargetSurface;	

				// 한점 찍기
				*((WORD*)lpTargetTemp) = 
					(*((WORD*)lpTargetTemp) >> DarkBits) & s_wMASK_SHIFT[DarkBits];
				
				lpTargetTemp = (QWORD*)((WORD*)lpTargetTemp + 1);

				// 네점씩 찍기
				for (int j =0; j<qTimes; j++)
				{
					*lpTargetTemp++ = (*lpTargetTemp >> DarkBits) & s_qwMASK_SHIFT[DarkBits];
				}

				// 다음 줄
				lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + Pitch);
			}
		break;

		//------------------
		// 2점 + 4점씩
		//------------------
		case 2 :
			for (int i =pRect->top; i<rectBottom; i++)
			{
				lpTargetTemp = lpTargetSurface;	

				// 두점 찍기
				*((DWORD*)lpTargetTemp) = (*((DWORD*)lpTargetTemp) >> DarkBits) & s_dwMASK_SHIFT[DarkBits];
				lpTargetTemp = (QWORD*)((DWORD*)lpTargetTemp + 1);

				// 네점씩 찍기
				for (int j =0; j<qTimes; j++)
				{
					*lpTargetTemp++ = (*lpTargetTemp >> DarkBits) & s_qwMASK_SHIFT[DarkBits];
				}

				// 다음 줄
				lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + Pitch);
			}
		break;

		//------------------
		// 1점 + 2점 + 4점씩
		//------------------
		case 3 :
			for (int i =pRect->top; i<rectBottom; i++)
			{
				lpTargetTemp = lpTargetSurface;	

				// 한점 찍기
				*((WORD*)lpTargetTemp) = 
					(*((WORD*)lpTargetTemp) >> DarkBits) & s_wMASK_SHIFT[DarkBits];
				lpTargetTemp = (QWORD*)((WORD*)lpTargetTemp + 1);

				// 두점 찍기
				*((DWORD*)lpTargetTemp) = (*((DWORD*)lpTargetTemp) >> DarkBits) & s_dwMASK_SHIFT[DarkBits];
				lpTargetTemp = (QWORD*)((DWORD*)lpTargetTemp + 1);

				// 네점씩 찍기
				for (int j =0; j<qTimes; j++)
				{
					*lpTargetTemp++ = (*lpTargetTemp >> DarkBits) & s_qwMASK_SHIFT[DarkBits];
				}

				// 다음 줄
				lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + Pitch);
			}
		break;
	}
	*/

///*
	// 흐름을 바꾼다.. 으흠..~	
	int maxWave = 8;		// 2, 4, 8, 16...

	if (maxWave >= width)
	{
		return;
	}

	int maxWave_1 = maxWave-1;
	
	int step = ((DarkBits & maxWave)? maxWave+maxWave_1-DarkBits : DarkBits);
	step = max( 0, min( maxWave_1, step ) );
	int stepInc = 1;	

	int height = pRect->bottom - pRect->top;
	int widthHalf = width>>1;
	int heightHalf = height >> 1;
	int width2 = maxWave;
	int width2Inc = width / heightHalf;
	int heightHalfValue = pRect->top + heightHalf;
	
	QWORD* pSourceTemp;

	for (int i =pRect->top; i<rectBottom; i++)
	{			
		// step만큼 오른쪽에서 출력한다. 
		// 4-step만큼은 덜 출력한다.
		int widthPlus = (width - width2)>>1;

		int newWidth = width2 - maxWave + step;
		int qTimes = newWidth >> 2;

		pSourceTemp = (QWORD*)((WORD*)lpTargetSurface + widthPlus + step);
		lpTargetTemp = (QWORD*)((WORD*)lpTargetSurface + widthPlus);
		
		switch (newWidth & 0x03)
		{
			//------------------
			// 4점씩
			//------------------
			case 0 :
				for (int j =0; j<qTimes; j++)
				{
					*lpTargetTemp++ = *pSourceTemp++;
				}
			break;			

			//------------------
			// 1점 + 4점씩
			//------------------
			case 1 :
				// 한점 찍기
				*((WORD*)lpTargetTemp) = *((WORD*)pSourceTemp);
				
				lpTargetTemp = (QWORD*)((WORD*)lpTargetTemp + 1);
				pSourceTemp = (QWORD*)((WORD*)pSourceTemp + 1);

				// 네점씩 찍기
				for (int j =0; j<qTimes; j++)
				{
					*lpTargetTemp++ = *pSourceTemp++;
				}
			break;

			//------------------
			// 2점 + 4점씩
			//------------------
			case 2 :
				// 두점 찍기
				*((DWORD*)lpTargetTemp) = *((DWORD*)pSourceTemp);
				lpTargetTemp = (QWORD*)((DWORD*)lpTargetTemp + 1);
				pSourceTemp = (QWORD*)((DWORD*)pSourceTemp + 1);

				// 네점씩 찍기
				for (int j =0; j<qTimes; j++)
				{
					*lpTargetTemp++ = *pSourceTemp++;
				}
			break;

			//------------------
			// 1점 + 2점 + 4점씩
			//------------------
			case 3 :			
				// 한점 찍기
				*((WORD*)lpTargetTemp) = *((WORD*)pSourceTemp);
				lpTargetTemp = (QWORD*)((WORD*)lpTargetTemp + 1);
				pSourceTemp = (QWORD*)((WORD*)pSourceTemp + 1);

				// 두점 찍기
				*((DWORD*)lpTargetTemp) = *((DWORD*)pSourceTemp);
				lpTargetTemp = (QWORD*)((DWORD*)lpTargetTemp + 1);
				pSourceTemp = (QWORD*)((DWORD*)pSourceTemp + 1);

				// 네점씩 찍기
				for (int j =0; j<qTimes; j++)
				{
					*lpTargetTemp++ = *pSourceTemp++;
				}
			break;
		}

		// 다음 줄
		lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + Pitch);

		step += stepInc;
		if (step < 0 || step >= maxWave)
		{
			if (step < 0)
			{
				step = 0;
				stepInc = 1;
			}
			else if (step >= maxWave)
			{
				step = maxWave_1;
				stepInc = -1;
			}
		}

		width2 += width2Inc;

		width2 = min( width, width2 );
		width2 = max( maxWave, width2 );

		if (i==heightHalfValue)
		{
			width2Inc = -width2Inc;
		}
	}
//	*/
}

//----------------------------------------------------------------------
// Blt Color Alpha
//----------------------------------------------------------------------
// 단색으로(color) Surface의 pRect영역을
// alpha값만큼 alpha blending한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltColorAlpha(RECT* pRect, WORD color, BYTE alpha2)
{
	// ClippingRect
	if (pRect->left < m_ClipLeft) pRect->left=m_ClipLeft;
	if ((WORD)pRect->right > m_ClipRight) pRect->right=m_ClipRight;

	if (pRect->top < m_ClipTop) pRect->top=m_ClipTop;
	if ((WORD)pRect->bottom > m_ClipBottom) pRect->bottom=m_ClipBottom;

	// 영역을 벗어나는 경우
	if (pRect->left >= pRect->right || pRect->top >= pRect->bottom)
		return;

	// Faz G1 - dx11gpu: tek renk alpha quad. Legacy agirlik: dest*alpha2/32 +
	// color*(32-alpha2)/32 -> straight alpha A=(32-alpha2)*255/32.
	{
		D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
		if (sc.IsCompositionTarget(m_ddsd.lpSurface) && sc.IsActive())
		{
			int a8 = ((32 - (int)alpha2) * 255) / 32;
			if (a8 > 255) a8 = 255;
			if (a8 < 0)   a8 = 0;
			int r = (color >> 11) & 0x1F;
			int g = (color >> 5)  & 0x3F;
			int b =  color        & 0x1F;
			r = (r << 3) | (r >> 2);
			g = (g << 2) | (g >> 4);
			b = (b << 3) | (b >> 2);
			if (sc.RecordSolidQuad(*pRect,
			        D3D11QuadPipeline::MakeColor((BYTE)r, (BYTE)g, (BYTE)b, (BYTE)a8),
			        D3D11QuadPipeline::BLEND_ALPHA))
				return;
		}
		G1_MarkRect(m_ddsd.lpSurface, pRect);
	}

	QWORD	*lpTargetTemp;
	QWORD	*lpTargetSurface	= (QWORD*)m_ddsd.lpSurface;
	WORD	Pitch			= m_ddsd.lPitch;
	
	// 출력 시작 위치
	lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + pRect->top*Pitch + (pRect->left<<1));

	int i;
	//int j;

	DWORD	srcColor2, destColor2;
	DWORD	first2, second2;

	QWORD	srcColor, destColor;
	QWORD	first, second;

	int rectBottom = pRect->bottom;

	int width = pRect->right - pRect->left;
	int qTimes = width >> 2;	//  / 4
	
	DWORD dwColor = ((DWORD)color << 16) | color;
	QWORD qwColor = ((QWORD)dwColor << 32) | dwColor;

	// 31 - alpha2
	BYTE alpha = 31 - alpha2;
		
	//-----------------------------------------------------
	// Surface의 정해진 영역을 alpha blending				
	//-----------------------------------------------------
	switch ( width & 0x03 )
	{
		//-----------------------------------------------------
		// 정확히 4의 배수가 되는 경우..
		//-----------------------------------------------------
		case 0 :
			for (int i =pRect->top; i<rectBottom; i++)
			{
				lpTargetTemp = lpTargetSurface;	

				int qTimesTemp = qTimes;

				//-----------------------------------
				// 4점씩...
				//-----------------------------------
				while (qTimesTemp--)
				{
					destColor = *lpTargetTemp;
				
					first   =   qwColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  alpha;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;
					
					second  =   qwColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  alpha;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;
					
					srcColor =  first | second;

					first   =   destColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  alpha2;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   destColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  alpha2;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					destColor=  first | second;
					
					*lpTargetTemp = srcColor + destColor;

					lpTargetTemp++;					
				}

				// 다음 줄
				lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + Pitch);
			}
		break;

		//-----------------------------------------------------
		// 1점 찍고  4점씩...
		//-----------------------------------------------------
		case 1 :
			for (int i =pRect->top; i<rectBottom; i++)
			{
				lpTargetTemp = lpTargetSurface;	

				//-----------------------------------
				// 한 점 찍기...
				//-----------------------------------
				destColor2 = *(WORD*)lpTargetTemp;

				first2   =   color & CDirectDraw::s_wMASK_ALPHA1;
				first2   >>= 5;
				first2   *=  alpha;
				first2   &=  CDirectDraw::s_wMASK_ALPHA1;

				second2  =   color & CDirectDraw::s_wMASK_ALPHA0;
				second2  *=  alpha;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_wMASK_ALPHA0;

				srcColor2 =  first2 | second2;

				first2   =   destColor2 & CDirectDraw::s_wMASK_ALPHA1;
				first2   >>= 5;
				first2   *=  alpha2;
				first2   &=  CDirectDraw::s_wMASK_ALPHA1;

				second2  =   destColor2 & CDirectDraw::s_wMASK_ALPHA0;
				second2  *=  alpha2;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_wMASK_ALPHA0;

				destColor2=  first2 | second2;

				*(WORD*)lpTargetTemp = (WORD)(srcColor2 + destColor2);		
					
				lpTargetTemp = (QWORD*)((WORD*)lpTargetTemp + 1);	


				int qTimesTemp = qTimes;
				
				//-----------------------------------
				// 4점씩..
				//-----------------------------------
				while (qTimesTemp--)
				{
					destColor = *lpTargetTemp;
				
					first   =   qwColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  alpha;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;
					
					second  =   qwColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  alpha;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;
					
					srcColor =  first | second;

					first   =   destColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  alpha2;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   destColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  alpha2;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					destColor=  first | second;
					
					*lpTargetTemp = srcColor + destColor;

					lpTargetTemp++;					
				}

				// 다음 줄
				lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + Pitch);
			}
		break;

		//-----------------------------------------------------
		// 2점찍고.. 4점씩...
		//-----------------------------------------------------
		case 2 :
			for (int i =pRect->top; i<rectBottom; i++)
			{
				lpTargetTemp = lpTargetSurface;	
			
				//-----------------------------------
				// 두 점 찍기..
				//-----------------------------------
				destColor2 = *(DWORD*)lpTargetTemp;

				first2   =   dwColor & CDirectDraw::s_dwMASK_ALPHA0;
				first2   >>= 5;
				first2   *=  alpha;
				first2   &=  CDirectDraw::s_dwMASK_ALPHA0;

				second2  =   dwColor & CDirectDraw::s_dwMASK_ALPHA1;
				second2  *=  alpha;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_dwMASK_ALPHA1;

				srcColor2 =  first2 | second2;

				first2   =   destColor2 & CDirectDraw::s_dwMASK_ALPHA0;
				first2   >>= 5;
				first2   *=  alpha2;
				first2   &=  CDirectDraw::s_dwMASK_ALPHA0;

				second2  =   destColor2 & CDirectDraw::s_dwMASK_ALPHA1;
				second2  *=  alpha2;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_dwMASK_ALPHA1;

				destColor2=  first2 | second2;

				*(DWORD*)lpTargetTemp = srcColor2 + destColor2;
					
				lpTargetTemp = (QWORD*)((DWORD*)lpTargetTemp + 1);


				int qTimesTemp = qTimes;
				
				//-----------------------------------
				// 4점씩..
				//-----------------------------------
				while (qTimesTemp--)
				{
					destColor = *lpTargetTemp;
				
					first   =   qwColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  alpha;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;
					
					second  =   qwColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  alpha;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;
					
					srcColor =  first | second;

					first   =   destColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  alpha2;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   destColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  alpha2;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					destColor=  first | second;
					
					*lpTargetTemp = srcColor + destColor;

					lpTargetTemp++;					
				}

				// 다음 줄
				lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + Pitch);
			}
		break;

		//-----------------------------------------------------
		// 1점 찍고 2점찍고.. 4점씩...
		//-----------------------------------------------------
		case 3 :
			for (int i =pRect->top; i<rectBottom; i++)
			{
				lpTargetTemp = lpTargetSurface;	

				//-----------------------------------
				// 한 점 찍기...
				//-----------------------------------
				destColor2 = *(WORD*)lpTargetTemp;

				first2   =   color & CDirectDraw::s_wMASK_ALPHA1;
				first2   >>= 5;
				first2   *=  alpha;
				first2   &=  CDirectDraw::s_wMASK_ALPHA1;

				second2  =   color & CDirectDraw::s_wMASK_ALPHA0;
				second2  *=  alpha;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_wMASK_ALPHA0;

				srcColor2 =  first2 | second2;

				first2   =   destColor2 & CDirectDraw::s_wMASK_ALPHA1;
				first2   >>= 5;
				first2   *=  alpha2;
				first2   &=  CDirectDraw::s_wMASK_ALPHA1;

				second2  =   destColor2 & CDirectDraw::s_wMASK_ALPHA0;
				second2  *=  alpha2;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_wMASK_ALPHA0;

				destColor2=  first2 | second2;

				*(WORD*)lpTargetTemp = (WORD)(srcColor2 + destColor2);		
					
				lpTargetTemp = (QWORD*)((WORD*)lpTargetTemp + 1);	

				//-----------------------------------
				// 두 점 찍기..
				//-----------------------------------
				destColor2 = *(DWORD*)lpTargetTemp;

				first2   =   dwColor & CDirectDraw::s_dwMASK_ALPHA0;
				first2   >>= 5;
				first2   *=  alpha;
				first2   &=  CDirectDraw::s_dwMASK_ALPHA0;

				second2  =   dwColor & CDirectDraw::s_dwMASK_ALPHA1;
				second2  *=  alpha;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_dwMASK_ALPHA1;

				srcColor2 =  first2 | second2;

				first2   =   destColor2 & CDirectDraw::s_dwMASK_ALPHA0;
				first2   >>= 5;
				first2   *=  alpha2;
				first2   &=  CDirectDraw::s_dwMASK_ALPHA0;

				second2  =   destColor2 & CDirectDraw::s_dwMASK_ALPHA1;
				second2  *=  alpha2;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_dwMASK_ALPHA1;

				destColor2=  first2 | second2;

				*(DWORD*)lpTargetTemp = srcColor2 + destColor2;
					
				lpTargetTemp = (QWORD*)((DWORD*)lpTargetTemp + 1);


				int qTimesTemp = qTimes;
				
				//-----------------------------------
				// 4점씩..
				//-----------------------------------
				while (qTimesTemp--)
				{
					destColor = *lpTargetTemp;
				
					first   =   qwColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  alpha;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;
					
					second  =   qwColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  alpha;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;
					
					srcColor =  first | second;

					first   =   destColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  alpha2;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   destColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  alpha2;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					destColor=  first | second;
					
					*lpTargetTemp = srcColor + destColor;

					lpTargetTemp++;					
				}

				// 다음 줄
				lpTargetSurface = (QWORD*)((BYTE*)lpTargetSurface + Pitch);
			}
		break;
	}
}

//----------------------------------------------------------------------
// Blt Sprite
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSprite(POINT* pPoint, CSprite* pSprite)
{
	// 2026-05-14: NULL guard'lar — D3D9 modunda CDirectDrawSurface compat-only
	// olabilir (m_pDDSurface + m_pRenderSurface NULL, Lock fail). Sprite codec
	// burada lpSurface'a WORD* cast ile yaziyor; m_ddsd.lpSurface NULL ise
	// segfault. DrawTitleLoading pSurface->Lock fail durumunda da check.
	if (!pSprite || !pPoint || m_ddsd.lpSurface == NULL)
		return;

	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G1 - dx11gpu: GPU quad yolu (166 call-site'in ana yolu).
	// Kayit basarisizsa CPU devam eder ve bolge dirty isaretlenir.
	if (G1_TryBltSprite(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
	                    m_ClipBottom, pPoint, pSprite, 0xFFFFFFFFu,
	                    D3D11QuadPipeline::BLEND_ALPHA))
		return;
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());

	POINT pt = {pPoint->x, pPoint->y};

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;			
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}	

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->Blt(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt Sprite NoClip
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteNoClip(POINT* pPoint, CSprite* pSprite)
{
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	// Faz G1 - dx11gpu GPU quad yolu
	if (G1_TryBltSprite(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
	                    m_ClipBottom, pPoint, pSprite, 0xFFFFFFFFu,
	                    D3D11QuadPipeline::BLEND_ALPHA))
		return;
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;

	// Lock
	//LockW(lpSurface, Pitch);

	// 출력 시작 위치			
	lpSurface = (WORD*)((BYTE*)lpSurface + pPoint->y*Pitch + (pPoint->x<<1));

	// 출력
	pSprite->Blt(lpSurface, Pitch);	

	// Unlock
	//Unlock();
}

//----------------------------------------------------------------------
// Blt SpriteHalf
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteHalf(POINT* pPoint, CSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G1 - dx11gpu: %50 blend = A=128 straight alpha (memcpyHalf paritesi)
	if (G1_TryBltSprite(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
	                    m_ClipBottom, pPoint, pSprite,
	                    D3D11QuadPipeline::MakeColor(255, 255, 255, 128),
	                    D3D11QuadPipeline::BLEND_ALPHA))
		return;
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());

	POINT pt = *pPoint;
	
	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltHalfClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltHalfClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltHalfClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltHalfClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltHalfClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltHalf(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt Sprite Alpha
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteAlpha(POINT* pPoint, CSprite* pSprite, BYTE alpha)
{
	// 2026-05-14: NULL guard'lar — bkz. BltSprite yorumu (D3D9 compat-only Lock fail).
	if (!pSprite || !pPoint || m_ddsd.lpSurface == NULL)
		return;

	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G1 - dx11gpu: legacy memcpyAlpha agirlik alpha/32 -> A=alpha*255/32
	{
		const int a8 = (alpha >= 32) ? 255 : ((int)alpha * 255) / 32;
		if (G1_TryBltSprite(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
		                    m_ClipBottom, pPoint, pSprite,
		                    D3D11QuadPipeline::MakeColor(255, 255, 255, (BYTE)a8),
		                    D3D11QuadPipeline::BLEND_ALPHA))
			return;
		G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
		        pSprite->GetWidth(), pSprite->GetHeight());
	}

	POINT pt = *pPoint;

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltAlphaClipWidth(lpSurface, Pitch, &rect, alpha);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltAlphaClipLeft(lpSurface, Pitch, &rect, alpha);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltAlphaClipRight(lpSurface, Pitch, &rect, alpha);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltAlphaClipHeight(lpSurface, Pitch, &rect, alpha);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlphaClipHeight(lpSurface, Pitch, &rect, alpha);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlpha(lpSurface, Pitch, alpha);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt Sprite Color
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteColor(POINT* pPoint, CSprite* pSprite, BYTE rgb)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G2 - dx11gpu: kanal maskesi (src & MASK_RGB[rgb]) modulate ile birebir
	{
		const DWORD mod = G2_ColorMaskToModulate(rgb);
		if (mod != 0 &&
		    G1_TryBltSprite(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
		                    m_ClipBottom, pPoint, pSprite, mod,
		                    D3D11QuadPipeline::BLEND_ALPHA))
			return;
	}
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltColorClipWidth(lpSurface, Pitch, &rect, rgb);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltColorClipLeft(lpSurface, Pitch, &rect, rgb);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltColorClipRight(lpSurface, Pitch, &rect, rgb);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltColorClipHeight(lpSurface, Pitch, &rect, rgb);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltColorClipHeight(lpSurface, Pitch, &rect, rgb);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltColor(lpSurface, Pitch, rgb);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt Sprite ColorSet
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteColorSet(POINT* pPoint, CSprite* pSprite, WORD colorSet)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G2 - dx11gpu: gradation LUT decode'lu cache'li texture
	if (G2_TryBltColorSet(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
	                      m_ClipBottom, pPoint, pSprite, colorSet))
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltColorSetClipWidth(lpSurface, Pitch, &rect, colorSet);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltColorSetClipLeft(lpSurface, Pitch, &rect, colorSet);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltColorSetClipRight(lpSurface, Pitch, &rect, colorSet);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltColorSetClipHeight(lpSurface, Pitch, &rect, colorSet);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltColorSetClipHeight(lpSurface, Pitch, &rect, colorSet);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltColorSet(lpSurface, Pitch, colorSet);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt Sprite Darkness
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteDarkness(POINT* pPoint, CSprite* pSprite, BYTE DarkBits)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G2 - dx11gpu: (src >> n) kanal karartmasi = gri modulate 255>>n
	// (legacy MASK_SHIFT tablosu 0..4 destekler, disi CPU'ya duser)
	if (DarkBits <= 4)
	{
		const BYTE gray = (BYTE)(255u >> DarkBits);
		if (G1_TryBltSprite(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
		                    m_ClipBottom, pPoint, pSprite,
		                    D3D11QuadPipeline::MakeColor(gray, gray, gray, 255),
		                    D3D11QuadPipeline::BLEND_ALPHA))
			return;
	}
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltDarknessClipWidth(lpSurface, Pitch, &rect, DarkBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltDarknessClipLeft(lpSurface, Pitch, &rect, DarkBits);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltDarknessClipRight(lpSurface, Pitch, &rect, DarkBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltDarknessClipHeight(lpSurface, Pitch, &rect, DarkBits);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltDarknessClipHeight(lpSurface, Pitch, &rect, DarkBits);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltDarkness(lpSurface, Pitch, DarkBits);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt Sprite Effect
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteEffect(POINT* pPoint, CSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G3 - dx11gpu: ifade edilebilen effect'ler GPU blend'e
	// (Darker/Lighten/Darken/Screen/DodgeBurn + GrayScale/Gradation decode)
	if (G3_TryBltSpriteEffect(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop,
	                          m_ClipRight, m_ClipBottom, pPoint, pSprite))
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltEffectClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltEffectClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltEffectClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
				pSprite->BltEffectClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltEffectClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltEffect(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Faz G7 (2026-09-03) - BltSpritePalEffect GPU yolu (KRITIK katman fix'i,
// G6'nin devami).
//
// Skill/yaratik efekt animasyonlari (m_EffectScreenSPK + m_EffectScreenPPK
// paletleri, MTopView 6 cagri noktasi — hepsi EFFECT_SCREEN) CSpritePal +
// BltSpritePalEffect ile cizilir; dx11gpu'da GPU hook'u YOKTU. CPU yolu
// memcpyPalEffectScreen dst'yi COMPOSITION'dan okur — karakter/yaratik
// artik GPU quad'inda (composition'da yok) -> efekt BAYAT zeminle screen
// blend yapiyor -> diff segmenti bu pikselleri OPAK basiyor -> efektin
// altindaki/ustundeki karakter katmani bozuluyor (kullanici raporu
// 2026-09-03: "skill animasyonlari karakterin onundeyken kayboluyor",
// G6'daki raporun kalan yarisi). GPU quad kaydi sirali komut listesinde
// dogru katmana oturur; screen blend GPU RT'deki GERCEK arka plani gorur.
//
// RLE satir formati (BYTE bazli, CSpritePal::SetPixel — CAlphaSpritePal'in
// aksine alpha byte'i YOK):
// (count)(skip, colorCount, [palIndexByte] x colorCount)*
// memcpyPalEffectScreen: dst = screen(dst, pal[idx]) — G3'un BLEND_SCREEN
// eslemesiyle ayni formul (s+d-sd, 5-bit tablo). Seffaf (skip) pikseller
// texture'da 0x00000000: screen blend'de s=0 -> dst degismez (parite).
//----------------------------------------------------------------------
inline unsigned int G7_SpritePalGeneration(const CSpritePal* pSprite,
                                           const MPalette& pal)
{
	unsigned int h = 2166136261u;
	h = (h ^ 0x50374546u) * 16777619u;   // 'P7EF' etiketi
	h = (h ^ (unsigned int)pSprite->GetWidth()) * 16777619u;
	h = (h ^ (unsigned int)pSprite->GetHeight()) * 16777619u;

	// Palet icerigi (animasyonlu paletlerde invalidation sart; <=256 WORD)
	const int palSize = (int)pal.GetSize();
	h = (h ^ (unsigned int)palSize) * 16777619u;
	for (int i = 0; i < palSize; ++i)
		h = (h ^ (unsigned int)pal[(BYTE)i]) * 16777619u;

	// Ilk satirin run yapisi (sprite icerigi sabit ama pointer reuse'a karsi)
	if (pSprite->GetHeight() > 0)
	{
		const BYTE* p = pSprite->GetPixelLine(0);
		if (p != NULL)
		{
			int count = (int)*p++;
			h = (h ^ (unsigned int)count) * 16777619u;
			for (int run = 0; run < count && run < 8; ++run)
			{
				const BYTE skip = *p++;
				const BYTE colorCount = *p++;
				h = (h ^ (unsigned int)skip) * 16777619u;
				h = (h ^ (unsigned int)colorCount) * 16777619u;
				if (colorCount > 0)
					h = (h ^ (unsigned int)p[0]) * 16777619u;   // ilk indeks
				p += (size_t)colorCount;
			}
		}
	}
	return h;
}

ID3D11ShaderResourceView* G7_GetSpritePalSRV(CSpritePal* pSprite,
                                             MPalette& pal,
                                             int* pW, int* pH)
{
	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	if (w <= 0 || h <= 0)
		return NULL;

	D3D11SpriteTextureCache& cache = D3D11SpriteTextureCache::Instance();
	// Anahtar sprite+palet ikilisi (G6 paritesi: ayni sprite farkli paletle
	// ayni frame'de cizilebilir); G6 anahtarlarindan bit 22 ile ayrisir.
	uintptr_t keyBits = (uintptr_t)pSprite ^ ((uintptr_t)&pal << 1);
	keyBits ^= ((uintptr_t)1 << 22);
	const void* key = (const void*)keyBits;
	const unsigned int gen = G7_SpritePalGeneration(pSprite, pal);

	ID3D11ShaderResourceView* pSRV = cache.Lookup(key, gen, pW, pH);
	if (pSRV != NULL)
		return pSRV;

	std::vector<DWORD> rgba((size_t)w * h, 0u);
	for (int y = 0; y < h; ++y)
	{
		const BYTE* pPixels = pSprite->GetPixelLine((WORD)y);
		if (pPixels == NULL)
			continue;
		DWORD* pDst = &rgba[(size_t)y * w];

		int count = (int)*pPixels++;
		int x = 0;
		while (count-- > 0)
		{
			x += (int)*pPixels++;
			int colorCount = (int)*pPixels++;
			for (int i = 0; i < colorCount && x < w; ++i, ++x)
				pDst[x] = G1_565ToRGBA(pal[pPixels[i]]);
			pPixels += (size_t)colorCount;
		}
	}

	pSRV = cache.InsertRGBA(key, gen, &rgba[0], w, h);
	if (pSRV != NULL)
	{
		if (pW) *pW = w;
		if (pH) *pH = h;
	}
	return pSRV;
}

bool G7_TryBltSpritePalEffect(void* lpSurface,
                              int clipL, int clipT, int clipR, int clipB,
                              const POINT* pPoint, CSpritePal* pSprite,
                              MPalette& pal)
{
	D3D11SpriteCommands& sc = D3D11SpriteCommands::Instance();
	if (!sc.IsCompositionTarget(lpSurface) || !sc.IsActive())
		return false;

	// Yalniz fixed-function blend ile birebir ifade edilebilen effect'ler
	// GPU'ya gider (G3 paritesi); kalanlar CPU yolunda kalir.
	D3D11QuadPipeline::BlendMode blend;
	if (!G3_EffectToBlend((int)CSpriteSurface::s_CurrentPalEffect, &blend))
		return false;

	int texW = 0, texH = 0;
	ID3D11ShaderResourceView* pSRV = G7_GetSpritePalSRV(pSprite, pal, &texW, &texH);
	if (pSRV == NULL)
		return false;

	const int w = (int)pSprite->GetWidth();
	const int h = (int)pSprite->GetHeight();
	return G1_RecordSprite(lpSurface, clipL, clipT, clipR, clipB, pPoint,
	                       w, h, w, h, pSRV, texW, texH,
	                       0xFFFFFFFFu, blend);
}

//----------------------------------------------------------------------
// Blt Sprite Effect
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpritePalEffect(POINT* pPoint, CSpritePal* pSprite, MPalette &pal)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;


	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G7 (2026-09-03) - dx11gpu: skill efekt animasyonu GPU quad'i
	// (katman fix'i — aciklama yukarida). GPU devraldiysa CPU yolu atlanir.
	if (G7_TryBltSpritePalEffect(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop,
	                             m_ClipRight, m_ClipBottom, pPoint, pSprite,
	                             pal))
		return;

	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	
	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltEffectClipWidth(lpSurface, Pitch, &rect, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltEffectClipLeft(lpSurface, Pitch, &rect, pal);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltEffectClipRight(lpSurface, Pitch, &rect, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
				pSprite->BltEffectClipHeight(lpSurface, Pitch, &rect, pal);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltEffectClipHeight(lpSurface, Pitch, &rect, pal);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltEffect(lpSurface, Pitch, pal);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt Sprite Scale 
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteScale(POINT* pPoint, CSprite* pSprite, BYTE scale)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G1 (2026-09-01) - dx11gpu dirty (scale bolge hesabi degisken: tum ekran)
	G1_MarkRect(m_ddsd.lpSurface, NULL);

	POINT pt = *pPoint;
	
	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltScaleClipWidth(lpSurface, Pitch, &rect, scale);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltScaleClipLeft(lpSurface, Pitch, &rect, scale);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
			pSprite->BltScaleClipRight(lpSurface, Pitch, &rect, scale);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
				pSprite->BltScaleClipHeight(lpSurface, Pitch, &rect, scale);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltScaleClipHeight(lpSurface, Pitch, &rect, scale);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltScale(lpSurface, Pitch, scale);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}



//----------------------------------------------------------------------
// Blt AlphaSprite
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSprite(POINT* pPoint, CAlphaSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G2 - dx11gpu: per-pixel alpha decode'lu texture (memcpyAlpha lerp
	// paritesi straight-alpha blend ile)
	if (G2_TryBltAlphaSprite(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
	                         m_ClipBottom, pPoint, pSprite, false, 0xFFFFFFFFu))
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
			pSprite->BltClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
				pSprite->BltClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->Blt(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt AlphaSprite
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSpritePal(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette &pal)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;
	

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G6 (2026-09-01) - dx11gpu: per-pixel alpha decode'lu texture
	// (memcpyAlpha lerp paritesi). CPU yolu bayat composition ile blend
	// yapip efekt arkasindaki GPU-cizimli yaratigi orter (kritik fix).
	if (G6_TryBltAlphaSpritePal(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop,
	                            m_ClipRight, m_ClipBottom, pPoint, pSprite,
	                            pal, false, 0xFFFFFFFFu))
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltClipWidth(lpSurface, Pitch, &rect, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltClipLeft(lpSurface, Pitch, &rect, pal);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
			pSprite->BltClipRight(lpSurface, Pitch, &rect, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
				pSprite->BltClipHeight(lpSurface, Pitch, &rect, pal);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltClipHeight(lpSurface, Pitch, &rect, pal);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->Blt(lpSurface, Pitch, pal);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt AlphaSprite Alpha
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSpriteAlpha(POINT* pPoint, CAlphaSprite* pSprite, BYTE alpha)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G2 - dx11gpu: memcpyAlphaValue per-pixel alpha'yi YOK SAYAR,
	// uniform alpha (0..32) uygular -> mask texture + modulate alpha
	{
		const DWORD a8 = (alpha >= 32) ? 255u : ((DWORD)alpha * 255u) / 32u;
		if (G2_TryBltAlphaSprite(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop,
		                         m_ClipRight, m_ClipBottom, pPoint, pSprite, true,
		                         D3D11QuadPipeline::MakeColor(255, 255, 255, (BYTE)a8)))
			return;
	}
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltAlphaClipWidth(lpSurface, Pitch, &rect, alpha);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltAlphaClipLeft(lpSurface, Pitch, &rect, alpha);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
			pSprite->BltAlphaClipRight(lpSurface, Pitch, &rect, alpha);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
				pSprite->BltAlphaClipHeight(lpSurface, Pitch, &rect, alpha);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltAlphaClipHeight(lpSurface, Pitch, &rect, alpha);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltAlpha(lpSurface, Pitch, alpha);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt AlphaSprite Alpha
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSpritePalAlpha(POINT* pPoint, CAlphaSpritePal* pSprite, BYTE alpha, MPalette &pal)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;


	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G6 (2026-09-01) - dx11gpu: memcpyAlphaValue per-pixel alpha'yi
	// YOK SAYAR, uniform alpha (0..32) uygular -> mask texture + modulate
	{
		const DWORD a8 = (alpha >= 32) ? 255u : ((DWORD)alpha * 255u) / 32u;
		if (G6_TryBltAlphaSpritePal(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop,
		                            m_ClipRight, m_ClipBottom, pPoint, pSprite,
		                            pal, true,
		                            D3D11QuadPipeline::MakeColor(255, 255, 255, (BYTE)a8)))
			return;
	}
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	


	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltAlphaClipWidth(lpSurface, Pitch, &rect, alpha, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->BltAlphaClipLeft(lpSurface, Pitch, &rect, alpha, pal);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
			pSprite->BltAlphaClipRight(lpSurface, Pitch, &rect, alpha, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
				pSprite->BltAlphaClipHeight(lpSurface, Pitch, &rect, alpha, pal);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltAlphaClipHeight(lpSurface, Pitch, &rect, alpha, pal);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));			
					pSprite->BltAlpha(lpSurface, Pitch, alpha, pal);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt AlphaSprite4444
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSprite4444(POINT* pPoint, CAlphaSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->Blt4444ClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->Blt4444ClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->Blt4444ClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->Blt4444ClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444ClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt AlphaSprite4444
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSpritePal4444(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette &pal)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;


	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->Blt4444ClipWidth(lpSurface, Pitch, &rect, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));			
			pSprite->Blt4444ClipLeft(lpSurface, Pitch, &rect, pal);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->Blt4444ClipRight(lpSurface, Pitch, &rect, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->Blt4444ClipHeight(lpSurface, Pitch, &rect, pal);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444ClipHeight(lpSurface, Pitch, &rect, pal);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444(lpSurface, Pitch, pal);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt AlphaSprite4444 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSprite4444NotTrans(POINT* pPoint, CAlphaSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->Blt4444NotTransClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->Blt4444NotTransClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->Blt4444NotTransClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444NotTrans(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt AlphaSprite4444 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSpritePal4444NotTrans(POINT* pPoint, CAlphaSpritePal* pSprite, MPalette &pal)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->Blt4444NotTransClipWidth(lpSurface, Pitch, &rect, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->Blt4444NotTransClipLeft(lpSurface, Pitch, &rect, pal);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->Blt4444NotTransClipRight(lpSurface, Pitch, &rect, pal);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect, pal);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect, pal);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444NotTrans(lpSurface, Pitch, pal);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt AlphaSprite4444 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSprite4444SmallNotTrans(POINT* pPoint, CAlphaSprite* pSprite, BYTE shift)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth() >> shift;
	rect.bottom = pSprite->GetHeight() >> shift;


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + (pSprite->GetWidth() >> shift);
	int yPlusSpriteHeight	= pt.y + (pSprite->GetHeight() >> shift);

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -(pt.x << shift);		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipWidth(lpSurface, Pitch, &rect, shift);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipLeft(lpSurface, Pitch, &rect, shift);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			//pSprite->Blt4444SmallClipRight(lpSurface, Pitch, &rect, shift);			
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -(pt.y << shift);

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444SmallNotTrans(lpSurface, Pitch, shift);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt AlphaSprite4444 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltAlphaSpritePal4444SmallNotTrans(POINT* pPoint, CAlphaSpritePal* pSprite, BYTE shift, MPalette &pal)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;


	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth() >> shift;
	rect.bottom = pSprite->GetHeight() >> shift;


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + (pSprite->GetWidth() >> shift);
	int yPlusSpriteHeight	= pt.y + (pSprite->GetHeight() >> shift);

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -(pt.x << shift);		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipWidth(lpSurface, Pitch, &rect, shift);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipLeft(lpSurface, Pitch, &rect, shift);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			//pSprite->Blt4444SmallClipRight(lpSurface, Pitch, &rect, shift);			
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -(pt.y << shift);

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444SmallNotTrans(lpSurface, Pitch, shift, pal);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt IndexSprite
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltIndexSprite(POINT* pPoint, CIndexSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G3 - dx11gpu: s_IndexValue remap'li decode (creature renkleri
	// anahtar+generation'a girer, farkli creature'lar ayri cache'lenir)
	if (G3_TryBltIndexSprite(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop,
	                         m_ClipRight, m_ClipBottom, pPoint, pSprite))
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt IndexSprite Darkness
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltIndexSpriteDarkness(POINT* pPoint, CIndexSprite* pSprite, BYTE DarkBits)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltDarknessClipWidth(lpSurface, Pitch, &rect, DarkBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltDarknessClipLeft(lpSurface, Pitch, &rect, DarkBits);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltDarknessClipRight(lpSurface, Pitch, &rect, DarkBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltDarknessClipHeight(lpSurface, Pitch, &rect, DarkBits);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltDarknessClipHeight(lpSurface, Pitch, &rect, DarkBits);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltDarkness(lpSurface, Pitch, DarkBits);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt IndexSprite Alpha
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltIndexSpriteAlpha(POINT* pPoint, CIndexSprite* pSprite, BYTE alpha)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltAlphaClipWidth(lpSurface, Pitch, &rect, alpha);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltAlphaClipLeft(lpSurface, Pitch, &rect, alpha);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltAlphaClipRight(lpSurface, Pitch, &rect, alpha);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltAlphaClipHeight(lpSurface, Pitch, &rect, alpha);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlphaClipHeight(lpSurface, Pitch, &rect, alpha);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlpha(lpSurface, Pitch, alpha);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}



//----------------------------------------------------------------------
// Blt IndexSprite Color
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltIndexSpriteColor(POINT* pPoint, CIndexSprite* pSprite, BYTE rgb)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltColorClipWidth(lpSurface, Pitch, &rect, rgb);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltColorClipLeft(lpSurface, Pitch, &rect, rgb);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltColorClipRight(lpSurface, Pitch, &rect, rgb);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltColorClipHeight(lpSurface, Pitch, &rect, rgb);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltColorClipHeight(lpSurface, Pitch, &rect, rgb);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltColor(lpSurface, Pitch, rgb);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt IndexSprite ColorSet
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltIndexSpriteColorSet(POINT* pPoint, CIndexSprite* pSprite, WORD colorSet)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltColorSetClipWidth(lpSurface, Pitch, &rect, colorSet);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltColorSetClipLeft(lpSurface, Pitch, &rect, colorSet);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltColorSetClipRight(lpSurface, Pitch, &rect, colorSet);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltColorSetClipHeight(lpSurface, Pitch, &rect, colorSet);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltColorSetClipHeight(lpSurface, Pitch, &rect, colorSet);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltColorSet(lpSurface, Pitch, colorSet);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt IndexSpriteEffect
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltIndexSpriteEffect(POINT* pPoint, CIndexSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G3 - dx11gpu: legacy paritesiyle RAW decode (idxWord 565 muamelesi)
	// + ifade edilebilen effect blend. Digerleri CPU'da.
	if (G3_TryBltIndexSpriteEffect(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop,
	                               m_ClipRight, m_ClipBottom, pPoint, pSprite))
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltEffectClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltEffectClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltEffectClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltEffectClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltEffectClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltEffect(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt SpriteAlpha Filter
//----------------------------------------------------------------------
// 아직 Clipping이 지원되지 않음
//----------------------------------------------------------------------
/*
void	
CSpriteSurface::BltSpriteAlphaFilter(POINT* pPoint, CSprite* pSprite, CFilter* pFilter)
{
	WORD*	lpSurface;
	WORD	Pitch;

	// Lock
	LockW(lpSurface, Pitch);

	// 출력 시작 위치			
	//lpSurface = (WORD*)((BYTE*)lpSurface + pPoint->y*Pitch + (pPoint->x<<1));
	lpSurface = (WORD*)((BYTE*)lpSurface + pPoint->y*Pitch + (pPoint->x<<1));
	pSprite->BltAlphaFilter(lpSurface, Pitch, pFilter);

	Unlock();
}
*/


//----------------------------------------------------------------------
// Blt SpriteAlphaFilter
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
// Filter는 CSprite::SetFilter(...)를 통해서 설정된다.
//
// Filter가 Sprite와 겹치지 않는 경우...
// BltSprite를 호출하도록 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteAlphaFilter(POINT* pPoint, CSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit() || CSprite::GetFilter()->IsNotInit())
		return;

	//-------------------------------------------------------------
	// Filter가 Sprite와 겹치지 않는 경우...
	// BltSprite를 호출하도록 한다.
	//-------------------------------------------------------------
	if (!pSprite->IsIntersectFilter())
	{
		BltSprite( pPoint, pSprite );
		return;
	}

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G8 (2026-09-03) - oyuncu arkasindaki obje seridi GPU'da cizilir
	// (maske filter texture alpha'sina islenir; "yarim siyah obje" fix'i,
	// aciklama G8_GetSpriteAlphaFilterSRV ustunde)
	if (G8_TryBltSpriteAlphaFilter(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop,
	                               m_ClipRight, m_ClipBottom, pPoint, pSprite))
		return;

	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltAlphaFilterClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltAlphaFilterClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltAlphaFilterClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltAlphaFilterClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlphaFilterClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlphaFilter(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt SpriteAlphaFilter Darkness
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
// Filter는 CSprite::SetFilter(...)를 통해서 설정된다.
//
// Filter가 Sprite와 겹치지 않는 경우...
// BltSpriteDarkness를 호출하도록 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteAlphaFilterDarkness(POINT* pPoint, CSprite* pSprite, BYTE DarkBits)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit() || CSprite::GetFilter()->IsNotInit())
		return;

	//-------------------------------------------------------------
	// Filter가 Sprite와 겹치지 않는 경우...
	// BltSprite를 호출하도록 한다.
	//-------------------------------------------------------------
	if (!pSprite->IsIntersectFilter())
	{
		BltSpriteDarkness( pPoint, pSprite, DarkBits );
		return;
	}

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltAlphaFilterDarknessClipWidth(lpSurface, Pitch, &rect, DarkBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltAlphaFilterDarknessClipLeft(lpSurface, Pitch, &rect, DarkBits);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltAlphaFilterDarknessClipRight(lpSurface, Pitch, &rect, DarkBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltAlphaFilterDarknessClipHeight(lpSurface, Pitch, &rect, DarkBits);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlphaFilterDarknessClipHeight(lpSurface, Pitch, &rect, DarkBits);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlphaFilterDarkness(lpSurface, Pitch, DarkBits);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt Sprite Alpha4444 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteAlpha4444NotTrans(POINT* pPoint, CSprite* pSprite, BYTE alpha)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444NotTransClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444NotTransClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			//pSprite->Blt4444NotTransClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					//pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlpha4444NotTrans(lpSurface, Pitch, alpha);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt AlphaSprite4444 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteAlpha4444SmallNotTrans(POINT* pPoint, CSprite* pSprite, BYTE alpha, BYTE shift)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth() >> shift;
	rect.bottom = pSprite->GetHeight() >> shift;


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + (pSprite->GetWidth() >> shift);
	int yPlusSpriteHeight	= pt.y + (pSprite->GetHeight() >> shift);

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -(pt.x << shift);		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipWidth(lpSurface, Pitch, &rect, shift);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipLeft(lpSurface, Pitch, &rect, shift);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			//pSprite->Blt4444SmallClipRight(lpSurface, Pitch, &rect, shift);			
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -(pt.y << shift);

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltAlpha4444SmallNotTrans(lpSurface, Pitch, alpha, shift);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt Sprite 1555 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSprite1555NotTrans(POINT* pPoint, CSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444NotTransClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444NotTransClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			//pSprite->Blt4444NotTransClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					//pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt1555NotTrans(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt Sprite 1555 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpritePal1555NotTrans(POINT* pPoint, CSpritePal* pSprite, MPalette &pal)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;


	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444NotTransClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444NotTransClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			//pSprite->Blt4444NotTransClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					//pSprite->Blt4444NotTransClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt1555NotTrans(lpSurface, Pitch, pal);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt AlphaSprite4444 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSprite1555SmallNotTrans(POINT* pPoint, CSprite* pSprite, BYTE shift)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth() >> shift;
	rect.bottom = pSprite->GetHeight() >> shift;


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + (pSprite->GetWidth() >> shift);
	int yPlusSpriteHeight	= pt.y + (pSprite->GetHeight() >> shift);

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -(pt.x << shift);		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipWidth(lpSurface, Pitch, &rect, shift);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipLeft(lpSurface, Pitch, &rect, shift);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			//pSprite->Blt4444SmallClipRight(lpSurface, Pitch, &rect, shift);			
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -(pt.y << shift);

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt1555SmallNotTrans(lpSurface, Pitch, shift);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt AlphaSprite4444 NotTrans
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다. 투명색 부분은 검게 칠한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpritePal1555SmallNotTrans(POINT* pPoint, CSpritePal* pSprite, BYTE shift, MPalette &pal)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;


	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth() >> shift;
	rect.bottom = pSprite->GetHeight() >> shift;


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + (pSprite->GetWidth() >> shift);
	int yPlusSpriteHeight	= pt.y + (pSprite->GetHeight() >> shift);

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -(pt.x << shift);		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipWidth(lpSurface, Pitch, &rect, shift);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->Blt4444SmallClipLeft(lpSurface, Pitch, &rect, shift);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			//pSprite->Blt4444SmallClipRight(lpSurface, Pitch, &rect, shift);			
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -(pt.y << shift);

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					//pSprite->Blt4444SmallClipHeight(lpSurface, Pitch, &rect, shift);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt1555SmallNotTrans(lpSurface, Pitch, shift, pal);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt SpriteDarkerFilter
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
// Filter는 CSprite::SetFilter(...)를 통해서 설정된다.
//
// Filter가 Sprite와 겹치지 않는 경우...
// BltSprite를 호출하도록 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltSpriteDarkerFilter(POINT* pPoint, CSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit() || CSprite::GetFilter()->IsNotInit())
		return;

	//-------------------------------------------------------------
	// Filter가 Sprite와 겹치지 않는 경우...
	// BltSprite를 호출하도록 한다.
	//-------------------------------------------------------------
	// Darker는 Tile에서만 사용할 예정이므로..
	// 일단은... 
	/*
	if (!pSprite->IsIntersectFilter())
	{
		BltSprite( pPoint, pSprite );
		return;
	}
	*/

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltDarkerFilterClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltDarkerFilterClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltDarkerFilterClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltDarkerFilterClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltDarkerFilterClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltDarkerFilter(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// Blt Sprite
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltShadowSprite(POINT* pPoint, CShadowSprite* pSprite)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G1 - dx11gpu: golge = opak siyah maske quad'i (memset(0) paritesi)
	if (G1_TryBltShadow(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
	                    m_ClipBottom, pPoint, pSprite))
		return;
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());

	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltClipWidth(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltClipLeft(lpSurface, Pitch, &rect);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltClipRight(lpSurface, Pitch, &rect);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltClipHeight(lpSurface, Pitch, &rect);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltClipHeight(lpSurface, Pitch, &rect);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt(lpSurface, Pitch);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// BltDarkness Sprite
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltShadowSpriteDarkness(POINT* pPoint, CShadowSprite* pSprite, BYTE DarkBits)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltDarknessClipWidth(lpSurface, Pitch, &rect, DarkBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltDarknessClipLeft(lpSurface, Pitch, &rect, DarkBits);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltDarknessClipRight(lpSurface, Pitch, &rect, DarkBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltDarknessClipHeight(lpSurface, Pitch, &rect, DarkBits);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltDarknessClipHeight(lpSurface, Pitch, &rect, DarkBits);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltDarkness(lpSurface, Pitch, DarkBits);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// BltSmall Sprite
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltShadowSpriteSmall(POINT* pPoint, CShadowSprite* pSprite, BYTE shift)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;

	// Faz G1 - dx11gpu: kucultulmus golge (dest >>shift, point sampling)
	if (G1_TryBltShadow(m_ddsd.lpSurface, m_ClipLeft, m_ClipTop, m_ClipRight,
	                    m_ClipBottom, pPoint, pSprite,
	                    (int)pSprite->GetWidth() >> shift,
	                    (int)pSprite->GetHeight() >> shift))
		return;
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());

	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth() >> shift;
	rect.bottom = pSprite->GetHeight() >> shift;


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + (pSprite->GetWidth() >> shift);
	int yPlusSpriteHeight	= pt.y + (pSprite->GetHeight() >> shift);

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -(pt.x << shift);		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltSmallClipWidth(lpSurface, Pitch, &rect, shift);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltSmallClipLeft(lpSurface, Pitch, &rect, shift);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltSmallClipRight(lpSurface, Pitch, &rect, shift);			
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -(pt.y << shift);

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltSmallClipHeight(lpSurface, Pitch, &rect, shift);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltSmallClipHeight(lpSurface, Pitch, &rect, shift);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltSmall(lpSurface, Pitch, shift);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}


//----------------------------------------------------------------------
// BltSmall4444 Sprite
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltShadowSpriteSmall4444(POINT* pPoint, CShadowSprite* pSprite, WORD pixel, BYTE shift)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth() >> shift;
	rect.bottom = pSprite->GetHeight() >> shift;


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + (pSprite->GetWidth() >> shift);
	int yPlusSpriteHeight	= pt.y + (pSprite->GetHeight() >> shift);

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -(pt.x << shift);		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->BltSmall4444ClipWidth(lpSurface, Pitch, &rect, pixel, shift);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			//pSprite->BltSmall4444ClipLeft(lpSurface, Pitch, &rect, pixel, shift);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
//			pSprite->BltSmall4444ClipRight(lpSurface, Pitch, &rect, pixel, shift);			
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -(pt.y << shift);

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
//				pSprite->BltSmall4444ClipHeight(lpSurface, Pitch, &rect, pixel, shift);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
//					pSprite->BltSmall4444ClipHeight(lpSurface, Pitch, &rect, pixel, shift);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltSmall4444(lpSurface, Pitch, pixel, shift);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}

//----------------------------------------------------------------------
// Blt ShadowSprite4444
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltShadowSprite4444(POINT* pPoint, CShadowSprite* pSprite, WORD pixel)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->Blt4444ClipWidth(lpSurface, Pitch, &rect, pixel);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->Blt4444ClipLeft(lpSurface, Pitch, &rect, pixel);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->Blt4444ClipRight(lpSurface, Pitch, &rect, pixel);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->Blt4444ClipHeight(lpSurface, Pitch, &rect, pixel);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444ClipHeight(lpSurface, Pitch, &rect, pixel);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->Blt4444(lpSurface, Pitch, pixel);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}



//----------------------------------------------------------------------
// Half Copy
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 반투명 출력을 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyHalf(WORD* pDest, WORD* pSource, WORD pixels)
{
	QWORD	*qpDest		= (QWORD*)pDest, 
			*qpSource	= (QWORD*)pSource;


	int qTimes = pixels >> 2;	// pixels / 4

	// 반투명
	switch ( pixels & 0x03 )	// pixels % 4
	{
		//------------------
		// 4점씩
		//------------------
		case 0 :			
			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = 
					((*qpDest >> 1) & CDirectDraw::s_qwMASK_SHIFT[1]) +
					((*qpSource >> 1) & CDirectDraw::s_qwMASK_SHIFT[1]);
				

				*qpDest++;
				*qpSource++;
			}
		break;

		//------------------
		// 1점 + 4점씩
		//------------------
		case 1 :
			// 한점 찍기
			*(WORD*)qpDest = 
					((*(WORD*)qpDest >> 1) & CDirectDraw::s_wMASK_SHIFT[1]) +
					((*(WORD*)qpSource >> 1) & CDirectDraw::s_wMASK_SHIFT[1]);
				
			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);

			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = 
					((*qpDest >> 1) & CDirectDraw::s_qwMASK_SHIFT[1]) +
					((*qpSource >> 1) & CDirectDraw::s_qwMASK_SHIFT[1]);

				*qpDest++;
				*qpSource++;
			}
			
		break;

		//------------------
		// 2점 + 4점씩
		//------------------
		case 2 :
			// 두점 찍기
			*(DWORD*)qpDest = 
					((*(DWORD*)qpDest >> 1) & CDirectDraw::s_dwMASK_SHIFT[1]) +
					((*(DWORD*)qpSource >> 1) & CDirectDraw::s_dwMASK_SHIFT[1]);
				
			qpDest = (QWORD*)((DWORD*)qpDest + 1);
			qpSource = (QWORD*)((DWORD*)qpSource + 1);


			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = 
					((*qpDest >> 1) & CDirectDraw::s_qwMASK_SHIFT[1]) +
					((*qpSource >> 1) & CDirectDraw::s_qwMASK_SHIFT[1]);

				*qpDest++;
				*qpSource++;
			}
		break;

		//------------------
		// 1점 + 2점 + 4점씩
		//------------------
		case 3 :
			// 한점 찍기
			*(WORD*)qpDest = 
					((*(WORD*)qpDest >> 1) & CDirectDraw::s_wMASK_SHIFT[1]) +
					((*(WORD*)qpSource >> 1) & CDirectDraw::s_wMASK_SHIFT[1]);
				
			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);


			// 두점 찍기
			*(DWORD*)qpDest = 
					((*(DWORD*)qpDest >> 1) & CDirectDraw::s_dwMASK_SHIFT[1]) +
					((*(DWORD*)qpSource >> 1) & CDirectDraw::s_dwMASK_SHIFT[1]);
				
			qpDest = (QWORD*)((DWORD*)qpDest + 1);
			qpSource = (QWORD*)((DWORD*)qpSource + 1);

			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = 
					((*qpDest >> 1) & CDirectDraw::s_qwMASK_SHIFT[1]) +
					((*qpSource >> 1) & CDirectDraw::s_qwMASK_SHIFT[1]);

				*qpDest++;
				*qpSource++;
			}
		break;
	}
}


//----------------------------------------------------------------------
// Alpha Copy 1Pixel
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 s_Value1값으로 출력을 한다.
//
// s_Value1 = 1~32
//
//----------------------------------------------------------------------
/*
WORD
CSpriteSurface::memcpyAlpha1Pixel(WORD pDest, WORD pSource)
{
	WORD sr,sg,sb, dr,dg,db;

	// 한점 찍기
	sr = CDirectDraw::Red(pSource);
	sg = CDirectDraw::Green(pSource);
	sb = CDirectDraw::Blue(pSource);

	dr = CDirectDraw::Red(pDest);
	dg = CDirectDraw::Green(pDest);
	db = CDirectDraw::Blue(pDest);		
	
	return ((s_Value1 * (sb - db) >> 5) + db |
				((s_Value1 * (sg - dg) >> 5) + dg) << CDirectDraw::s_bSHIFT_G |
				((s_Value1 * (sr - dr) >> 5) + dr) << CDirectDraw::s_bSHIFT_R);
}
*/

//----------------------------------------------------------------------
// Alpha Copy
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 s_Value1값으로 출력을 한다.
//
// s_Value1 = 1~32
//
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyAlpha(WORD* pDest, WORD* pSource, WORD pixels)
{
//	WORD sr,sg,sb, dr,dg,db;
//	
//	while(pixels--)
//	{
//	// 한점 찍기
//	sr = CDirectDraw::Red(*pSource);
//	sg = CDirectDraw::Green(*pSource);
//	sb = CDirectDraw::Blue(*pSource);
//	
//	dr = CDirectDraw::Red(*pDest);
//	dg = CDirectDraw::Green(*pDest);
//	db = CDirectDraw::Blue(*pDest);		
//	
//	*pDest = ((s_Value1 * (sb - db) >> 5) + db |
//		((s_Value1 * (sg - dg) >> 5) + dg) << CDirectDraw::s_bSHIFT_G |
//		((s_Value1 * (sr - dr) >> 5) + dr) << CDirectDraw::s_bSHIFT_R);
//	
//	pDest++;
//	pSource++;
//	}
//	return;

	QWORD	*qpDest		= (QWORD*)pDest, 
			*qpSource	= (QWORD*)pSource;

	//static WORD		sTemp,dTemp;
	//static WORD		sr,sg,sb,dr,dg,db;

	DWORD	srcColor2, destColor2;
	DWORD	first2, second2;

	QWORD	srcColor, destColor;
	QWORD	first, second;

	int qTimes = pixels >> 2;	// pixels / 4

	// Alpha Blending
	switch ( pixels & 0x03 )	// pixels % 4
	{
		//------------------
		// 4점씩
		//------------------
		case 0 :			
			// 네점씩 찍기
			/*
			while (qTimes--)
			{
				srcColor  = *qpSource;
				destColor = *qpDest;
			
				first   =   (((srcColor & CDirectDraw::s_qwMASK_ALPHA0) >> 5) * s_Value1) &  CDirectDraw::s_qwMASK_ALPHA0;				
				second  =   (((srcColor & CDirectDraw::s_qwMASK_ALPHA1) * s_Value1) >> 5) & CDirectDraw::s_qwMASK_ALPHA1;
				
				srcColor =  first | second;

				first   =   (((destColor & CDirectDraw::s_qwMASK_ALPHA0) >> 5) * s_Value2) & CDirectDraw::s_qwMASK_ALPHA0;				
				second  =   (((destColor & CDirectDraw::s_qwMASK_ALPHA1) * s_Value2) >> 5) & CDirectDraw::s_qwMASK_ALPHA1;				

				destColor=  first | second;
				
				*qpDest = srcColor + destColor;

				qpDest++;
				qpSource++;
			}
			*/
			///*
			while (qTimes--)
			{
				if(*qpSource != *qpDest)
				{
					srcColor  = *qpSource;
					destColor = *qpDest;
				
					first   =   srcColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  s_Value1;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;
					
					second  =   srcColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  s_Value1;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;
					
					srcColor =  first | second;

					first   =   destColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  s_Value2;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   destColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  s_Value2;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					destColor=  first | second;
					
					*qpDest = srcColor + destColor;
				}

				qpDest++;
				qpSource++;
			}
			//*/
		break;

		//------------------
		// 1점 + 4점씩
		//------------------
		case 1 :
			// 한 점 찍기
			srcColor2  = *(WORD*)qpSource;
			destColor2 = *(WORD*)qpDest;

			if(srcColor2 != destColor2)
			{
				first2   =   srcColor2 & CDirectDraw::s_wMASK_ALPHA1;
				first2   >>= 5;
				first2   *=  s_Value1;
				first2   &=  CDirectDraw::s_wMASK_ALPHA1;

				second2  =   srcColor2 & CDirectDraw::s_wMASK_ALPHA0;
				second2  *=  s_Value1;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_wMASK_ALPHA0;

				srcColor2 =  first2 | second2;

				first2   =   destColor2 & CDirectDraw::s_wMASK_ALPHA1;
				first2   >>= 5;
				first2   *=  s_Value2;
				first2   &=  CDirectDraw::s_wMASK_ALPHA1;

				second2  =   destColor2 & CDirectDraw::s_wMASK_ALPHA0;
				second2  *=  s_Value2;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_wMASK_ALPHA0;

				destColor2=  first2 | second2;

				*(WORD*)qpDest = (WORD)(srcColor2 + destColor2);
			}
				
			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);
			

			// 네점씩 찍기
			while (qTimes--)
			{
				if(*qpSource != *qpDest)
				{
					srcColor  = *qpSource;
					destColor = *qpDest;

					first   =   srcColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  s_Value1;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   srcColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  s_Value1;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					srcColor =  first | second;

					first   =   destColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  s_Value2;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   destColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  s_Value2;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					destColor=  first | second;

					*qpDest = srcColor + destColor;
				}

				qpDest++;
				qpSource++;
			}
			
		break;

		//------------------
		// 2점 + 4점씩
		//------------------
		case 2 :
			// 두점 찍기
			srcColor2  = *(DWORD*)qpSource;
			destColor2 = *(DWORD*)qpDest;

			if(srcColor2 != destColor2)
			{
				first2   =   srcColor2 & CDirectDraw::s_dwMASK_ALPHA0;
				first2   >>= 5;
				first2   *=  s_Value1;
				first2   &=  CDirectDraw::s_dwMASK_ALPHA0;

				second2  =   srcColor2 & CDirectDraw::s_dwMASK_ALPHA1;
				second2  *=  s_Value1;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_dwMASK_ALPHA1;

				srcColor2 =  first2 | second2;

				first2   =   destColor2 & CDirectDraw::s_dwMASK_ALPHA0;
				first2   >>= 5;
				first2   *=  s_Value2;
				first2   &=  CDirectDraw::s_dwMASK_ALPHA0;

				second2  =   destColor2 & CDirectDraw::s_dwMASK_ALPHA1;
				second2  *=  s_Value2;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_dwMASK_ALPHA1;

				destColor2=  first2 | second2;

				*(DWORD*)qpDest = srcColor2 + destColor2;
			}
			
			qpDest = (QWORD*)((DWORD*)qpDest + 1);
			qpSource = (QWORD*)((DWORD*)qpSource + 1);


			// 네점씩 찍기
			while (qTimes--)
			{
				if(*qpSource != *qpDest)
				{
					srcColor  = *qpSource;
					destColor = *qpDest;

					first   =   srcColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  s_Value1;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   srcColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  s_Value1;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					srcColor =  first | second;

					first   =   destColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  s_Value2;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   destColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  s_Value2;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					destColor=  first | second;

					*qpDest = srcColor + destColor;
				}
				qpDest++;
				qpSource++;
			}
		break;

		//------------------
		// 1점 + 2점 + 4점씩
		//------------------
		case 3 :
			// 한점 찍기
			/*
			sTemp = *((WORD*)qpSource);
			dTemp = *((WORD*)qpDest);
			sb = sTemp & 0x1F;
			db = dTemp & 0x1F;
			sg = (sTemp >> 5) & 0x3F;
			dg = (dTemp >> 5) & 0x3F;
			sr = (sTemp >> 11) & 0x1F;
			dr = (dTemp >> 11) & 0x1F;

			*((WORD*)qpDest) = (WORD)((s_Value1 * (sb - db) >> 5) + db |
									((s_Value1 * (sg - dg) >> 5) + dg) << 5 |
									((s_Value1 * (sr - dr) >> 5) + dr) << 11);
			//*(WORD*)qpDest = (((sr*s_Value1 + dr*s_Value2) >> 5) << 11) |
							//(((sg*s_Value1 + dg*s_Value2) >> 5) << 5) |
							//	((sb*s_Value1 + db*s_Value2) >> 5);

			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);
			*/
			// 한점 찍기		
			srcColor2  = *(WORD*)qpSource;
			destColor2 = *(WORD*)qpDest;

			if(srcColor2 != destColor2)
			{
				first2   =   srcColor2 & CDirectDraw::s_wMASK_ALPHA1;
				first2   >>= 5;
				first2   *=  s_Value1;
				first2   &=  CDirectDraw::s_wMASK_ALPHA1;

				second2  =   srcColor2 & CDirectDraw::s_wMASK_ALPHA0;
				second2  *=  s_Value1;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_wMASK_ALPHA0;

				srcColor2 =  first2 | second2;

				first2   =   destColor2 & CDirectDraw::s_wMASK_ALPHA1;
				first2   >>= 5;
				first2   *=  s_Value2;
				first2   &=  CDirectDraw::s_wMASK_ALPHA1;

				second2  =   destColor2 & CDirectDraw::s_wMASK_ALPHA0;
				second2  *=  s_Value2;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_wMASK_ALPHA0;

				destColor2=  first2 | second2;

				*(WORD*)qpDest = (WORD)(srcColor2 + destColor2);
			}

			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);				
			

			// 두점 찍기
			srcColor2  = *(DWORD*)qpSource;
			destColor2 = *(DWORD*)qpDest;

			if(srcColor2 != destColor2)
			{
				// 1111 1000 0001 1111 
				// 0000 0111 1110 0000
				first2   =   srcColor2 & CDirectDraw::s_dwMASK_ALPHA0;
				first2   >>= 5;
				first2   *=  s_Value1;
				first2   &=  CDirectDraw::s_dwMASK_ALPHA0;

				second2  =   srcColor2 & CDirectDraw::s_dwMASK_ALPHA1;
				second2  *=  s_Value1;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_dwMASK_ALPHA1;

				srcColor2 =  first2 | second2;

				first2   =   destColor2 & CDirectDraw::s_dwMASK_ALPHA0;
				first2   >>= 5;
				first2   *=  s_Value2;
				first2   &=  CDirectDraw::s_dwMASK_ALPHA0;

				second2  =   destColor2 & CDirectDraw::s_dwMASK_ALPHA1;
				second2  *=  s_Value2;
				second2  >>= 5;
				second2  &=  CDirectDraw::s_dwMASK_ALPHA1;

				destColor2=  first2 | second2;

				*(DWORD*)qpDest = srcColor2 + destColor2;
			}

			qpDest = (QWORD*)((DWORD*)qpDest + 1);
			qpSource = (QWORD*)((DWORD*)qpSource + 1);

			// 네점씩 찍기
			while (qTimes--)
			{
				if(*qpSource != *qpDest)
				{
					srcColor  = *qpSource;
					destColor = *qpDest;

					first   =   srcColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  s_Value1;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   srcColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  s_Value1;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					srcColor =  first | second;

					first   =   destColor & CDirectDraw::s_qwMASK_ALPHA0;
					first   >>= 5;
					first   *=  s_Value2;
					first   &=  CDirectDraw::s_qwMASK_ALPHA0;

					second  =   destColor & CDirectDraw::s_qwMASK_ALPHA1;
					second  *=  s_Value2;
					second  >>= 5;
					second  &=  CDirectDraw::s_qwMASK_ALPHA1;

					destColor=  first | second;

					*qpDest = srcColor + destColor;
				}

				qpDest++;
				qpSource++;
			}
		break;
	}
}


//----------------------------------------------------------------------
// Color Copy
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특정한 Color값(s_Value1)으로 출력을 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyColor(WORD* pDest, WORD* pSource, WORD pixels)
{
	QWORD	*qpDest		= (QWORD*)pDest, 
			*qpSource	= (QWORD*)pSource;


	int qTimes = pixels >> 2;	// pixels / 4

	// rgb값 중 하나만 출력한다.
	switch ( pixels & 0x03 )	// pixels % 4
	{
		//------------------
		// 4점씩
		//------------------
		case 0 :			
			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = (*qpSource & CDirectDraw::s_qwMASK_RGB[s_Value1]);

				*qpDest++;
				*qpSource++;
			}
		break;

		//------------------
		// 1점 + 4점씩
		//------------------
		case 1 :
			// 한점 찍기
			*(WORD*)qpDest = *(WORD*)qpSource & CDirectDraw::s_wMASK_RGB[s_Value1];
				
			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);

			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = (*qpSource & CDirectDraw::s_qwMASK_RGB[s_Value1]);

				*qpDest++;
				*qpSource++;
			}
			
		break;

		//------------------
		// 2점 + 4점씩
		//------------------
		case 2 :
			// 두점 찍기
			*(DWORD*)qpDest = *(DWORD*)qpSource & CDirectDraw::s_dwMASK_RGB[s_Value1];
				
			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);

			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = (*qpSource & CDirectDraw::s_qwMASK_RGB[s_Value1]);

				*qpDest++;
				*qpSource++;
			}
		break;

		//------------------
		// 1점 + 2점 + 4점씩
		//------------------
		case 3 :
			// 한점 찍기
			*(WORD*)qpDest = *(WORD*)qpSource & CDirectDraw::s_wMASK_RGB[s_Value1];
				
			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);

			// 두점 찍기
			*(DWORD*)qpDest = *(DWORD*)qpSource & CDirectDraw::s_dwMASK_RGB[s_Value1];

			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);

			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = (*qpSource & CDirectDraw::s_qwMASK_RGB[s_Value1]);

				*qpDest++;
				*qpSource++;
			}
		break;
	}
}

//----------------------------------------------------------------------
// Scale Copy
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 Scale(s_Value1)을 바꿔서 
// 출력한다. 
//
// 이 함수는 실제 게임에서 사용하지 않으므로 
// 속도에 신경을 안써도 될 것이다. - -;;;
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyScale(WORD* pDest, WORD destPitch, WORD* pSource, WORD pixels)
{
	WORD*	pDestTemp, *pDestTemp2;

	int n=pixels,i,j;

	// n개의 점을 출력한다.
	while (n--)
	{
		pDestTemp = pDest;
		// 각 점에 대해서 scale*scale의 크기로 출력한다.

		i = s_Value1;
		while (i--)
		{
			pDestTemp2 = pDestTemp;

			j=s_Value1;
			while (j--)
			{
				*pDestTemp2++ = *pSource;
			}
			pDestTemp = (WORD*)((BYTE*)pDestTemp + destPitch);
		}

		pDest += s_Value1;
		pSource++;
	}
}


//----------------------------------------------------------------------
// Darkness Copy
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특정한 Darkness값(s_Value1)만큼
// 어둡게 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyDarkness(WORD* pDest, WORD* pSource, WORD pixels)
{
	QWORD	*qpDest		= (QWORD*)pDest, 
			*qpSource	= (QWORD*)pSource;


	int qTimes = pixels >> 2;	// pixels / 4

	// 어둡게 출력한다.
	switch ( pixels & 0x03 )	// pixels % 4
	{
		//------------------
		// 4점씩
		//------------------
		case 0 :			
			// 네점씩 찍기
			while (qTimes--)
			{					
				*qpDest = (*qpSource >> s_Value1) & CDirectDraw::s_qwMASK_SHIFT[s_Value1];

				*qpDest++;
				*qpSource++;
			}
		break;

		//------------------
		// 1점 + 4점씩
		//------------------
		case 1 :
			// 한점 찍기
			*(WORD*)qpDest = (*(WORD*)qpSource >> s_Value1) & CDirectDraw::s_wMASK_SHIFT[s_Value1];
				
			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);

			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = (*qpSource >> s_Value1) & CDirectDraw::s_qwMASK_SHIFT[s_Value1];

				*qpDest++;
				*qpSource++;
			}
			
		break;

		//------------------
		// 2점 + 4점씩
		//------------------
		case 2 :
			// 두점 찍기
			*(DWORD*)qpDest = (*(DWORD*)qpSource >> s_Value1) & CDirectDraw::s_dwMASK_SHIFT[s_Value1];
				
			qpDest = (QWORD*)((DWORD*)qpDest + 1);
			qpSource = (QWORD*)((DWORD*)qpSource + 1);

			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = (*qpSource >> s_Value1) & CDirectDraw::s_qwMASK_SHIFT[s_Value1];

				*qpDest++;
				*qpSource++;
			}
		break;

		//------------------
		// 1점 + 2점 + 4점씩
		//------------------
		case 3 :
			// 한점 찍기
			*(WORD*)qpDest = (*(WORD*)qpSource >> s_Value1) & CDirectDraw::s_wMASK_SHIFT[s_Value1];
				
			qpDest = (QWORD*)((WORD*)qpDest + 1);
			qpSource = (QWORD*)((WORD*)qpSource + 1);

			// 두점 찍기
			*(DWORD*)qpDest = (*(DWORD*)qpSource >> s_Value1) & CDirectDraw::s_dwMASK_SHIFT[s_Value1];

			qpDest = (QWORD*)((DWORD*)qpDest + 1);
			qpSource = (QWORD*)((DWORD*)qpSource + 1);

			// 네점씩 찍기
			while (qTimes--)
			{
				*qpDest = (*qpSource >> s_Value1) & CDirectDraw::s_qwMASK_SHIFT[s_Value1];

				*qpDest++;
				*qpSource++;
			}
		break;
	}
}

//----------------------------------------------------------------------
// Brightness Copy
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특정한 Brightness값(s_Value1)만큼
// 밝게 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyBrightness(WORD* pDest, WORD* pSource, WORD pixels)
{
	int R,G,B;

	int i=pixels;

	while (i--)
	{
		R = ((CDirectDraw::Red(*pSource) + s_Value1) & 0x001F) << CDirectDraw::s_bSHIFT_R;
		G = ((CDirectDraw::Green(*pSource) + s_Value1) & 0x001F) << CDirectDraw::s_bSHIFT_G;
		B = (CDirectDraw::Blue(*pSource) + s_Value1) & 0x001F;
		
		*pDest = R | G | B;			
			
		pDest++;
		pSource++;
	}

	//*pDest = (R << CDirectDraw::s_bSHIFT_R) 
				//| (G << CDirectDraw::s_bSHIFT_G) 
				//| B;
	
}


//----------------------------------------------------------------------
// Effect Copy - Darker
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectDarker(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Darker : BG - SPR_Filter
	//------------------------------------------------------------
	int		darkR,
			darkG,
			darkB;
			
	while (i--)
	{
		darkR = Red(*pSource);
		darkG = Green(*pSource);
		darkB = Blue(*pSource);;

		*pDest = 
			(((Red(*pDest) * darkR) >> 5) << s_bSHIFT_R)
			| (((Green(*pDest) * darkG) >> 5) << s_bSHIFT_G)
			| ((Blue(*pDest) * darkB) >> 5);
			
		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - GrayScale
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectGrayScale(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Gray Scale : color value = (R+G+B)/3
	//------------------------------------------------------------
	BYTE average;

	while (i--)
	{
		average = ( Red(*pSource) +
					Green(*pSource) +
					Blue(*pSource) ) / 3;

		*pDest = (average << s_bSHIFT_R) 
				| (average << s_bSHIFT_G)
				| average;			
		
		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - Lighten
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectLighten(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Lighten : max(BG, SPR)
	//------------------------------------------------------------
	while (i--)
	{
		*pDest = 
			(max( Red(*pDest), Red(*pSource) ) << s_bSHIFT_R)
			| (max( Green(*pDest), Green(*pSource) ) << s_bSHIFT_G)
			| (max( Blue(*pDest), Blue(*pSource) ));
		
		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - Darken
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectDarken(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Darken : min(BG, SPR)
	//------------------------------------------------------------	
	while (i--)
	{

		*pDest = 
			(min( Red(*pDest), Red(*pSource) ) << s_bSHIFT_R)
			| (min( Green(*pDest), Green(*pSource) ) << s_bSHIFT_G)
			| (min( Blue(*pDest), Blue(*pSource) ));

		pDest++;
		pSource++;
	}
	
}

//----------------------------------------------------------------------
// Effect Copy - ColorDodge
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectColorDodge(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;
	
	//------------------------------------------------------------	
	// Color Dodge : (BG*MAXDAC)/(MAXDAC-SPR)
	//------------------------------------------------------------	
	while (i--)
	{

		*pDest = 
			((Red(*pDest) << 5) / (32-Red(*pSource)) << s_bSHIFT_R)
			| ((Green(*pDest) << 5) / (32-Green(*pSource)) << s_bSHIFT_G)
			| (Blue(*pDest) << 5) / (32-Blue(*pSource));
			

		pDest++;
		pSource++;
	}
	
}

//----------------------------------------------------------------------
// Effect Copy - Screen
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectScreen(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Screen :  TempSum=(MAXDAC-max(BG,SPR))/MAXDAC*min(BG,SPR)
	//           Screen=max(BG,SPR)+TempSum
	//------------------------------------------------------------	
	WORD temp;
	int d, s;
	//int m; 
	while (i--)
	{
		// R
		d = Red(*pDest);
		s = Red(*pSource);	

		//m = max(d,s);
		//temp = (((32 - m) * min(d,s) >> 5) + m) << s_bSHIFT_R;
		temp = s_EffectScreenTableR[d][s];

		// G
		d = Green(*pDest);
		s = Green(*pSource);

		//m = max(d,s);
		//temp |= (((32 - m) * min(d,s) >> 5) + m) << s_bSHIFT_G;
		temp |= s_EffectScreenTableG[d][s];
		
		// B
		d = Blue(*pDest);
		s = Blue(*pSource);

		//m = max(d,s);
		//temp |= ((32 - m) * min(d,s) >> 5) + m;
		temp |= s_EffectScreenTableB[d][s];

		// 
		*pDest = temp;

		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - Screen
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectScreenAlpha(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;
	
	//------------------------------------------------------------	
	// Screen :  TempSum=(MAXDAC-max(BG,SPR))/MAXDAC*min(BG,SPR)
	//           Screen=max(BG,SPR)+TempSum
	//------------------------------------------------------------	
	WORD temp;
	int d, s;
	int sr,sg,sb, dr,dg,db;
	//int m; 
	while (i--)
	{
		// R
		d = Red(*pDest);
		s = Red(*pSource);	
		
		//m = max(d,s);
		//temp = (((32 - m) * min(d,s) >> 5) + m) << s_bSHIFT_R;
		temp = s_EffectScreenTableR[d][s];
		
		// G
		d = Green(*pDest);
		s = Green(*pSource);
		
		//m = max(d,s);
		//temp |= (((32 - m) * min(d,s) >> 5) + m) << s_bSHIFT_G;
		temp |= s_EffectScreenTableG[d][s];
		
		// B
		d = Blue(*pDest);
		s = Blue(*pSource);
		
		//m = max(d,s);
		//temp |= ((32 - m) * min(d,s) >> 5) + m;
		temp |= s_EffectScreenTableB[d][s];
		
		// 
		// 한점 찍기
		sr = CDirectDraw::Red(temp);
		sg = CDirectDraw::Green(temp);
		sb = CDirectDraw::Blue(temp);
		
		dr = CDirectDraw::Red(*pDest);
		dg = CDirectDraw::Green(*pDest);
		db = CDirectDraw::Blue(*pDest);		
		
		*pDest = ((s_Value1 * (sb - db) >> 5) + db |
			((s_Value1 * (sg - dg) >> 5) + dg) << CDirectDraw::s_bSHIFT_G |
			((s_Value1 * (sr - dr) >> 5) + dr) << CDirectDraw::s_bSHIFT_R);
		
		pDest++;
		pSource++;
	}
}


//----------------------------------------------------------------------
// Effect Copy - DodgeBurn
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectDodgeBurn(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	//------------------------------------------------------------	
	// DodgeBurn = (BG*(MAXDAC-SPR))/MAXDAC
	//------------------------------------------------------------	
	while (i--)
	{

		*pDest = 		
			 (( (Red(*pDest) * (32 - Red(*pSource)) ) >> 5) << s_bSHIFT_R)
			| ((( Green(*pDest) * (32 - Green(*pSource))) >> 5) << s_bSHIFT_G)
			| ((Blue(*pDest) * (32 - Blue(*pSource))) >> 5);
			

		pDest++;
		pSource++;
	}

}

//----------------------------------------------------------------------
// Effect Copy - Different
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectDifferent(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Different=(max(BG,SPR)-min(BG,SPR))
	//------------------------------------------------------------	
	int temp;
	WORD d, s;
	
	while (i--)
	{
		temp = 0;

		d = Red(*pDest);
		s = Red(*pSource);
		temp |= (max(d,s)-min(d,s)) << 11;

		d = Green(*pDest);
		s = Green(*pSource);
		temp |= (max(d,s)-min(d,s)) << 5;

		d = Blue(*pDest);
		s = Blue(*pSource);
		temp |= max(d,s)-min(d,s);
		
		*pDest = temp;

		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - Gradation
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//
// s_Value1이 ColorSet번호이다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectGradation(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Different=(max(BG,SPR)-min(BG,SPR))
	//------------------------------------------------------------	
	WORD s;
	BYTE r, g, b;
	BYTE gradation;

	while (i--)
	{
		s = *pSource;

		r = s >> s_bSHIFT_R;		
		g = (s >> s_bSHIFT_G) & 0x1F;
		b = s & 0x1F;

		gradation = CIndexSprite::ColorToGradation[ r + g + b ];

		*pDest = CIndexSprite::ColorSet[s_Value1][gradation];

		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect SimpleOutline
//----------------------------------------------------------------------
// 좀 띄엄띄엄한 외곽선 출력(투명색 압축의 양 끝 점에 출력)
// pixels는 1이상이라고 가정한다.
//----------------------------------------------------------------------
void		
CSpriteSurface::memcpyEffectSimpleOutline(WORD* pDest, WORD* pSource, WORD pixels)
{
	// 첫 점
	*pDest = *pSource;
	
	int pixels_1 = pixels-1;

	// 끝 점 - pixels가 1일때 비교하는거보다 그냥 찍는게 더 빠를까?
	*(pDest+pixels_1) = *(pSource+pixels_1);	
}

//----------------------------------------------------------------------
// Effect WipeOut
//----------------------------------------------------------------------
// s_Value1 : 덜 출력할 수준? 
//				64 - 투명
//				0 - 다 출력
//
// 중심에서부터 양쪽으로 지워져간다.
//
// ***************
// ******   ******
//----------------------------------------------------------------------
void		
CSpriteSurface::memcpyEffectWipeOut(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	int skipPixels = (pixels * s_Value1) >> 6;	// / 64
	int drawPixels = (pixels - skipPixels)>>1;
	int drawPixels2 = pixels - drawPixels - skipPixels;
	
	// [1] drawPixels만큼 출력하고..
	// [2] skipPixels 만큼 건너띄고
	// [3] drawPixels2만큼 출력
	
	//------------------------------------------------------------	
	// drawPixels만큼 출력
	//------------------------------------------------------------	
	memcpy(pDest, pSource, (drawPixels<<1));
	pDest += drawPixels;
	pSource += drawPixels;

	//------------------------------------------------------------	
	// skipPixels만큼 건너띔
	//------------------------------------------------------------	
	pDest += skipPixels;
	pSource += skipPixels;

	//------------------------------------------------------------	
	// drawPixels2만큼 출력
	//------------------------------------------------------------	
	memcpy(pDest, pSource, (drawPixels2<<1));
	//pDest += drawPixels2;
	//pSource += drawPixels2;
}

//----------------------------------------------------------------------
// Effect Net
//----------------------------------------------------------------------
// s_Value1 : 건너띄는 점
//
// 한 점 찍고.. s_Value1의 점 수만큼 건너띈다.
//
// ***************
// * * * * * * * *	: s_Value1 = 1
// *  *  *  *  *    : s_Value1 = 2
// *   *   *   *    : s_Value1 = 3
//----------------------------------------------------------------------
void		
CSpriteSurface::memcpyEffectNet(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	int skipPixels = 1 + s_Value1;
	
	//------------------------------------------------------------	
	// drawPixels만큼 출력
	//------------------------------------------------------------	
	do {
		//memcpy(pDest, pSource, (drawPixels<<1));
		*pDest = *pSource;

		pDest += skipPixels;
		pSource += skipPixels;

		i -= skipPixels;
	} while (i > 0);
}

//----------------------------------------------------------------------
// Effect Copy - GrayScaleVarious
//----------------------------------------------------------------------
// s_Value1 은 얼마나 gray되는가?(-_-;)이다..
// s_Value1값은 0~31.. 32인가?.. - -
// 0이면 완전 gray
// 32이면 원래색깔
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyEffectGrayScaleVarious(WORD* pDest, WORD* pSource, WORD pixels)
{
	int i = pixels;

	//int grayValue = s_Value1;

	//------------------------------------------------------------	
	// Gray Scale : color value = (R+G+B)/3
	//------------------------------------------------------------
	int average;
	int r, g, b;

	while (i--)
	{	
		r = Red(*pSource);
		g = Green(*pSource);
		b = Blue(*pSource);

		average = ( r + g + b ) / 3;		// 0 ~ 31

		/*
		if (average==0)
		{
			*pDest = 0;
		}
		else
		{
		*/
			r = average + ((r-average)*s_Value1 >> 5);
			g = average + ((g-average)*s_Value1 >> 5);
			b = average + ((b-average)*s_Value1 >> 5);
		
			*pDest = (r << s_bSHIFT_R) 
					| (g << s_bSHIFT_G)
					| b;			
		//}
		
		pDest++;
		pSource++;
	}
}

















//----------------------------------------------------------------------
// Effect Copy - Darker
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectDarker(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Darker : BG - SPR_Filter
	//------------------------------------------------------------
	int		darkR,
			darkG,
			darkB;
			
	while (i--)
	{
		darkR = Red(pal[*pSource]);
		darkG = Green(pal[*pSource]);
		darkB = Blue(pal[*pSource]);;

		*pDest = 
			(((Red(*pDest) * darkR) >> 5) << s_bSHIFT_R)
			| (((Green(*pDest) * darkG) >> 5) << s_bSHIFT_G)
			| ((Blue(*pDest) * darkB) >> 5);
			
		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - GrayScale
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectGrayScale(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Gray Scale : color value = (R+G+B)/3
	//------------------------------------------------------------
	BYTE average;

	while (i--)
	{
		average = ( Red(pal[*pSource]) +
					Green(pal[*pSource]) +
					Blue(pal[*pSource]) ) / 3;

		*pDest = (average << s_bSHIFT_R) 
				| (average << s_bSHIFT_G)
				| average;			
		
		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - Lighten
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectLighten(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Lighten : max(BG, SPR)
	//------------------------------------------------------------
	while (i--)
	{
		*pDest = 
			(max( Red(*pDest), Red(pal[*pSource]) ) << s_bSHIFT_R)
			| (max( Green(*pDest), Green(pal[*pSource]) ) << s_bSHIFT_G)
			| (max( Blue(*pDest), Blue(pal[*pSource]) ));
		
		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - Darken
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectDarken(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Darken : min(BG, SPR)
	//------------------------------------------------------------	
	while (i--)
	{

		*pDest = 
			(min( Red(*pDest), Red(pal[*pSource]) ) << s_bSHIFT_R)
			| (min( Green(*pDest), Green(pal[*pSource]) ) << s_bSHIFT_G)
			| (min( Blue(*pDest), Blue(pal[*pSource]) ));

		pDest++;
		pSource++;
	}
	
}

//----------------------------------------------------------------------
// Effect Copy - ColorDodge
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectColorDodge(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;
	
	//------------------------------------------------------------	
	// Color Dodge : (BG*MAXDAC)/(MAXDAC-SPR)
	//------------------------------------------------------------	
	while (i--)
	{

		*pDest = 
			((Red(*pDest) << 5) / (32-Red(pal[*pSource])) << s_bSHIFT_R)
			| ((Green(*pDest) << 5) / (32-Green(pal[*pSource])) << s_bSHIFT_G)
			| (Blue(*pDest) << 5) / (32-Blue(pal[*pSource]));
			

		pDest++;
		pSource++;
	}
	
}

//----------------------------------------------------------------------
// Effect Copy - Screen
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectScreen(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Screen :  TempSum=(MAXDAC-max(BG,SPR))/MAXDAC*min(BG,SPR)
	//           Screen=max(BG,SPR)+TempSum
	//------------------------------------------------------------	
	WORD temp;
	int d, s;
	//int m; 
	while (i--)
	{
		// R
		d = Red(*pDest);
		s = Red(pal[*pSource]);	

		//m = max(d,s);
		//temp = (((32 - m) * min(d,s) >> 5) + m) << s_bSHIFT_R;
		temp = s_EffectScreenTableR[d][s];

		// G
		d = Green(*pDest);
		s = Green(pal[*pSource]);

		//m = max(d,s);
		//temp |= (((32 - m) * min(d,s) >> 5) + m) << s_bSHIFT_G;
		temp |= s_EffectScreenTableG[d][s];
		
		// B
		d = Blue(*pDest);
		s = Blue(pal[*pSource]);

		//m = max(d,s);
		//temp |= ((32 - m) * min(d,s) >> 5) + m;
		temp |= s_EffectScreenTableB[d][s];

		// 
		*pDest = temp;

		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - Screen
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectScreenAlpha(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
}

//----------------------------------------------------------------------
// Effect Copy - DodgeBurn
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectDodgeBurn(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	//------------------------------------------------------------	
	// DodgeBurn = (BG*(MAXDAC-SPR))/MAXDAC
	//------------------------------------------------------------	
	while (i--)
	{

		*pDest = 		
			 (( (Red(*pDest) * (32 - Red(pal[*pSource])) ) >> 5) << s_bSHIFT_R)
			| ((( Green(*pDest) * (32 - Green(pal[*pSource]))) >> 5) << s_bSHIFT_G)
			| ((Blue(*pDest) * (32 - Blue(pal[*pSource]))) >> 5);
			

		pDest++;
		pSource++;
	}

}

//----------------------------------------------------------------------
// Effect Copy - Different
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectDifferent(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Different=(max(BG,SPR)-min(BG,SPR))
	//------------------------------------------------------------	
	int temp;
	WORD d, s;
	
	while (i--)
	{
		temp = 0;

		d = Red(*pDest);
		s = Red(pal[*pSource]);
		temp |= (max(d,s)-min(d,s)) << 11;

		d = Green(*pDest);
		s = Green(pal[*pSource]);
		temp |= (max(d,s)-min(d,s)) << 5;

		d = Blue(*pDest);
		s = Blue(pal[*pSource]);
		temp |= max(d,s)-min(d,s);
		
		*pDest = temp;

		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect Copy - Gradation
//----------------------------------------------------------------------
// source --> dest 로 pixels만큼 특수효과 처리를 한다.
//
// s_Value1이 ColorSet번호이다.
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectGradation(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	//------------------------------------------------------------	
	// Different=(max(BG,SPR)-min(BG,SPR))
	//------------------------------------------------------------	
	WORD s;
	BYTE r, g, b;
	BYTE gradation;

	while (i--)
	{
		s = pal[*pSource];

		r = s >> s_bSHIFT_R;		
		g = (s >> s_bSHIFT_G) & 0x1F;
		b = s & 0x1F;

		gradation = CIndexSprite::ColorToGradation[ r + g + b ];

		*pDest = CIndexSprite::ColorSet[s_Value1][gradation];

		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Effect SimpleOutline
//----------------------------------------------------------------------
// 좀 띄엄띄엄한 외곽선 출력(투명색 압축의 양 끝 점에 출력)
// pixels는 1이상이라고 가정한다.
//----------------------------------------------------------------------
void		
CSpriteSurface::memcpyPalEffectSimpleOutline(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	// 첫 점
	*pDest = pal[*pSource];
	
	int pixels_1 = pixels-1;

	// 끝 점 - pixels가 1일때 비교하는거보다 그냥 찍는게 더 빠를까?
	*(pDest+pixels_1) = *(pSource+pixels_1);	
}

//----------------------------------------------------------------------
// Effect WipeOut
//----------------------------------------------------------------------
// s_Value1 : 덜 출력할 수준? 
//				64 - 투명
//				0 - 다 출력
//
// 중심에서부터 양쪽으로 지워져간다.
//
// ***************
// ******   ******
//----------------------------------------------------------------------
void		
CSpriteSurface::memcpyPalEffectWipeOut(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	int skipPixels = (pixels * s_Value1) >> 6;	// / 64
	int drawPixels = (pixels - skipPixels)>>1;
	int drawPixels2 = pixels - drawPixels - skipPixels;
	
	// [1] drawPixels만큼 출력하고..
	// [2] skipPixels 만큼 건너띄고
	// [3] drawPixels2만큼 출력
	
	//------------------------------------------------------------	
	// drawPixels만큼 출력
	//------------------------------------------------------------	
	memcpy(pDest, pSource, (drawPixels<<1));
	pDest += drawPixels;
	pSource += drawPixels;

	//------------------------------------------------------------	
	// skipPixels만큼 건너띔
	//------------------------------------------------------------	
	pDest += skipPixels;
	pSource += skipPixels;

	//------------------------------------------------------------	
	// drawPixels2만큼 출력
	//------------------------------------------------------------	
	memcpy(pDest, pSource, (drawPixels2<<1));
	//pDest += drawPixels2;
	//pSource += drawPixels2;
}

//----------------------------------------------------------------------
// Effect Net
//----------------------------------------------------------------------
// s_Value1 : 건너띄는 점
//
// 한 점 찍고.. s_Value1의 점 수만큼 건너띈다.
//
// ***************
// * * * * * * * *	: s_Value1 = 1
// *  *  *  *  *    : s_Value1 = 2
// *   *   *   *    : s_Value1 = 3
//----------------------------------------------------------------------
void		
CSpriteSurface::memcpyPalEffectNet(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	int skipPixels = 1 + s_Value1;
	
	//------------------------------------------------------------	
	// drawPixels만큼 출력
	//------------------------------------------------------------	
	do {
		//memcpy(pDest, pSource, (drawPixels<<1));
		*pDest = pal[*pSource];

		pDest += skipPixels;
		pSource += skipPixels;

		i -= skipPixels;
	} while (i > 0);
}

//----------------------------------------------------------------------
// Effect Copy - GrayScaleVarious
//----------------------------------------------------------------------
// s_Value1 은 얼마나 gray되는가?(-_-;)이다..
// s_Value1값은 0~31.. 32인가?.. - -
// 0이면 완전 gray
// 32이면 원래색깔
//----------------------------------------------------------------------
void	
CSpriteSurface::memcpyPalEffectGrayScaleVarious(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	int i = pixels;

	//int grayValue = s_Value1;

	//------------------------------------------------------------	
	// Gray Scale : color value = (R+G+B)/3
	//------------------------------------------------------------
	int average;
	int r, g, b;

	while (i--)
	{	
		r = Red(pal[*pSource]);
		g = Green(pal[*pSource]);
		b = Blue(pal[*pSource]);

		average = ( r + g + b ) / 3;		// 0 ~ 31

		/*
		if (average==0)
		{
			*pDest = 0;
		}
		else
		{
		*/
			r = average + ((r-average)*s_Value1 >> 5);
			g = average + ((g-average)*s_Value1 >> 5);
			b = average + ((b-average)*s_Value1 >> 5);
		
			*pDest = (r << s_bSHIFT_R) 
					| (g << s_bSHIFT_G)
					| b;			
		//}
		
		pDest++;
		pSource++;
	}
}





















//----------------------------------------------------------------------
// Init EffectTable
//----------------------------------------------------------------------
void		
CSpriteSurface::InitEffectTable()
{
	WORD temp;
	WORD m1, m2;

	for (int d=0; d<32; d++)
	{
		for (int s=0; s<32; s++)
		{
			m1 = max(d,s);
			m2 = min(d,s);

			// R			
			temp = (((32 - m1) * m2 >> 5) + m1) << s_bSHIFT_R;
			s_EffectScreenTableR[d][s] = temp;

			// G
			temp = (((32 - m1) * m2 >> 5) + m1) << s_bSHIFT_G;
			s_EffectScreenTableG[d][s] = temp;
			
			// B
			temp = ((32 - m1) * m2 >> 5) + m1;
			s_EffectScreenTableB[d][s] = temp;
		}
	}

}


//----------------------------------------------------------------------
// Blt IndexSprite Darkness
//----------------------------------------------------------------------
// pPoint에 pSprite를 출력한다.
//----------------------------------------------------------------------
void	
CSpriteSurface::BltIndexSpriteBrightness(POINT* pPoint, CIndexSprite* pSprite, BYTE BrightBits)
{	
	// Sprite가 아직 생성되지 않은 경우..
	if (pSprite->IsNotInit())
		return;

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 1
	//-------------------------------------------------------------
	if (pPoint->x >= m_ClipRight || pPoint->y >= m_ClipBottom)
		return;
	// Faz G1 (2026-09-01) - dx11gpu dirty bolge (CPU yolu; sirali segment icin)
	G1_Mark(m_ddsd.lpSurface, pPoint->x, pPoint->y,
	        pSprite->GetWidth(), pSprite->GetHeight());


	POINT pt = *pPoint;
	

	WORD*	lpSurface	= (WORD*)m_ddsd.lpSurface;
	WORD	Pitch		= m_ddsd.lPitch;
	RECT	rect;

	// sprite내부의 출력 영역
	rect.left = 0;
	rect.top = 0;
	rect.right = pSprite->GetWidth();
	rect.bottom = pSprite->GetHeight();


	// Lock
	//LockW(lpSurface, Pitch);

	int xPlusSpriteWidth	= pt.x + pSprite->GetWidth();
	int yPlusSpriteHeight	= pt.y + pSprite->GetHeight();

	//-------------------------------------------------------------
	// 화면에 보이지 않는 경우 2
	//-------------------------------------------------------------
	if (xPlusSpriteWidth <= m_ClipLeft || yPlusSpriteHeight <= m_ClipTop)
		return;

	//-------------------------------------------------------------
	// Clipping이 필요한 부분에 대한 check
	//-------------------------------------------------------------
	bool	leftClip	= (pt.x < m_ClipLeft) && (xPlusSpriteWidth > m_ClipLeft);
	bool	rightClip	= (xPlusSpriteWidth > m_ClipRight);
	bool	topClip		= (pt.y < m_ClipTop) && (yPlusSpriteHeight > m_ClipTop);
	bool	bottomClip	= (yPlusSpriteHeight > m_ClipBottom);

	//------------------------------------------
	// 왼쪽 Clipping이 필요한 상태..
	//------------------------------------------
	if (leftClip)
	{
		// 왼쪽으로 들어간만큼은 생략해서 출력해야 한다.
		rect.left = -pt.x + m_ClipLeft;    
		//pt.x = m_ClipLeft;		
		
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;			

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}

			// 왼쪽 + 오른쪽			
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltBrightnessClipWidth(lpSurface, Pitch, &rect, BrightBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch);
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}				
			}

			// 왼쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (m_ClipLeft<<1));
			pSprite->BltBrightnessClipLeft(lpSurface, Pitch, &rect, BrightBits);
		}
	}
	//------------------------------------------
	// 왼쪽 Clipping이 필요 없는 상태..
	//------------------------------------------
	else
	{
		//------------------------------------------
		// 오른쪽 Clipping이 필요한 상태
		//------------------------------------------
		if (rightClip)
		{	
			// 가로 길이를 줄인다.
			rect.right = m_ClipRight - pt.x;

			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{				
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{						
					rect.bottom = m_ClipBottom - pt.y;
				}				

				pt.y = m_ClipTop;
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{					
					rect.bottom = m_ClipBottom - pt.y;										
				}								
			}	
			
			// 오른쪽 Clip
			lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
			pSprite->BltBrightnessClipRight(lpSurface, Pitch, &rect, BrightBits);
		}
		//------------------------------------------
		// 오른쪽 Clipping이 필요 없는 상태
		//------------------------------------------
		else
		{
			//------------------------------------------
			// 위쪽 Clipping이 필요한 상태
			//------------------------------------------
			if (topClip)
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + (pt.x<<1));
				rect.top = -pt.y + m_ClipTop;   				

				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
				}

				pt.y = m_ClipTop;
				
				// Height만 Clip
				lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				pSprite->BltBrightnessClipHeight(lpSurface, Pitch, &rect, BrightBits);
			}
			else
			{
				//lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
				//------------------------------------------
				// 아래쪽 Clipping이 필요한 상태
				//------------------------------------------
				if (bottomClip)
				{
					rect.bottom = m_ClipBottom - pt.y;					
					// Height만 Clip
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltBrightnessClipHeight(lpSurface, Pitch, &rect, BrightBits);
				}
				else
				{
					// No Clip					
					lpSurface = (WORD*)((BYTE*)lpSurface + pt.y*Pitch + (pt.x<<1));
					pSprite->BltBrightness(lpSurface, Pitch, BrightBits);
				}				
			}			
			
		}		
	}
	

	//Unlock();
}
