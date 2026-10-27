#include "Client_PCH.h"
#define MOTORCYCLE_LIGHT		2

//----------------------------------------------------------------------
//
// Define Function��
//
//----------------------------------------------------------------------
// ó��.. - -;;
//----------------------------------------------------------------------
// DRAW_ALPHASPRITE
//----------------------------------------------------------------------
#define DRAW_ALPHASPRITE(pPoint, spriteID, ASPK, ASPKI, ASPKFile)	\
		{																\
			CAlphaSprite* pSprite = &ASPK[ spriteID ];					\
																		\
			if (pSprite->IsNotInit())									\
			{															\
				ASPKFile.seekg( ASPKI[spriteID] );						\
				pSprite->LoadFromFile( ASPKFile );						\
			}															\
																		\
			m_pSurface->BltAlphaSprite(pPoint, pSprite);				\
		}

//----------------------------------------------------------------------
// DRAW_ALPHASPRITE
//----------------------------------------------------------------------
#define DRAW_ALPHASPRITEPAL(pPoint, spriteID, ASPK, PAL)	\
		{																\
			CAlphaSpritePal* pSprite = &ASPK[ spriteID ];					\
																		\
			m_pSurface->BltAlphaSpritePal(pPoint, pSprite, PAL);		\
		}

//----------------------------------------------------------------------
// DRAW_NORMALSPRITE
//----------------------------------------------------------------------
#define DRAW_NORMALSPRITE(pPoint, spriteID, SPK, SPKI, SPKFile)			\
		{																\
			CSprite* pSprite = &ASPK[ spriteID ];						\
																		\
			if (pSprite->IsNotInit())									\
			{															\
				SPKFile.seekg( SPKI[spriteID] );						\
				pSprite->LoadFromFile( SPKFile );						\
			}															\
																		\
			m_pSurface->BltSprite(pPoint, pSprite);						\
		}

//----------------------------------------------------------------------
// DRAW_NORMALSPRITE_EFFECT
//----------------------------------------------------------------------
#define DRAW_NORMALSPRITE_EFFECT(pPoint, spriteID, SPK, SPKI, SPKFile, EffectType)	\
		{																\
			CSprite* pSprite = &SPK[ spriteID ];						\
																		\
			if (pSprite->IsNotInit())									\
			{															\
				SPKFile.seekg( SPKI[spriteID] );						\
				pSprite->LoadFromFile( SPKFile );						\
			}															\
																		\
			CSpriteSurface::SetEffect( EffectType );					\
			m_pSurface->BltSpriteEffect(pPoint, pSprite);				\
		}

//----------------------------------------------------------------------
// DRAW_NORMALSPRITEPAL_EFFECT
//----------------------------------------------------------------------
#define DRAW_NORMALSPRITEPAL_EFFECT(pPoint, spriteID, SPK, pal, EffectType)	\
		{																\
			CSpritePal* pSprite = &SPK[ spriteID ];						\
																		\
			CSpriteSurface::SetPalEffect( EffectType );					\
			m_pSurface->BltSpritePalEffect(pPoint, pSprite, pal);		\
		}

