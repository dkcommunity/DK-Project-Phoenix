project(DirectXLib)

set(SOURCES
    "BIT_RES.CPP"
    "CDirectDraw.cpp"
    "CDirectDrawSurface.cpp"
    "CDirectInput.cpp"
    "CDirectMusic.cpp"
    "CDirectSetupGetVersion.cpp"
    "CDirectSound.cpp"
    "CDirectSoundStream.cpp"
    # D3D9Backend implementasyonu
    "D3D9Backend.cpp"
    # 2026-08-31 Legacy DX11 gecis P0 — D3D11Backend (flag ile secilir,
    # varsayilan D3D9; bkz. render_backend.cfg / PHOENIX_RENDER_BACKEND)
    "D3D11Backend.cpp"
    # 2026-09-01 FULL DX11 gecis Faz G0 — GPU quad pipeline + sprite texture cache
    "D3D11QuadPipeline.cpp"
    # 2026-09-01 FULL DX11 gecis Faz G1 — Blt* komut katmani (dx11gpu)
    "D3D11SpriteCommands.cpp"
    # Post-build-fix — DX7/DX9 SDK header coexist sorunu icin facade (2026-05-14)
    "D3D9Facade.cpp"
    # Phase 7 — pixel shader post-process (vignette) (2026-05-14)
    "D3D9PostProcess.cpp"
    # 2026-05-24 FAZ 3.1 C.4: CMP3.cpp DXLib build'den cikarildi (duplicate).
    # Client target (Designed_Chs/Project/CMakeLists.txt:24) ayni dosyayi
    # derliyor — burada da derlemek wasted work + DXLib'in CDirectSound.h'i
    # SoLoud forward decl + GetSoloud() icermiyor (Client/CDirectSound.h'de
    # SoLoud var). Kural 4.1: dosya disk'te kalir, sadece CMake entry yorum.
    # Pattern: vs_ui_gamecommon2.cpp duplicate audit (commit cd50fed).
    # "../CMP3.cpp"
    "header.cpp"
    "huffman.cpp"
    "mp3.cpp"
    "reader.cpp"
    "soundbuf.cpp"
    "subdecoder.cpp"
    "synfilt.cpp"
)

set(HEADERS
    "BIT_RES.H"
    "CDirectDraw.h"
    "CDirectDrawSurface.h"
    "CDirectInput.h"
    "CDirectMusic.h"
    "CDirectSetup.h"
    "CDirectSound.h"
    "CDirectSoundStream.h"
    # Phase 0 — D3D9 native port abstraction layer (2026-05-14)
    "RenderTypes.h"
    "IRenderSurface.h"
    "IRenderBackend.h"
    # D3D9Backend
    "D3D9Backend.h"
    # 2026-08-31 DX11 P0
    "D3D11Backend.h"
    # 2026-09-01 FULL DX11 gecis Faz G0
    "D3D11QuadPipeline.h"
    # 2026-09-01 FULL DX11 gecis Faz G1
    "D3D11SpriteCommands.h"
    # Post-build-fix — DX7/DX9 coexist facade (2026-05-14)
    "D3D9Facade.h"
    # Phase 7 — post-process shader (2026-05-14)
    "D3D9PostProcess.h"
    # 2026-05-24 FAZ 3.1 C.4: CMP3.h DXLib'den cikarildi (Client target kullaniyor)
    # "../CMP3.h"
    "common.h"
    "DXLib.h"
    "header.h"
    "huffman.h"
    "l3types.h"
    "mp3.h"
    "reader.h"
    "soundbuf.h"
)

message(STATUS "Configuring project ${PROJECT_NAME}")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _MBCS
    _LIB
    NO_GAMEGUARD
    NO_DPK
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "../VirtualFileSystem"
    "..//"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    # Legacy DirectX 7 SDK header'ları (ddraw.h, dsound.h, dinput.h, dmusic*.h)
    "${CMAKE_SOURCE_DIR}/thirdparty/dx7sdk/INCLUDE"
    # Phase 1 — DirectX 9 (d3d9.h). Windows SDK 10.x'te shared/d3d9.h ve
    # um/x86/d3d9.lib mevcut; eski thirdparty/directx9sdk/Include eski sal.h
    # icerdigi icin Windows SDK'nin modern sal.h'i ile catisip vcruntime.h
    # SAL macro hatalari uretiyordu. Windows SDK default include path
    # zaten d3d9.h sagliyor; d3dx9 deprecated, Phoenix kullanmiyor.
)

target_link_libraries(${PROJECT_NAME} PRIVATE XercesC::XercesC ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} PRIVATE d3d9.lib)
# 2026-08-31 DX11 P0 — D3D11Backend gereksinimleri (device+swapchain+runtime
# shader derleme). d3dcompiler_47.dll Win8+ inbox.
target_link_libraries(${PROJECT_NAME} PRIVATE d3d11.lib dxgi.lib d3dcompiler.lib)
if(TARGET LZO::LZO)
    target_link_libraries(${PROJECT_NAME} PRIVATE LZO::LZO)
endif()
# 2026-05-24 FAZ 3.1 C.4: CMP3 SoLoud migration — soloud include path (INTERFACE).
# CMP3.cpp soloud.h + soloud_wavstream.h ihtiyac duyar.
if(TARGET soloud)
    target_link_libraries(${PROJECT_NAME} PRIVATE soloud)
endif()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
if(MSVC)
    # Suppress warnings: C4566 (encoding), C4091 (ignored declaration), C4996 (deprecated/unsafe)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /MP /Zc:wchar_t /UTF-8 /wd4566 /wd4091 /wd4996 /FS)
endif()
