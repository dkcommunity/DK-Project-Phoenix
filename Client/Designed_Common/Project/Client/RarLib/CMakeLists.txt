project(RarLib)

set(SOURCES
    "RarFile.cpp"
    "UnRarBridge.cpp"
)

set(HEADERS
    "RarFile.h"
    "UnRarBridge.h"
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
    ".."
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/thirdparty/unrar/unrar"
)

target_link_libraries(${PROJECT_NAME} PRIVATE PhoenixUnrar XercesC::XercesC ZLIB::ZLIB)
if(TARGET LZO::LZO)
    target_link_libraries(${PROJECT_NAME} PRIVATE LZO::LZO)
endif()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
if(MSVC)
    # Suppress warnings: C4566 (encoding), C4091 (ignored declaration), C4996 (deprecated/unsafe)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /MP /Zc:wchar_t /UTF-8 /wd4566 /wd4091 /wd4996 /FS)
endif()

# RPK/RAR5 smoke test (GameData CWD'den calistirilir)
add_executable(RpkSmokeTest "${CMAKE_SOURCE_DIR}/tools/rpk_smoke_test.cpp")
target_link_libraries(RpkSmokeTest PRIVATE ${PROJECT_NAME} PhoenixUnrar)
target_include_directories(RpkSmokeTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/thirdparty/unrar/unrar"
)
target_compile_features(RpkSmokeTest PRIVATE cxx_std_17)
set_target_properties(RpkSmokeTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PHOENIX_RUNTIME_DIR}"
)