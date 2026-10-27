project(CppUnitTestMain)

set(SOURCES
    "CppUnitTestMain.cpp"
    "Tests/ExampleTest.cpp"
    "Suites/CppUnitTestSuite.cpp"
)

set(HEADERS
    "Tests/ExampleTest.h"
    "Suites/CoreSuite.h"
    "Suites/CppUnitTestSuite.h"
)

message(STATUS "Configuring project ${PROJECT_NAME}")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _CONSOLE
    _MBCS
    NO_GAMEGUARD
    NO_DPK
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "../.."
    "Suites"
    "suites"
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