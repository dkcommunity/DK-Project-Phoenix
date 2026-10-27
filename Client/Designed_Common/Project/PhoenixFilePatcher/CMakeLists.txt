project(PhoenixFilePatcher)

set(PHOENIX_FILE_PATCHER_SOURCES
    "main.cpp"
    "../Client/VirtualFileSystem/PhoenixProtectedFile.cpp"
    "../Client/VirtualFileSystem/PhoenixProtectedFile.h"
)

add_executable(PhoenixFilePatcher WIN32 ${PHOENIX_FILE_PATCHER_SOURCES})
add_executable(PhoenixFilePatcherCLI ${PHOENIX_FILE_PATCHER_SOURCES})

foreach(PATCHER_TARGET IN ITEMS PhoenixFilePatcher PhoenixFilePatcherCLI)
    target_include_directories(${PATCHER_TARGET} PRIVATE
        "../Client"
        "../Client/VirtualFileSystem"
        "../Client/MZLib"
    )

    target_link_libraries(${PATCHER_TARGET} PRIVATE ZLIB::ZLIB bcrypt user32 gdi32 comdlg32 shell32 ole32)
    target_compile_features(${PATCHER_TARGET} PRIVATE cxx_std_17)

    if(MSVC)
        target_compile_definitions(${PATCHER_TARGET} PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
        target_compile_options(${PATCHER_TARGET} PRIVATE /W3 /MP /Zc:wchar_t /UTF-8 /wd4566 /wd4996 /FS)
    endif()

    set_target_properties(${PATCHER_TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_SOURCE_DIR}/tools/PhoenixFilePatcher/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/tools/PhoenixFilePatcher/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/tools/PhoenixFilePatcher/bin"
    )
endforeach()

if(MSVC)
    target_link_options(PhoenixFilePatcher PRIVATE /ENTRY:mainCRTStartup)
endif()

set_target_properties(PhoenixFilePatcherCLI PROPERTIES
    OUTPUT_NAME "PhoenixFilePatcherCLI"
)
