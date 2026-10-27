cmake_minimum_required(VERSION 3.20)
project(DarkEdenLegacyModern)

# C++17 standardı — modern dil özelliklerini açık tutuyoruz
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ---------------------------------------------------------------------------
# Build output yönlendirmesi — Client.exe ve tüm runtime .dll'ler GameData/
# altına çıkacak; oyun çalıştırılabilir hâlde (CWD = GameData) başlatılır.
# Static/import lib'ler ise build ağacı içinde kalır (GameData'yı kirletmez).
# Multi-config generator (Visual Studio) için her konfig de aynı yere yazar.
# ---------------------------------------------------------------------------
set(PHOENIX_RUNTIME_DIR "${CMAKE_SOURCE_DIR}/GameData")
set(PHOENIX_INTERMEDIATE_LIB_DIR "${CMAKE_BINARY_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PHOENIX_RUNTIME_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PHOENIX_INTERMEDIATE_LIB_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PHOENIX_INTERMEDIATE_LIB_DIR}")

# Multi-config (VS) — Release/Debug alt klasörü oluşturmasın
foreach(_cfg IN ITEMS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_cfg} "${PHOENIX_RUNTIME_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_cfg} "${PHOENIX_INTERMEDIATE_LIB_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_cfg} "${PHOENIX_INTERMEDIATE_LIB_DIR}")
endforeach()

message(STATUS "Phoenix runtime output -> ${PHOENIX_RUNTIME_DIR}")

