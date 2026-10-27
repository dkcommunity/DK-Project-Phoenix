project(PhoenixUpdater)

add_executable(PhoenixUpdater WIN32
    PhoenixUpdater.cpp
    PhoenixUpdater.rc
    ../Client/VirtualFileSystem/PhoenixProtectedFile.cpp
    ../Client/VirtualFileSystem/PhoenixProtectedFile.h
)

target_compile_features(PhoenixUpdater PRIVATE cxx_std_17)

target_include_directories(PhoenixUpdater PRIVATE
    ../Client
    ../Client/VirtualFileSystem
    ../Client/MZLib
)

target_link_libraries(PhoenixUpdater PRIVATE
    winhttp
    bcrypt
    shell32
    comctl32
    gdiplus
    ole32
    ZLIB::ZLIB
)

if(MSVC)
    target_compile_definitions(PhoenixUpdater PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
    target_compile_options(PhoenixUpdater PRIVATE /UTF-8 /wd4566 /wd4996 /FS)
endif()

set_target_properties(PhoenixUpdater PROPERTIES
    OUTPUT_NAME "Updater"
    RUNTIME_OUTPUT_DIRECTORY         "${PHOENIX_RUNTIME_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${PHOENIX_RUNTIME_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PHOENIX_RUNTIME_DIR}"
)

add_custom_command(TARGET PhoenixUpdater POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PHOENIX_RUNTIME_DIR}/data/info"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/updater_layout.inf"
            "${PHOENIX_RUNTIME_DIR}/data/info/updater_layout.inf"
)
