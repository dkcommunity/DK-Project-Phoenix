project(ClientInfo)

set(SOURCES
    "../../../Designed_Common/Project/ClientInfo/ActionInfoTable.cpp"
    "../../../Designed_Common/Project/ClientInfo/ActionInfoTableFireCracker.cpp"
    "../../../Designed_Common/Project/ClientInfo/ActionInfoTableOusters.cpp"
    "../../../Designed_Common/Project/ClientInfo/ActionInfoTableVampire.cpp"
    "../../../Designed_Common/Project/ClientInfo/AppendPatch.cpp"
    "../../../Designed_Common/Project/ClientInfo/CreatureSpriteTable.cpp"
    "../../../Designed_Common/Project/ClientInfo/EffectStatusTable.CPP"
    "../../../Designed_Common/Project/ClientInfo/InitCreatureTable.cpp"
    "../../../Designed_Common/Project/ClientInfo/InitEffectLight.cpp"
    "../../../Designed_Common/Project/ClientInfo/InitHelpManager.cpp"
    "../../../Designed_Common/Project/ClientInfo/InitHelpString.cpp"
    "../../../Designed_Common/Project/ClientInfo/InitInfo.cpp"
    "../../../Designed_Common/Project/ClientInfo/InitPortal.cpp"
    "../../../Designed_Common/Project/ClientInfo/InitSound.CPP"
    "../../../Designed_Common/Project/ClientInfo/InitZone.cpp"
    "../../../Designed_Common/Project/Client/MGameStringTable.cpp"
    "../../../Designed_Common/Project/Client/MNPCTable.cpp"
    "../../../Designed_Common/Project/Client/MSoundTable.cpp"
    "../../../Designed_Common/Project/Client/AppendPatchInfo.cpp"
    "../../../Designed_Common/Project/ClientInfo/CAnimationFrame.cpp"
    "../../../Designed_Common/Project/ClientInfo/CFileIndexTable.cpp"
    "../../../Designed_Common/Project/Client/FrameLib/CFrame.cpp"
    "../../../Designed_Common/Project/Client/FrameLib/CFramePack.cpp"
    "../../../Designed_Common/Project/ClientInfo/ChildView.cpp"
    "../../../Designed_Common/Project/Client/ClientConfig.cpp"
    "../../../Designed_Common/Project/ClientInfo/ClientInfo.cpp"
    "../../../Designed_Common/Project/Client/CMessageArray.cpp"
    "../../../Designed_Common/Project/Client/ExpInfo.cpp"
    "../../../Designed_Common/Project/Client/MActionInfoTable.cpp"
    "../../../Designed_Common/Project/ClientInfo/MainFrm.cpp"
    "../../../Designed_Common/Project/client/MathTable.cpp"
    "../../../Designed_Common/Project/Client/MChatManager.cpp"
    "../../../Designed_Common/Project/Client/MCreatureSpriteTable.cpp"
    "../../../Designed_Common/Project/Client/MCreatureTable.cpp"
    "../../../Designed_Common/Project/Client/MEffectSpriteTypeTable.cpp"
    "../../../Designed_Common/Project/Client/MEffectStatusTable.cpp"
    "../../../Designed_Common/Project/client/MGridItemManager.cpp"
    "../../../Designed_Common/Project/Client/MHelpManager.cpp"
    "../../../Designed_Common/Project/Client/MHelpStringTable.cpp"
    "../../../Designed_Common/Project/Client/MInteractionObjectTable.cpp"
    "../../../Designed_Common/Project/client/MItem.cpp"
    "../../../Designed_Common/Project/client/MItemManager.cpp"
    "../../../Designed_Common/Project/Client/MItemOptionTable.cpp"
    "../../../Designed_Common/Project/Client/MItemTable.cpp"
    "../../../Designed_Common/Project/Client/MLevelNameTable.cpp"
    "../../../Designed_Common/Project/ClientInfo/MMinMaxInfo.cpp"
    "../../../Designed_Common/Project/Client/MMusicTable.cpp"
    "../../../Designed_Common/Project/Client/MObject.cpp"
    "../../../Designed_Common/Project/ClientInfo/MServerCreatureTable.cpp"
    "../../../Designed_Common/Project/ClientInfo/MServerItemTable.cpp"
    "../../../Designed_Common/Project/Client/MShopTemplate.cpp"
    "../../../Designed_Common/Project/Client/MSkillInfoTable.cpp"
    "../../../Designed_Common/Project/Client/MSkillManager.cpp"
    "../../../Designed_Common/Project/client/MSlotItemManager.cpp"
    "../../../Designed_Common/Project/Client/MString.cpp"
    "../../../Designed_Common/Project/Client/MStringArray.cpp"
    "../../../Designed_Common/Project/Client/MStringMap.cpp"
    "../../../Designed_Common/Project/client/MTimeItemManager.CPP"
    "../../../Designed_Common/Project/Client/MZoneTable.cpp"
    "../../../Designed_Common/Project/client/packet/Properties.cpp"
    "../../../Designed_Common/Project/client/RankBonusTable.cpp"
    "../../../Designed_Common/Project/ClientInfo/ServerRankBonusTable.cpp"
    "../../../Designed_Common/Project/ClientInfo/StdAfx.cpp"
    "../../../Designed_Common/Project/client/packet/StringStream.cpp"
    "../../../Designed_Common/Project/client/UserInformation.cpp"
    "../../../Designed_Common/Project/ClientInfo/EventGiftInfo.cpp"
    "../../../Designed_Common/Project/Client/MGpsTable.cpp"
)