//----------------------------------------------------------------------
// DRAW_TEXTURE_SPRITE
//
// Fix #20 (2026-05-15): D3D9 native port FAZ 3 — D3D9 modunda
// CDirect3D::GetDevice() (LPDIRECT3DDEVICE7) NULL doner (m_pD3DD legacy
// pointer Phase 4.5'te NULL kaldi; D3D9 device ownership D3D9Backend
// singleton'da). NULL deref crash'i engellemek icin macro icine NULL
// guard eklendi. DX7 fallback (PHOENIX_RENDER_BACKEND_D3D9=0) etkilenmez:
// IDirect3DDevice7 valid pointer doner, GetDevice()!=NULL gecer.
//
// D3D9 modunda 3D texture path silent skip — 3D entity render Phase 4.6'ya
// devir, sprite codec 2D path zaten composition buffer'a yaziyor.
//----------------------------------------------------------------------
#define DRAW_TEXTURE_SPRITE(x, y, spriteID, pTextureManager)					\
		{																		\
			LPDIRECT3DDEVICE7 _pPhxDev = CDirect3D::GetDevice();				\
			CSpriteSurface* pSurface = pTextureManager->GetTexture(spriteID);	\
																				\
			if (pSurface!=NULL && _pPhxDev!=NULL)								\
			{																	\
				if (_pPhxDev->BeginScene()==D3D_OK)								\
				{																\
					_pPhxDev->SetTexture(0, pSurface->GetSurface());			\
																				\
					RECT rect =													\
					{															\
						x,														\
						y,														\
						x + pTextureManager->GetWidth(spriteID),				\
						y + pTextureManager->GetHeight(spriteID)				\
					};															\
																				\
					m_TextureEffect.DrawEffect2D( &rect );						\
																				\
					_pPhxDev->EndScene();										\
																				\
					_pPhxDev->SetTexture(0, NULL);								\
				}																\
			}																	\
		}

//----------------------------------------------------------------------
// DRAW_TEXTURE_SPRITE_LOCKED
//
// Fix #20: Inner DRAW_TEXTURE_SPRITE NULL-guard'lı; Unlock/Lock cycle
// composition buffer'i etkiler (CDirectDrawSurface public API delegate-
// aware), D3D9'da safe.
//----------------------------------------------------------------------
#define DRAW_TEXTURE_SPRITE_LOCKED(x, y, spriteID, pTextureManager)				\
			m_pSurface->Unlock();												\
																				\
			DRAW_TEXTURE_SPRITE(x, y, spriteID, pTextureManager)				\
																				\
			m_pSurface->Lock();

////----------------------------------------------------------------------
//// DRAW_TEXTURE_SPRITE
////----------------------------------------------------------------------
//#define DRAW_TEXTURE_SPRITEPAL(x, y, spriteID, pTextureManager, pal)				\
//		{																			\
//			CSpriteSurface* pSurface = pTextureManager->GetTexture(spriteID, pal);	\
//																				\
//			if (pSurface!=NULL)													\
//			{																	\
//				if (CDirect3D::GetDevice()->BeginScene()==D3D_OK)				\
//				{																\
//					CDirect3D::GetDevice()->SetTexture(0, pSurface->GetSurface());	\
//																				\
//					RECT rect =													\
//					{															\
//						x,														\
//						y,														\
//						x + pTextureManager->GetWidth(spriteID),				\
//						y + pTextureManager->GetHeight(spriteID)				\
//					};															\
//																				\
//					m_TextureEffect.DrawEffect2D( &rect );						\
//																				\
//					CDirect3D::GetDevice()->EndScene();							\
//																				\
//					CDirect3D::GetDevice()->SetTexture(0, NULL);				\
//				}																\
//			}																	\
//		}
//
////----------------------------------------------------------------------
//// DRAW_TEXTURE_SPRITE_LOCKED
////----------------------------------------------------------------------
//#define DRAW_TEXTURE_SPRITEPAL_LOCKED(x, y, spriteID, pTextureManager, pal)		\
//			m_pSurface->Unlock();												\
//																				\
//			DRAW_TEXTURE_SPRITEPAL(x, y, spriteID, pTextureManager, pal)			\
//																				\
//			m_pSurface->Lock();