# Dependencies
find_package(XercesC CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LZO REQUIRED)
# Phoenix modernize: Jpeg.lib yerine libjpeg-turbo (vcpkg)
find_package(JPEG REQUIRED)
# DirectX 7/8 SDK (Legacy)
set(DX_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dx7sdk")
if(EXISTS "${DX_SDK_PATH}")
    include_directories("${DX_SDK_PATH}/INCLUDE")
    link_directories("${DX_SDK_PATH}/LIB")
    message(STATUS "Using Legacy DirectX SDK from local thirdparty folder")
else()
    message(WARNING "Legacy DirectX SDK not found in thirdparty folder. Build may fail.")
endif()

find_package(directx-headers CONFIG REQUIRED)

# ---------------------------------------------------------------------------
# FAZ 3.1 (2026-05-24): SoLoud audio engine — DirectSound/DirectMusic replacement.
#
# Why SoLoud (vs XAudio2 original plan):
#   - Higher-level API (Bus/crossfade/fade hazir, XAudio2 manual)
#   - MP3/OGG/WAV/FLAC built-in (XAudio2 sadece WAV native; MP3 icin
#     Media Foundation veya ek decoder gerek)
#   - Phoenix C++17 ile uyumlu C++ API (XAudio2 verbose COM)
#   - zlib license (Phoenix lisansi ile uyumlu)
#
# Why manual clone + add_subdirectory (vs FetchContent or vcpkg):
#   - SoLoud vcpkg registry'de yok
#   - FetchContent build-time'da external repo clone+execute = classifier
#     guvenlik endisesi
#   - Manual clone: thirdparty/soloud/ (RELEASE_20200207 tag, pinned)
#   - Reproducible build, internet bagimsiz
#
# Backend secimi: WASAPI (Vista+) modern audio. NULL fallback audio
# device yok senaryolar icin. WinMM kapali (eski legacy, daha kotu
# latency).
# ---------------------------------------------------------------------------
set(SOLOUD_BACKEND_WASAPI ON  CACHE BOOL "SoLoud: WASAPI backend (Windows Vista+, modern audio)" FORCE)
set(SOLOUD_BACKEND_WINMM  ON  CACHE BOOL "SoLoud: WinMM backend (fallback, legacy)" FORCE)
set(SOLOUD_BACKEND_NULL   ON  CACHE BOOL "SoLoud: NULL fallback (audio device yok)" FORCE)
# 2026-05-24: WASAPI re-enable denemesi. Onceki test'te (Faz B) error 7
# UNKNOWN_ERROR almistik — sebep belirsizdi, sadece WINMM ile devam ettik.
# Su an: WASAPI + WINMM + NULLDRIVER chain, init explicit WASAPI dene,
# fail ise WINMM, fail ise NULLDRIVER. WASAPI mmdeviceapi.h __uuidof()
# pattern kullaniyor — Mmdevapi.lib gerekmez, ole32 yeterli (Phoenix
# zaten linkli).
set(SOLOUD_BACKEND_SDL2   OFF CACHE BOOL "SoLoud: SDL2 backend (Phoenix SDL2 kullanmiyor)" FORCE)
set(SOLOUD_BUILD_DEMOS    OFF CACHE BOOL "SoLoud: demo binary'ler — build etme" FORCE)
set(SOLOUD_DYNAMIC        OFF CACHE BOOL "SoLoud: dynamic library (DLL)" FORCE)
set(SOLOUD_STATIC         ON  CACHE BOOL "SoLoud: static library (önerilen)" FORCE)
# SoLoud's contrib/CMakeLists.txt cmake_minimum_required(2.8) — modern CMake
# (3.x) deprecation warning verir, hata degil. Phoenix etkilenmez.
add_subdirectory(thirdparty/soloud/contrib EXCLUDE_FROM_ALL)

# SoLoud header'larini soloud target'in TUM consumer'larina INTERFACE
# olarak yay (#include "soloud.h", "soloud_wavstream.h" vb. icin).
# SoLoud kendi CMakeLists.txt'i eski include_directories() syntax kullaniyor
# (sadece kendi directory'sine etki eder). Phoenix'in BGMManager.cpp gibi
# consumer'lari include path'i goremiyor — manual interface ekleme.
target_include_directories(soloud INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/soloud/include>"
    "$<INSTALL_INTERFACE:include>")

# ---------------------------------------------------------------------------
# 2026-05-21 Effect/ses tam reset — VS6 stil for-loop scope desteği
# Referans (VS6 dönemi) cpp'ler `for(int i=...; ...; ...) {} ... for(i=0; ...)`
# pattern'i kullanıyor (eski C++03 for-loop scope, i loop sonrası visible).
# Modern MSVC default /Zc:forScope ile bu C2065 error veriyor. Legacy mode
# aktivasyonu (/Zc:forScope-) referans davranışını korur, davranış değişmez.
# Phoenix-only modern syntax dosyaları etkilenmez (her ikisi de derlenir).
# ---------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/Zc:forScope-)
endif()

# Define global macros
add_compile_definitions(
    NO_GAMEGUARD
    NO_DPK
    __DESIGNED_CHS  # Force CHS version globally to avoid conflicts
    # _STATIC_UI — DLL.h'nin DllClass/DllImport makrolarını `extern` olarak
    # set ediyor. Böylece C_VS_UI, mother, Timer vb. sınıfların metotları
    # static lib'den linkleniyor (Timer.dll, mother.dll gibi eski DLL'ler
    # artık kullanılmıyor; her şey static).
    # Beklenti: 360+ unresolved external VS_UI sembolü bu tek satırla düşer.
    _STATIC_UI
)

# ---------------------------------------------------------------------------
# D3D9 native render backend — tek aktif render yolu.
#
# 2026-06-12: DDraw7 + dgVoodoo rollback secimi build hattindan kaldirildi.
# Performans icin mevcut D3D9 16-bit composition/system-memory surface akisi
# korunur; sprite codec yolu yeniden yazilmaz.
# ---------------------------------------------------------------------------
add_compile_definitions(PHOENIX_RENDER_BACKEND_D3D9=1)
message(STATUS "Phoenix render backend: D3D9 NATIVE (single active backend)")

# Subdirectories
# NOT: TimerLib ve NMCryptLib diğer target'lardan ÖNCE eklenmesi lazım ki
# Client/vs_ui_lib onları link edebilsin.
add_subdirectory("Designed_Common/Lib/TimerLib")
add_subdirectory("Designed_Common/Lib/NMCryptLib")
add_subdirectory("Designed_Chs/Project")
add_subdirectory("Designed_Chs/Project/NEW - VS_UI/lib")
add_subdirectory("Designed_Common/Project/Client/CWinMain Library")
add_subdirectory("Designed_Common/Project/Client/D3DLib")
add_subdirectory("Designed_Common/Project/Client/DEUtil")
add_subdirectory("Designed_Common/Project/Client/DXLib")
add_subdirectory("Designed_Common/Project/Client/FileCheckLib")
add_subdirectory("Designed_Common/Project/Client/FrameLib")
add_subdirectory("Designed_Common/Project/Client/MZLib")
add_subdirectory("Designed_Common/Project/Client/MiniLZO")
add_subdirectory("Designed_Common/Project/Client/OggLib")
add_subdirectory("thirdparty/unrar")
add_subdirectory("Designed_Common/Project/Client/RarLib")
add_subdirectory("Designed_Common/Project/Client/SpriteLib")
add_subdirectory("Designed_Common/Project/Client/VirtualFileSystem")
add_subdirectory("Designed_Common/Project/Client/VolumeLib")
add_subdirectory("Designed_Common/Project/PhoenixUpdater")
add_subdirectory("Designed_Common/Project/PhoenixFilePatcher")
add_subdirectory("tools/PhoenixMiniDumper")

# 2026-05-18: ImGui v1.91.4 vendored to thirdparty/imgui (.git silinmis).
# Hibrit Admin Menu: VS_UI native menu dokunulmaz, ImGui ek DebugConsole
# (CTRL+SHIFT+F12, attrOperator gate). 3 tab: Item / Creature / Zone.
add_subdirectory("thirdparty/imgui")