set(HEADERS
    "../../../Designed_Common/Project/client/packet/Properties.h"
    "../../../Designed_Common/Project/client/packet/StringStream.h"
    "../../../Designed_Common/Project/Client/AddonDef.h"
    "../../../Designed_Common/Project/Client/AppendPatchInfo.h"
    "../../../Designed_Common/Project/ClientInfo/CFileIndexTable.h"
    "../../../Designed_Common/Project/Client/FrameLib/CFrame.h"
    "../../../Designed_Common/Project/Client/FrameLib/CFramePack.h"
    "../../../Designed_Common/Project/ClientInfo/ChildView.h"
    "../../../Designed_Common/Project/Client/ClientConfig.h"
    "../../../Designed_Common/Project/ClientInfo/ClientInfo.h"
    "../../../Designed_Common/Project/Client/CMessageArray.h"
    "../../../Designed_Common/Project/Client/CTypeTable.h"
    "../../../Designed_Common/Project/Client/EffectSpriteTypeDef.h"
    "../../../Designed_Common/Project/ClientInfo/InitInfo.h"
    "../../../Designed_Common/Project/Client/ItemClassDef.h"
    "../../../Designed_Common/Project/Client/MActionInfoTable.h"
    "../../../Designed_Common/Project/ClientInfo/MainFrm.h"
    "../../../Designed_Common/Project/Client/MChatManager.h"
    "../../../Designed_Common/Project/Client/MCreatureSpriteTable.h"
    "../../../Designed_Common/Project/Client/MCreatureTable.h"
    "../../../Designed_Common/Project/Client/MEffectSpriteTypeTable.h"
    "../../../Designed_Common/Project/Client/MEffectStatusDef.h"
    "../../../Designed_Common/Project/Client/MEffectStatusTable.h"
    "../../../Designed_Common/Project/Client/MFileDef.h"
    "../../../Designed_Common/Project/Client/MGameStringTable.h"
    "../../../Designed_Common/Project/Client/MGpsTable.h"
    "../../../Designed_Common/Project/Client/MGridItemManager.h"
    "../../../Designed_Common/Project/Client/MHelpDef.h"
    "../../../Designed_Common/Project/Client/MHelpManager.h"
    "../../../Designed_Common/Project/Client/MHelpStringTable.h"
    "../../../Designed_Common/Project/Client/MInteractionObjectTable.h"
    "../../../Designed_Common/Project/Client/MItem.h"
    "../../../Designed_Common/Project/Client/MItemManager.h"
    "../../../Designed_Common/Project/Client/MItemOptionTable.h"
    "../../../Designed_Common/Project/Client/MItemTable.h"
    "../../../Designed_Common/Project/Client/MLevelNameTable.h"
    "../../../Designed_Common/Project/ClientInfo/MMinMaxInfo.h"
    "../../../Designed_Common/Project/Client/MMusicTable.h"
    "../../../Designed_Common/Project/Client/MNPCTable.h"
    "../../../Designed_Common/Project/Client/MObject.h"
    "../../../Designed_Common/Project/ClientInfo/MServerCreatureTable.h"
    "../../../Designed_Common/Project/ClientInfo/MServerItemTable.h"
    "../../../Designed_Common/Project/Client/MShopTemplateTable.h"
    "../../../Designed_Common/Project/Client/MSkillManager.h"
    "../../../Designed_Common/Project/Client/MSlotItemManager.h"
    "../../../Designed_Common/Project/Client/MSoundTable.h"
    "../../../Designed_Common/Project/Client/MString.h"
    "../../../Designed_Common/Project/Client/MStringArray.h"
    "../../../Designed_Common/Project/Client/MStringMap.h"
    "../../../Designed_Common/Project/Client/MTypeDef.h"
    "../../../Designed_Common/Project/Client/MusicDef.h"
    "../../../Designed_Common/Project/Client/MZoneTable.h"
    "../../../Designed_Common/Project/Client/NomalEffectSpriteTypeDef.h"
    "../../../Designed_Common/Project/Client/Packet/Types/PacketItemDef.h"
    "../../../Designed_Common/Project/client/RankBonusDef.h"
    "../../../Designed_Common/Project/client/RankBonusTable.h"
    "../../../Designed_Common/Project/ClientInfo/Resource.h"
    "../../../Designed_Common/Project/Client/ScreenEffectSpriteTypeDef.h"
    "../../../Designed_Common/Project/ClientInfo/ServerRankBonusTable.h"
    "../../../Designed_Common/Project/Client/ShadowSpriteTypeDef.h"
    "../../../Designed_Common/Project/Client/SkillDef.h"
    "../../../Designed_Common/Project/Client/SoundDef.h"
    "../../../Designed_Common/Project/ClientInfo/StdAfx.h"
    "../../../Designed_Common/Project/Client/BitAttribute.h"
)