//----------------------------------------------------------------------
// DRAW_TEXTURE_SURFACE
//
// Fix #20 (2026-05-15): D3D9 native port FAZ 3 — D3D9 modunda
// CDirect3D::GetDevice() NULL. MTopView.cpp:13443 DrawLightBuffer3D
// icinde aktif callsite (CDirect3D::IsHAL() D3D9'da TRUE doner — Phase
// 4.5 — yani 3D path'e girer ve burayi cagirir → NULL deref crash).
// NULL guard ile D3D9 modunda sessiz skip. 3D light buffer render
// Phase 4.6'ya devir; 2D yol (DrawLightBuffer2D) zaten composition
// buffer'a sysmem write yapiyor. Kullanici karanlik bolgelerde light
// filter goremez ama crash etmez.
//
// DX7 fallback (PHOENIX_RENDER_BACKEND_D3D9=0): IDirect3DDevice7 valid,
// GetDevice()!=NULL gecer, eski davranis aynen calisir.
//
// !! MACRO BODY: callsite (MTopView.cpp:13443) `DRAW_TEXTURE_SURFACE(...)
// else { ... }` pattern'i kullaniyor — macro outermost `if(...) { ... }`
// olmali ki dangling else valid kalsin. NULL guard short-circuit ile
// tek `if`-condition'da birlestirildi.
//----------------------------------------------------------------------
#define	DRAW_TEXTURE_SURFACE( pTextureSurface, pRect )							\
			if (CDirect3D::GetDevice()!=NULL &&									\
			    CDirect3D::GetDevice()->BeginScene()==D3D_OK)					\
			{																	\
				CDirect3D::GetDevice()->SetTexture(0, pTextureSurface->GetSurface());	\
																				\
				m_TextureEffect.DrawEffect2D( pRect );							\
																				\
				CDirect3D::GetDevice()->EndScene();								\
																				\
				CDirect3D::GetDevice()->SetTexture(0, NULL);					\
			}

//----------------------------------------------------------------------
// GET_EFFECTSPRITETYPE_xxxxx
//----------------------------------------------------------------------
#define	GET_EFFECTSPRITETYPE_SCREEN( frameID )			\
		(frameID + MAX_EFFECTSPRITETYPE_ALPHAEFFECT)

#define	GET_EFFECTSPRITETYPE_EFFECT( frameID )			\
		(frameID)

#define	GET_EFFECTSPRITETYPE_NORMAL( frameID )			\
		(frameID + MAX_EFFECTSPRITETYPE_SCREENEFFECT)

#define	GET_EFFECTSPRITETYPE_SHADOW( frameID )			\
		(frameID + MAX_EFFECTSPRITETYPE_NORMALEFFECT)

//----------------------------------------------------------------------
// DRAW_SPRITE_WITH_EFFECTFRAME
//----------------------------------------------------------------------
#define	DRAW_SPRITE_WITH_EFFECTFRAME( pPoint, EffectSPK, EffectFPK, frameID, direction, frame )	\
		{																						\
			CFrame& Frame = EffectFPK[frameID][direction][frame];								\
			int sprite = Frame.GetSpriteID();													\
																								\
			if (sprite < EffectSPK.GetSize())													\
			{																					\
				POINT pointTemp = *pPoint;														\
				pointTemp.x += Frame.GetCX();													\
				pointTemp.y += Frame.GetCY();													\
																								\
				m_pSurface->BltSprite(&pointTemp, &EffectSPK[ sprite ]);						\
			}																					\
		}

//----------------------------------------------------------------------
// HAS_PAIR_EFFECTSPRITETYPE
//----------------------------------------------------------------------
#define	HAS_PAIR_EFFECTSPRITETYPE(est)	((*g_pEffectSpriteTypeTable)[est].PairFrameIDList.size())

//----------------------------------------------------------------------
// HAS_ACTION_EFFECTSPRITETYPE
//----------------------------------------------------------------------
#define	HAS_ACTION_EFFECTSPRITETYPE(est)	((*g_pEffectSpriteTypeTable)[est].ActionEffectFrameID!=FRAMEID_NULL)

//----------------------------------------------------------------------
// GET_ACTION_EFFECTSPRITETYPE
//----------------------------------------------------------------------
#define	GET_ACTION_EFFECTSPRITETYPE(est)	((*g_pEffectSpriteTypeTable)[est].ActionEffectFrameID)

