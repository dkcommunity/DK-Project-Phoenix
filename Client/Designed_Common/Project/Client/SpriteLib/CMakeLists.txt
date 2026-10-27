project(SpriteLib)

set(SOURCES
    "CAlphaSprite.cpp"
    "CAlphaSprite555.cpp"
    "CAlphaSprite565.cpp"
    "CAlphaSpritePal.cpp"
    "CFileIndexTable.cpp"
    "CFilter.cpp"
    "CIndexSprite.cpp"
    "CIndexSprite555.cpp"
    "CIndexSprite565.cpp"
    "CIndexSpritePack.cpp"
    "CShadowSprite.cpp"
    "CSprite.cpp"
    "CSprite555.cpp"
    "CSprite565.cpp"
    "CSpriteDef.cpp"
    "CSpriteOutlineManager.cpp"
    "CSpritePack.cpp"
    "CSpritePackList.cpp"
    "CSpritePackList555.cpp"
    "CSpritePackList565.cpp"
    "CSpritePal.cpp"
    "CSpritePalBase.cpp"
    "CSpriteSet.cpp"
    "CSpriteSetManager.cpp"
    "CSpriteSurface.cpp"
    "CStorageSurface.cpp"
    "MPalette.cpp"
    "MPalettePack.cpp"
)

set(HEADERS
    "CAlphaSpritePal.h"
    "CFileIndexTable.h"
    "CFilter.h"
    "CIndexSprite.h"
    "CIndexSprite555.h"
    "CIndexSprite565.h"
    "CIndexSpritePack.h"
    "CSetManager.h"
    "CShadowSprite.h"
    "CSprite.h"
    "CSprite555.h"
    "CSprite565.h"
    "CSpriteDef.h"
    "CSpriteOutlineManager.h"
    "CSpritePack.h"
    "CSpritePackList.h"
    "CSpritePackList555.h"
    "CSpritePackList565.h"
    "CSpritePal.h"
    "CSpritePalBase.h"
    "CSpriteSet.h"
    "CSpriteSetManager.h"
    "CSpriteSurface.h"
    "CStorageSurface.h"
    "CTypePack.h"
    "CTypePackVector.h"
    "DrawTypeDef.h"
    "MPalette.h"
    "MPalettePack.h"
    "SP.h"
    "TArray.h"
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
    "../dxlib"
    "../VirtualFileSystem"
    ".."
    "${CMAKE_CURRENT_SOURCE_DIR}"
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