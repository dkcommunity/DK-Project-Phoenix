project(vs_ui_lib)

set(SOURCES
    "../../../../Designed_Common/Project/NEW - VS_UI/lib/AcceleratorDef.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/AcceleratorManager.cpp"
    "../../../../Designed_Common/Project/Client/AtlTypes/atltypes.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/hangul/Ci.cpp"
    # TouchSense/Immersion SDK artik kullanilmiyor. Orijinal CImm.cpp
    # kaynakta kalir; modern build no-op wrapper'i derler.
    "../../../../Designed_Common/Project/NEW - VS_UI/src/Imm/CImmDisabled.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/lib/Client_PCH.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/hangul/FL2.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/KeyAccelerator.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/mother.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/OperatorCategory.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/pi_core.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/SkinManager.cpp"
    "../../../../Designed_Common/Project/client/SXml/SXml.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/u_button.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/U_edit.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/u_scrollbar.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/u_window.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/Vs_ui.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Base.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_BBS.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Control.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_DESC.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Description.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Descriptor.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Dialog.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_ELEVATOR.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Exception.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Exchange.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_ExtraDialog.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Friend_System.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Game.cpp"
    # src/VS_UI_GameCommon.cpp — ana dosya (1.3 MB). Tek aktif kaynak.
    # 2026-05-20: GEMINI'nin Part1-10.cpp + Sanitized.cpp + Internal.h
    # boş stub'ları silindi (commit e127cc4).
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_GameCommon.cpp"
    # lib/vs_ui_gamecommon2.cpp — DUPLICATE, build'den ÇIKARILDI (2026-05-20).
    # Empirik audit: src/VS_UI_GameCommon2.cpp'nin tam subset'i. 533/533 method
    # ortak, lib/'de 0 unique method. Build aylarca 555 LNK4006 warning'i ile
    # çalışıyordu — linker src/'i seçip lib/'in symbol'lerini discard ediyordu
    # (build_clean.log:2511-3066). Referans VS6 VS_UI.dsp:193 da sadece src/'i
    # build'e alıyordu; lib/ disk artifact'iydi. Dosya disk'te kalıyor (kural
    # 4.1 silme yasak); sadece compile listesinden çıkarıldı.
    # Detay: memory/project_vs_ui_gamecommon2_duplicate_audit.md
    # "../../../../Designed_Common/Project/NEW - VS_UI/lib/vs_ui_gamecommon2.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_GameOusters.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_GameSlayer.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_GameVampire.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_GlobalResource.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_GPS.CPP"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Helper.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_INDOUN_NPC_DIALOG.CPP"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Item.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Message.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_mouse_pointer.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_NewTitle.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_NewTitleInterfaceConfig.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Operator.cpp"
    # 2026-09-02: Reborn Option menu portu (plan: reborn_option_menü_portu)
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_OptionRenewal.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_PetMixingForge.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_PetStorage.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_PremiumGiveItem.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_progress.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Shop.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_skill_tree.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_SKILL_VIEW.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Storage.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_TaxRate.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_TIMECOUNT.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Title.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_TITLE_SHOWCHAR.CPP"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_Tutorial.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_ui_result_receiver.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_UserGrade.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_util.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_WebBrowser.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_widget.cpp"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_WorldMap.cpp"
    # src/VS_UI_GameCommon2.cpp — 649 KB, bu dosya önceden boştu ama VS_UI .cpp
    # dosyalarını toplu referanstan restore ederken dolu hali geri geldi.
    # İçinde countContributeGrade, g_StartShowWebPageMessage, RequestResurrectSetButton
    # ve C_VS_UI_REQUEST_RESURRECT'ın çoğu methodu var.
    "../../../../Designed_Common/Project/NEW - VS_UI/src/VS_UI_GameCommon2.cpp"
    "../../../../Designed_Common/Project/Client/TickTimer.cpp"
)