//----------------------------------------------------------------------
// GET_ACTION_EFFECTSPRITETYPE_FRAMEID
//----------------------------------------------------------------------
#define	GET_ACTION_EFFECTSPRITETYPE_FRAMEID(aest, action)	((*g_pActionEffectSpriteTypeTable)[aest][action].FrameID)

//----------------------------------------------------------------------
// DRAW_SPRITE_WITH_EFFECTFRAME_PAIR
//----------------------------------------------------------------------
#define DRAW_SPRITE_WITH_EFFECTFRAME_PAIR( est, pPoint, EffectSPK, EffectFPK, direction, frame )		\
		if (HAS_PAIR_EFFECTSPRITETYPE(est))																\
		{																								\
			EFFECTSPRITETYPETABLE_INFO::FRAMEID_LIST& idList = (*g_pEffectSpriteTypeTable)[est].PairFrameIDList;	\
			if(!idList.empty())																			\
			{																							\
				EFFECTSPRITETYPETABLE_INFO::FRAMEID_LIST::iterator iID = idList.begin();				\
																										\
				while (iID != idList.end())																\
				{																						\
					int frameID = *iID;																	\
																										\
					DRAW_SPRITE_WITH_EFFECTFRAME(pPoint, EffectSPK, EffectFPK, frameID, direction, frame )	\
																										\
					iID++;																				\
				}																						\
			}																							\
		}
	
//----------------------------------------------------------------------
// ADD_MOTORCYCLE_LIGHT_2D
//----------------------------------------------------------------------
#define ADD_MOTORCYCLE_LIGHT_2D( pCreature, bForceLight )						\
		if (pCreature->GetMoveDevice()==MCreature::MOVE_DEVICE_RIDE				\
			&& !pCreature->HasEffectStatus(EFFECTSTATUS_YELLOW_POISON_TO_CREATURE))	\
		{																		\
			int pX = pCreature->GetPixelX() - m_FirstZonePixel.x + TILE_X_HALF;	\
			int pY = pCreature->GetPixelY() - m_FirstZonePixel.y - TILE_Y;		\
																				\
			POINT pt = GetChangeValueToDirection(pCreature->GetDirection());	\
																				\
			AddLightFilter2D( pX + pt.x*(TILE_X<<2)			\
								+ TILE_X_HALF,				\
								pY - pCreature->GetZ() + pt.y*(TILE_Y<<2)	\
								+ TILE_Y_HALF,				\
								MOTORCYCLE_LIGHT,								\
								false,											\
								bForceLight);									\
		}

//----------------------------------------------------------------------
// ADD_MOTORCYCLE_LIGHT_3D
//----------------------------------------------------------------------
#define ADD_MOTORCYCLE_LIGHT_3D( pCreature, bForceLight )						\
		if (pCreature->GetMoveDevice()==MCreature::MOVE_DEVICE_RIDE				\
			&& !pCreature->HasEffectStatus(EFFECTSTATUS_YELLOW_POISON_TO_CREATURE))	\
		{																		\
			int pX = pCreature->GetPixelX() - m_FirstZonePixel.x + TILE_X_HALF;	\
			int pY = pCreature->GetPixelY() - m_FirstZonePixel.y - TILE_Y;		\
																				\
			POINT pt = GetChangeValueToDirection(pCreature->GetDirection());	\
																				\
			AddLightFilter3D( pX + pt.x*(TILE_X<<2)			\
								+ TILE_X_HALF,				\
								pY - pCreature->GetZ() + pt.y*(TILE_Y<<2)	\
								+ TILE_Y_HALF,				\
								MOTORCYCLE_LIGHT,								\
								false,											\
								bForceLight);									\
		}

