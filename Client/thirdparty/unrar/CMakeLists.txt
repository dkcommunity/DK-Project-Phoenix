# Phoenix — RARLAB UnRAR 7.1.9 (static, extract-only)
# Lisans: thirdparty/unrar/unrar/license.txt (UnRAR freeware, RAR olusturma yasak)

cmake_minimum_required(VERSION 3.20)

set(PHOENIX_UNRAR_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/unrar")

# Kaynak listesi UnRARDll.vcxproj ile eslesir (rar.cpp / konsol main haric).
set(PHOENIX_UNRAR_SOURCES
    "${PHOENIX_UNRAR_SRC_DIR}/archive.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/arcread.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/blake2s.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/cmddata.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/consio.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/crc.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/crypt.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/dll.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/encname.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/errhnd.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/extinfo.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/extract.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/filcreat.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/file.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/filefn.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/filestr.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/find.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/getbits.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/global.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/hash.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/headers.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/isnt.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/largepage.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/match.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/motw.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/options.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/pathfn.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/qopen.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/rarvm.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/rawread.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/rdwrfn.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/rijndael.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/rs.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/rs16.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/scantree.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/secpassword.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/sha1.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/sha256.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/smallfn.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/strfn.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/strlist.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/system.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/threadpool.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/timefn.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/ui.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/unicode.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/unpack.cpp"
    "${PHOENIX_UNRAR_SRC_DIR}/volume.cpp"
)

add_library(PhoenixUnrar STATIC ${PHOENIX_UNRAR_SOURCES})

target_include_directories(PhoenixUnrar PUBLIC
    "${PHOENIX_UNRAR_SRC_DIR}"
)

target_compile_definitions(PhoenixUnrar PRIVATE
    UNRAR
    SILENT
    RARDLL
)

target_compile_features(PhoenixUnrar PRIVATE cxx_std_17)

if(MSVC)
    set_property(TARGET PhoenixUnrar PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    target_compile_options(PhoenixUnrar PRIVATE
        /W3 /MP /utf-8
        /wd4007 /wd4996
    )
    target_precompile_headers(PhoenixUnrar PRIVATE "${PHOENIX_UNRAR_SRC_DIR}/rar.hpp")
    # global.cpp INCLUDEGLOBAL ile ErrHandler tanimlar; PCH bunu ezer.
    set_source_files_properties("${PHOENIX_UNRAR_SRC_DIR}/global.cpp"
        PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endif()

message(STATUS "PhoenixUnrar: RARLAB UnRAR 7.1.9 static (RAR5 extract)")
