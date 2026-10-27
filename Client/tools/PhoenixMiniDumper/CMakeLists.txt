cmake_minimum_required(VERSION 3.20)

add_executable(PhoenixMiniDumper main.cpp)
target_compile_features(PhoenixMiniDumper PRIVATE cxx_std_17)

if(MSVC)
  # Client ile ayni Win32 hedefi (x86)
  set_target_properties(PhoenixMiniDumper PROPERTIES
    OUTPUT_NAME "PhoenixMiniDumper"
    RUNTIME_OUTPUT_DIRECTORY "${PHOENIX_RUNTIME_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PHOENIX_RUNTIME_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PHOENIX_RUNTIME_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PHOENIX_RUNTIME_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${PHOENIX_RUNTIME_DIR}"
  )
endif()

target_link_libraries(PhoenixMiniDumper PRIVATE Dbghelp)