//----------------------------------------------------------------------
// ADD_MOTORCYCLE_LIGHT_XY_2D
//----------------------------------------------------------------------
#define ADD_MOTORCYCLE_LIGHT_XY_2D( pCreature, pX, pY, bForceLight )			\
		if (pCreature->GetMoveDevice()==MCreature::MOVE_DEVICE_RIDE				\
			&& !pCreature->HasEffectStatus(EFFECTSTATUS_YELLOW_POISON_TO_CREATURE))	\
		{																		\
			POINT pt = GetChangeValueToDirection(pCreature->GetDirection());	\
																				\
			AddLightFilter2D( pX + pt.x*(TILE_X<<2)			\
								+ TILE_X_HALF,				\
								pY - pCreature->GetZ() + pt.y*(TILE_Y<<2)	\
								+ TILE_Y_HALF,				\
								MOTORCYCLE_LIGHT,								\
								false,											\
								bForceLight);									\
		}

//----------------------------------------------------------------------
// ADD_MOTORCYCLE_LIGHT_XY_3D
//----------------------------------------------------------------------
#define ADD_MOTORCYCLE_LIGHT_XY_3D( pCreature, pX, pY, bForceLight )			\
		if (pCreature->GetMoveDevice()==MCreature::MOVE_DEVICE_RIDE				\
			&& !pCreature->HasEffectStatus(EFFECTSTATUS_YELLOW_POISON_TO_CREATURE))	\
		{																		\
			POINT pt = GetChangeValueToDirection(pCreature->GetDirection());	\
																				\
			AddLightFilter3D( pX + pt.x*(TILE_X<<2)			\
								+ TILE_X_HALF,				\
								pY - pCreature->GetZ() + pt.y*(TILE_Y<<2)	\
								+ TILE_Y_HALF,				\
								MOTORCYCLE_LIGHT,								\
								false,											\
								bForceLight);									\
		}


//----------------------------------------------------------------------
// DRAW_ALPHA_BOX_2D
//----------------------------------------------------------------------
#define DRAW_ALPHA_BOX_2D( rect, boxColor )						\
		if (boxColor==0)										\
		{														\
			if (CDirectDraw::Is565())							\
			{													\
				m_pSurface->GammaBox565(&rect, 15);				\
			}													\
			else												\
			{													\
				m_pSurface->GammaBox555(&rect, 15);				\
			}													\
		}														\
		else													\
		{														\
			m_pSurface->BltColorAlpha(&rect, boxColor, 15);		\
		}

//----------------------------------------------------------------------
// DRAW_CHAT_BOX_OUTLINE
//----------------------------------------------------------------------
#define	MIN_CHAT_BOX_WIDTH	60
#define CHAT_BOX_TAIL_X		29				// ä�� ����ǥ �ٴ� x��ġ
#define MIN_CHAT_LARGE_BOX_WIDTH	90

#define DRAW_CHAT_BOX_OUTLINE( rect, color )								\
		{																	\
			int left = rect.left+1;											\
			int top = rect.top+1;											\
																			\
			int width = abs(rect.right - rect.left);						\
			int height = abs(rect.bottom - rect.top);						\
																			\
			if (rect.top-1 > 0)												\
			{																\
				m_pSurface->HLine(left-1, rect.top-1, width, color);		\
			}																\
																			\
			if (rect.bottom < CLIPSURFACE_HEIGHT)			\
			{																\
				m_pSurface->HLine(left-1, rect.bottom, CHAT_BOX_TAIL_X, color);		\
				m_pSurface->HLine(left+CHAT_BOX_TAIL_X-1+6, rect.bottom, width-CHAT_BOX_TAIL_X-6, color);	\
			}																\
																			\
			if (rect.left >= 0)												\
			{																\
				m_pSurface->VLine(rect.left-1, top-1, height, color);		\
			}																\
																			\
			if (rect.right < CLIPSURFACE_WIDTH)			\
			{																\
				m_pSurface->VLine(rect.right, top-1, height, color);		\
			}																\
		}


