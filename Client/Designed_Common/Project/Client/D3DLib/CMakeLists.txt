project(D3DLib)

set(SOURCES
    "CD3DMath.cpp"
    "CD3DObject.cpp"
    "CD3DTextureEffect.cpp"
    "CDirect3D.cpp"
    "CTexturePack.cpp"
    "d3dutil.cpp"
)

set(HEADERS
    "CD3DMath.h"
    "CD3DObject.h"
    "CD3DTextureEffect.h"
    "CDirect3D.h"
    "CTexturePack.h"
    "d3dutil.h"
    "DX3D.h"
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
    "../dxlib"
    "../DXLib"  # Phase 4 — case-sensitive sistemlerde DXLib/D3D9Backend.h erisimi
    "../spritelib"
    "..//"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    # Phase 4 — DirectX 9 (d3d9.h). Windows SDK 10.x'te shared/d3d9.h ve
    # um/x86/d3d9.lib mevcut; eski thirdparty/directx9sdk/Include eski sal.h
    # icerdigi icin Windows SDK'nin modern sal.h'i ile catisip vcruntime.h
    # SAL macro hatalari uretiyordu (build kirildi). Windows SDK default
    # include path zaten d3d9.h sagliyor.
)

target_link_libraries(${PROJECT_NAME} PRIVATE XercesC::XercesC ZLIB::ZLIB)
if(TARGET LZO::LZO)
    target_link_libraries(${PROJECT_NAME} PRIVATE LZO::LZO)
endif()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
if(MSVC)
    # Suppress warnings: C4566 (encoding), C4091 (ignored declaration), C4996 (deprecated/unsafe)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /MP /Zc:wchar_t /UTF-8 /wd4566 /wd4091 /wd4996 /FS)
endif()