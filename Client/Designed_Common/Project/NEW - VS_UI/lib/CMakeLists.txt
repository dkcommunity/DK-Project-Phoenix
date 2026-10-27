project(vs_ui_lib)

set(SOURCES
    "AcceleratorDef.cpp"
    "../src/AcceleratorManager.cpp"
    "../../Client/AtlTypes/atltypes.cpp"
    "../src/hangul/Ci.cpp"
    "../src/Imm/CImm.cpp"
    "Client_PCH.cpp"
    "../src/hangul/FL2.cpp"
    "../src/KeyAccelerator.cpp"
    "../src/widget/mother.cpp"
    "../src/OperatorCategory.cpp"
    "../src/widget/pi_core.cpp"
    "../src/SkinManager.cpp"
    "../../client/SXml/SXml.cpp"
    "../src/widget/u_button.cpp"
    "../src/widget/U_edit.cpp"
    "../src/widget/u_scrollbar.cpp"
    "../src/widget/u_window.cpp"
    "../src/Vs_ui.cpp"
    "../src/VS_UI_Base.cpp"
    "../src/VS_UI_BBS.cpp"
    "../src/VS_UI_DESC.cpp"
    "../src/VS_UI_Description.cpp"
    "../src/VS_UI_Descriptor.cpp"
    "../src/VS_UI_Dialog.cpp"
    "../src/VS_UI_ELEVATOR.cpp"
    "../src/VS_UI_Exception.cpp"
    "../src/VS_UI_Exchange.cpp"
    "../src/VS_UI_ExtraDialog.cpp"
    "../src/VS_UI_Game.cpp"
    "../src/VS_UI_GameCommon.cpp"
    "../src/vs_ui_gamecommon2.cpp"
    "../src/VS_UI_GameOusters.cpp"
    "../src/VS_UI_GameSlayer.cpp"
    "../src/VS_UI_GameVampire.cpp"
    "../src/VS_UI_GlobalResource.cpp"
    "../src/VS_UI_Helper.cpp"
    "../src/VS_UI_Item.cpp"
    "../src/VS_UI_Message.cpp"
    "../src/VS_UI_mouse_pointer.cpp"
    "../src/VS_UI_Operator.cpp"
    "../src/VS_UI_PetStorage.cpp"
    "../src/VS_UI_progress.cpp"
    "../src/VS_UI_Shop.cpp"
    "../src/VS_UI_skill_tree.cpp"
    "../src/VS_UI_Storage.cpp"
    "../src/VS_UI_TaxRate.cpp"
    "../src/VS_UI_TIMECOUNT.cpp"
    "../src/VS_UI_Title.cpp"
    "../src/VS_UI_TITLE_SHOWCHAR.CPP"
    "../src/VS_UI_Tutorial.cpp"
    "../src/VS_UI_ui_result_receiver.cpp"
    "../src/VS_UI_util.cpp"
    "../src/VS_UI_WebBrowser.cpp"
    "../src/VS_UI_widget.cpp"
    "../src/VS_UI_WorldMap.cpp"
    "../../Client/TickTimer.cpp"
)

set(HEADERS
    "../src/header/AcceleratorDef.h"
    "../src/header/AcceleratorManager.h"
    "../src/hangul/Ci.h"
    "../../client/Client_PCH.h"
    "../src/hangul/Fl2.h"
    "../src/header/KeyAccelerator.h"
    "../src/widget/mother.h"
    "../src/widget/PI.H"
    "../src/widget/pi_core.h"
    "../src/widget/SimpleDataList.h"
    "../src/header/SkinManager.h"
    "../src/widget/u_button.h"
    "../src/widget/U_edit.h"
    "../src/widget/u_scrollbar.h"
    "../src/widget/u_window.h"
    "../src/widget/unicorn.h"
    "../src/header/VS_UI.h"
    "../src/header/VS_UI_Base.h"
    "../src/header/VS_UI_bbs.h"
    "../src/header/VS_UI_DESC.h"
    "../src/header/VS_UI_Description.h"
    "../src/header/VS_UI_descriptor.h"
    "../src/header/VS_UI_Dialog.h"
    "../src/header/VS_UI_ELEVATOR.h"
    "../src/header/VS_UI_Exchange.h"
    "../src/header/VS_UI_ExtraDialog.h"
    "../src/header/VS_UI_filepath.h"
    "../src/header/VS_UI_Game.h"
    "../src/header/VS_UI_GameCommon.h"
    "../src/header/VS_UI_GameCommon2.h"
    "../src/header/VS_UI_GameOusters.h"
    "../src/header/VS_UI_GameSlayer.h"
    "../src/header/VS_UI_GameVampire.h"
    "../src/header/VS_UI_GlobalResource.h"
    "../src/header/VS_UI_Helper.h"
    "../src/header/VS_UI_item.h"
    "../src/header/VS_UI_Message.h"
    "../src/header/VS_UI_mouse_pointer.h"
    "../src/header/VS_UI_Operator.h"
    "../src/header/VS_UI_PetStorage.h"
    "../src/header/VS_UI_progress.h"
    "../src/header/VS_UI_Shop.h"
    "../src/header/VS_UI_skill_tree.h"
    "../src/header/VS_UI_Storage.h"
    "../src/header/VS_UI_TaxRate.h"
    "../src/header/VS_UI_TIMECOUNT.h"
    "../src/header/VS_UI_title.h"
    "../src/header/VS_UI_Tutorial.h"
    "../src/header/VS_UI_ui_result_receiver.h"
    "../src/header/VS_UI_UIMessage.h"
    "../src/header/VS_UI_util.h"
    "../src/header/VS_UI_WebBrowser.h"
    "../src/header/VS_UI_widget.h"
    "../../Client/TickTimer.h"
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
    "../../client"
    "../../client/frameLib"
    "../../../lib/ui"
    "../src/widget"
    "../../lib"
    "../../client/sxml"
    "../../client/spriteLib"
    "../../client/virtualfilesystem"
    "../src/imm"
    "../src/hangul"
    "../../client/dxlib"
    "../src/header"
    "../../../lib"
    "../../client/RarLib"
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