//----------------------------------------------------------------------
// DRAW_CHAT_BOX_TAIL ( ���� �ڽ�, ���� )
//----------------------------------------------------------------------
#define DRAW_CHAT_BOX_TAIL_LOCKED( rect, color )					\
		{															\
			WORD *lpSurface, pitch_2;								\
			lpSurface = (WORD*)m_pSurface->GetSurfacePointer();		\
			pitch_2 = m_pSurface->GetSurfacePitch() >> 1;			\
																	\
			int x = rect.left + CHAT_BOX_TAIL_X;					\
			int y = rect.bottom;									\
																	\
			const int ptNum = 13;									\
			POINT pt[ptNum] =										\
			{														\
				{ x, y+1 },											\
				{ x+5, y+1 },										\
				{ x, y+2 },											\
				{ x+4, y+2 },										\
				{ x+1, y+3 },										\
				{ x+4, y+3 },										\
				{ x+1, y+4 },										\
				{ x+3, y+4 },										\
				{ x+1, y+5 },										\
				{ x+3, y+5 },										\
				{ x+2, y+6 },										\
				{ x+2, y+7 },										\
				{ x+2, y+8 }										\
			};														\
																	\
			if (x>=0 && y+1>=0										\
					&& x+5<CLIPSURFACE_WIDTH		\
					&& y+8<CLIPSURFACE_HEIGHT)	\
			{														\
				for (int i=0; i<ptNum; i++)							\
				{													\
					*(lpSurface + pitch_2*pt[i].y + pt[i].x) = color;	\
				}														\
			}															\
			else														\
			{															\
				for (int i=0; i<ptNum; i++)								\
				{														\
					int x = pt[i].x;									\
					int y = pt[i].y;									\
																		\
					if (x>=0 && y>=0									\
						&& x<CLIPSURFACE_WIDTH		\
						&& y<CLIPSURFACE_HEIGHT)		\
					{													\
						*(lpSurface + pitch_2*y + x) = color;			\
					}													\
				}														\
			}															\
			/* Faz G5 (2026-09-01) - dx11gpu: ham kuyruk yazisi dirty */	\
			{															\
				phoenix::render::D3D11SpriteCommands& g5cmds =			\
					phoenix::render::D3D11SpriteCommands::Instance();	\
				if (g5cmds.IsActive() &&								\
					g5cmds.IsCompositionTarget(lpSurface))				\
				{														\
					RECT g5dirty;										\
					g5dirty.left   = x;									\
					g5dirty.top    = y + 1;								\
					g5dirty.right  = x + 6;								\
					g5dirty.bottom = y + 9;								\
					g5cmds.MarkDirty(&g5dirty);							\
				}														\
			}															\
		}

//----------------------------------------------------------------------
// Affect Orbit Position
//----------------------------------------------------------------------
// pEffect�� AttachOrbitEffect���..
// point�� orbitPosition�� �����ش�.
//----------------------------------------------------------------------
#define AFFECT_ORBIT_EFFECT_POSITION( pEffect, point )					\
		if (pEffect->GetEffectType()==MEffect::EFFECT_ATTACH_ORBIT)		\
		{																\
			const POINT& pt = ((MAttachOrbitEffect*)pEffect)->GetOrbitPosition();	\
			point.x += pt.x;											\
			point.y += pt.y;											\
		}

//----------------------------------------------------------------------
// Is BackGround Orbit Position
//----------------------------------------------------------------------
// pEffect�� AttachOrbitEffect���..
// point�� y��ǥ�� ���� Back, Fore�� �����Ѵ�.
//----------------------------------------------------------------------
#define AFFECT_ORBIT_EFFECT_BACKGROUND( pEffect, bBackground )					\
		if (pEffect->GetEffectType()==MEffect::EFFECT_ATTACH_ORBIT)		\
		{																\
			const POINT& pt = ((MAttachOrbitEffect*)pEffect)->GetOrbitPosition();	\
			bBackground = (pt.y < 0); \
		}