set(HEADERS
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/AcceleratorDef.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/AcceleratorManager.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/hangul/Ci.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/lib/Client_PCH.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/hangul/Fl2.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/KeyAccelerator.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/mother.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/PI.H"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/pi_core.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/SimpleDataList.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/SkinManager.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/u_button.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/U_edit.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/u_scrollbar.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/u_window.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget/unicorn.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Base.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_bbs.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Control.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_DESC.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Description.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_descriptor.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Dialog.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_ELEVATOR.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Exchange.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_ExtraDialog.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_filepath.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Friend_System.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Game.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_GameCommon.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_GameCommon2.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_GameOusters.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_GameSlayer.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_GameVampire.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_GlobalResource.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_GPS.H"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Helper.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_INDOUN_NPC_DIALOG.H"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_item.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Message.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_mouse_pointer.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Newtitle.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_NewTitleInterfaceConfig.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Operator.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_OptionRenewal.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_PetMixingForge.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_PetStorage.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_PremiumGiveItem.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_progress.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Shop.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_skill_tree.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_SKILL_VIEW.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Storage.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_TaxRate.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_TIMECOUNT.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_title.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_Tutorial.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_ui_result_receiver.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_UIMessage.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_UserGrade.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_util.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_WebBrowser.h"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header/VS_UI_widget.h"
    "../../../../Designed_Common/Project/Client/TickTimer.h"
)

message(STATUS "Configuring project ${PROJECT_NAME}")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _LIB
    _MBCS
    NO_GAMEGUARD
    NO_DPK
    # FAZ 4: Client.exe ile ABI/struct layout uyumu icin tum makroları
    # senkronla. ContentsFilter.h kosullu field'lar (m_wZoneID, ek base
    # class member'lari) bu makrolarla yonetiliyor.
    __DESIGNED_CHS
    __WORK__
    __WINDOWS__
    __WIN32__
    __GAME_CLIENT__
    __MOVE_ZONE_UI__
    OUTPUT_DEBUG
    # FAZ 4 (Jules audit, 2026-04-29): Client'ta tanimli ama vs_ui_lib'de
    # eksik olan STL implementation switch. STLport static lib kullaniyorsa
    # std::string/std::vector sizeof'u Client'ta farkli oluyordu.
    _STLP_USE_STATIC_LIB
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "../../../../Designed_Common/Project/NEW - VS_UI/src/hangul"
    "../../../../Designed_Common/Project/client/sxml"
    "../../../../Designed_Common/Project/client/virtualfilesystem"
    "../../../../Designed_Common/lib"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/imm"
    "../../../../Designed_Common/Project/client/dxlib"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/widget"
    "../../../../Designed_Common/Project/NEW - VS_UI/src/header"
    "../../../../Designed_Common/Project/client/RarLib"
    "../../../../Designed_Common/Project/client/frameLib"
    "../../../../Designed_Common/lib/ui"
    "../../../../Designed_Common/Project/client"
    # Packet klasörü — DarkAssert.h, Exception.h vb. bu yol altında bulunuyor
    "../../../../Designed_Common/Project/client/Packet"
    "../../../../Designed_Common/Project/lib"
    "../../../../Designed_Common/project/client/D3DLib"
    "../../../../Designed_Common/Project/client/spriteLib"
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
    # /Zc:forScope- : C++98 legacy 'for' scope — `for(int i;...)` içinde
    # tanımlanan değişken döngü dışına sızsın. Modern C++17 standardı scope'u
    # döngüye sınırlı tutar; bu eski VS_UI kodu ise sızmaya güveniyor
    # (ör. iki ayrı for bloğunda aynı 'i' değişkeni, ikincisi yeniden int'siz).
    # /permissive : eski MSVC ad-hoc davranışları (uzun tip isimleri, vs.)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /MP /Zc:wchar_t /UTF-8 /wd4566 /wd4091 /wd4996 /FS /Zc:forScope- /permissive)
    # NOT: Bir denemede /FI "client_PCH.h" ile forced-include yapılmıştı ama
    # MSBuild incremental cache davranışı ile sonuç deterministik gelmiyordu.
    # Yerine VS_UI_Base.h'ye doğrudan <string>/<vector> ve using std::string/
    # vector eklenerek bu bağımlılık header-level'de garanti altına alındı.
    # C++17 'register' storage class'ını kaldırdı. Eski VS_UI kodu
    # (vs_ui_gamecommon2.cpp vb.) hâlâ 'register int i;' kullanıyor. MSVC
    # bunu error olarak reject ettiğinde 'i' tanımsız kalıyor. register
    # keyword'ünü empty define'a çeviriyoruz — anlamsal bir etki yok (zaten
    # modern derleyici ignore ediyor) ama parse hatası ortadan kalkıyor.
    # _ALLOW_KEYWORD_MACROS: MSVC Standard Library xkeycheck.h, keyword'leri
    # macro olarak yeniden tanımlamayı yasaklar. Bu flag muafiyet sağlar.
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        register=
        _ALLOW_KEYWORD_MACROS=1
    )
endif()