message(STATUS "Configuring project ${PROJECT_NAME}")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    __WINDOWS__
    _AFXDLL
    _MBCS
    __INIT_INFO__
    NO_GAMEGUARD
    NO_DPK
    # FAZ 4: Client.exe ile struct layout uyumu icin tum makro seti senkron.
    __DESIGNED_CHS
    __WORK__
    __WIN32__
    __GAME_CLIENT__
    __MOVE_ZONE_UI__
    OUTPUT_DEBUG
    # FAZ 4 (Jules audit, 2026-04-29): STL implementation switch — Client
    # ile aynı olmasi lazim.
    _STLP_USE_STATIC_LIB
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "/designed/project/vs_ui/src/hangul"
    "/designed/project/vs_ui/src/widget"
    "SpriteLib"
    "Updater"
    "../../../../Darkeden/Designed_Common/project/client"
    "../../../../Darkeden/Designed_Common/project/client/lib"
    "../../../../Darkeden/Designed_Common/project/client/MZLib"
    "../../../../Darkeden/Designed_Common/project/clientinfo"
    "../../../../Darkeden/Designed_Common/lib/ui"
    "../../../../Darkeden/Designed_Common/project/client/DXLib"
    "MZLib"
    "VolumeLib"
    "../../../../Darkeden/Designed_Common/project/NEW - vs_ui/src/hangul"
    "/designed/project/client"
    "../../../../Darkeden/Designed_Common/project/client/Packet"
    "../../../../Darkeden/Designed_Common/project/client/Updater"
    "WinLib"
    "../../../../Darkeden/Designed_Common/project/NEW - vs_ui/src/Imm"
    "../../../../Darkeden/Designed_Common/project/client/D3DLib"
    "../../../../Darkeden/Designed_Common/project/client/VolumeLib"
    "../../../../Darkeden/Designed_Common/project/NEW - vs_ui/src/header"
    "DXLib"
    "FrameLib"
    "/designed/project/vs_ui/src/header"
    "../../../../Darkeden/Designed_Common/project/client/WinLib"
    "/designed/project/lib"
    "/designed/lib/ui"
    "lib"
    "Packet"
    "../../../../Darkeden/Designed_Common/project/NEW - vs_ui/src/widget"
    "D3DLib"
    "../../../../Darkeden/Designed_Common/project/Client/PackingLib"
    "/designed/project/vs_ui/src/Imm"
    "../../../../Darkeden/Designed_Common/project/client/SpriteLib"
    "../../../../Darkeden/Designed_Common/project/client/FrameLib"
    "../../../../Darkeden/Designed_Common/project/Client/VirtualFileSystem"
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