//----------------------------------------------------------------------
// MViewDef.h
//----------------------------------------------------------------------
//
// View에 관련된 정의.
//
//----------------------------------------------------------------------

#ifndef	__MVIEWDEF_H__
#define	__MVIEWDEF_H__


//----------------------------------------------------------------------
// 화면 크기에 대한 정보
//----------------------------------------------------------------------
#define	SURFACE_WIDTH				(g_pUserInformation->iResolution_x)
#define	SURFACE_HEIGHT				(g_pUserInformation->iResolution_y)

//----------------------------------------------------------------------
// 기본 바닥 Tile의 크기
//----------------------------------------------------------------------
#define	TILE_X						48	//64
#define	TILE_Y						24	//32
#define	TILE_X_HALF					24	//32
#define	TILE_Y_HALF					12	//16

//----------------------------------------------------------------------
// 8방향에 따른 기준이 되는 기울기 : 가로/세로 비율과 관련
//----------------------------------------------------------------------
#define	BASIS_DIRECTION_LOW			0.35
#define	BASIS_DIRECTION_HIGH		3.0


//----------------------------------------------------------------------
// ScreenLight 크기 - 3D시야처리에서 사용하는 값
//----------------------------------------------------------------------
// 800/12.5f = 64 딱떨어짐
// 1024/12.5f = 81.92 안떨어짐 이 경우 화면 오른쪽이 하얗게 나오는 유져가 있음
// 반올림으로 처리

#define	SCREENLIGHT_WIDTH			((int)(SURFACE_WIDTH/12.5f + 0.5f))
#define	SCREENLIGHT_HEIGHT			((int)(SCREENLIGHT_WIDTH * ((SURFACE_HEIGHT/(float)SURFACE_WIDTH) * (4/3.0f)) + 0.5f))

//----------------------------------------------------------------------
// 한 화면에 출력되는 Sector의 개수?
//----------------------------------------------------------------------
#define	SECTOR_SKIP_LEFT			-1
#define	SECTOR_SKIP_UP				-1

#define	SECTOR_WIDTH				(int)(SURFACE_WIDTH/TILE_X)//21
#define	SECTOR_HEIGHT				(int)(SURFACE_HEIGHT/TILE_Y)//32

//----------------------------------------------------------------------
// Player의 시야 최대 범위 == 화면 상의 Tile범위
//
// 가로 : 9 + 1 + 9
// 세로 : 10 + 1 + 10
//
// 1은 캐릭터의 좌표 중심이다.
//----------------------------------------------------------------------

#define	SECTOR_WIDTH_HALF			(int)(SECTOR_WIDTH/2)//11
#define	SECTOR_HEIGHT_HALF			(int)(SECTOR_HEIGHT/2)//17
// 캐릭터를 화면 중심에 세우기 위한 좌표
#define	SECTOR_SKIP_PLAYER_LEFT		(int)(-SECTOR_WIDTH_HALF)//10
#define	SECTOR_SKIP_PLAYER_UP		(int)(-SECTOR_HEIGHT_HALF)//16

//----------------------------------------------------------------------
// 실제 게임 화면
//----------------------------------------------------------------------
#define	CLIPSURFACE_WIDTH			SURFACE_WIDTH
#define	CLIPSURFACE_HEIGHT			SURFACE_HEIGHT

//----------------------------------------------------------------------
//
// Tile Surface에 대한 정보
//
//----------------------------------------------------------------------
#define	TILESURFACE_SECTOR_EDGE				3		// 여유 공간 Sector개수

#define	TILESURFACE_SECTOR_WIDTH			(TILESURFACE_SECTOR_EDGE+SECTOR_WIDTH+TILESURFACE_SECTOR_EDGE)//27		// 3+21+3가로 여백
#define	TILESURFACE_SECTOR_HEIGHT			(TILESURFACE_SECTOR_EDGE+SECTOR_HEIGHT+TILESURFACE_SECTOR_EDGE)//38		// 3+32+3세로 여백
#define	TILESURFACE_SECTOR_OUTLINE_LEFT		(TILESURFACE_SECTOR_EDGE)//3											// 왼쪽 경계
#define	TILESURFACE_SECTOR_OUTLINE_RIGHT	(TILESURFACE_SECTOR_EDGE+SECTOR_WIDTH)//24								// 3+21오른쪽 경계
#define	TILESURFACE_SECTOR_OUTLINE_UP		(TILESURFACE_SECTOR_EDGE)//3											// 위쪽 경계
#define	TILESURFACE_SECTOR_OUTLINE_DOWN		(TILESURFACE_SECTOR_EDGE+SECTOR_HEIGHT)//35								// 3+32아래쪽 경계

#define	TILESURFACE_WIDTH					(TILE_X*TILESURFACE_SECTOR_WIDTH)//1296									// TileSurface의 가로 크기
#define	TILESURFACE_HEIGHT					(TILE_Y*TILESURFACE_SECTOR_HEIGHT)//912									// TileSurface의 세로 크기
#define	TILESURFACE_OUTLINE_LEFT			(TILE_X*TILESURFACE_SECTOR_OUTLINE_LEFT)//144							// 왼쪽 경계
#define	TILESURFACE_OUTLINE_RIGHT			(TILE_X*TILESURFACE_SECTOR_OUTLINE_RIGHT)//1152							// 오른쪽 경계
#define	TILESURFACE_OUTLINE_UP				(TILE_Y*TILESURFACE_SECTOR_OUTLINE_UP)//72								// 위쪽 경계
#define	TILESURFACE_OUTLINE_DOWN			(TILE_Y*TILESURFACE_SECTOR_OUTLINE_DOWN)//840							// 아래쪽 경계


//----------------------------------------------------------------------
// Tile Surface를 다시 그릴때 Flag
//----------------------------------------------------------------------
#define	FLAG_TILESURFACE_LACK_LEFT			0x01	// 왼쪽 부족
#define	FLAG_TILESURFACE_LACK_RIGHT			0x02	// 오른쪽 부족
#define	FLAG_TILESURFACE_LACK_UP			0x04	// 위쪽 부족
#define	FLAG_TILESURFACE_LACK_DOWN			0x08	// 아래쪽 부족

#endif
