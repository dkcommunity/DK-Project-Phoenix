# ==========================================================================
# TimerLib — Project Phoenix modern C++17 timer static library
# --------------------------------------------------------------------------
# Eski Timer.dll / Timer.lib yerine doğrudan kaynaktan derlenen modern
# implementasyon. Header (Timer2.h) Designed_Common/Lib/ui/ altında kalıyor;
# biz sadece .cpp'yi sunuyoruz.
# ==========================================================================

project(TimerLib)

set(SOURCES
    "Timer2_Modern.cpp"
)

message(STATUS "Configuring project ${PROJECT_NAME}")
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Timer2.h'nin bulunduğu dizinin include path'ini veriyoruz (../ui/)
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../ui"
    # Client header'ları (BasicS.h timer2.h içinden include ediliyor)
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Project/Client"
)

# _STATIC_UI — DLL.h'nin DllImport makrosunu `extern` olarak ayarlamak için.
# Böylece Timer_import.h'daki `DllImport C_TIMER2 gC_timer2;` standart extern
# bildirimi olur ve TimerLib.lib içinde zaten sağlanan tanımla linklenir.
target_compile_definitions(${PROJECT_NAME} PUBLIC _STATIC_UI)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /MP /Zc:wchar_t /UTF-8 /wd4996 /FS)
endif()
