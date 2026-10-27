# ==========================================================================
# NMCryptLib — Project Phoenix stub replacement for NMCrypt.lib
# --------------------------------------------------------------------------
# Netmarble Kore login'e ait kapalı kaynak kütüphaneydi. Stub olarak minimal
# no-op implementasyon veriliyor; gerçek şifreleme yok. FAZ 0.3'te Netmarble
# tamamen devre dışı bırakılınca bu lib'e gerek kalmayacak.
# ==========================================================================

project(NMCryptLib)

set(SOURCES
    "NMCrypt_Stub.cpp"
)

message(STATUS "Configuring project ${PROJECT_NAME}")
add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Project/Client"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /MP /Zc:wchar_t /UTF-8 /wd4996 /FS)
endif()
