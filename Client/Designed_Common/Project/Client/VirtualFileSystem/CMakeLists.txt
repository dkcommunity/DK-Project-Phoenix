project(VirtualFileSystem)

set(SOURCES
    "CNFS.cpp"
    "nFS/nfs.cpp"
    "nFS/nfs_data.cpp"
    "nFS/nfs_dt.cpp"
    "nFS/nfs_fat.cpp"
    "nFS/nfs_iio.cpp"
    "nFS/nfs_nt.cpp"
    "nFS/nfs_pmatch.cpp"
    "CompressVirtualFileAccess.cpp"
    "NormalFileIO.cpp"
    "PhoenixProtectedFile.cpp"
    "PhoenixProtectedFileIO.cpp"
    "VirtualFileAccess.cpp"
    "VirtualFileIO.cpp"
    "VirtualTempFileIO.cpp"
    "vfstream.cpp"
    "VirtualFileSystem.cpp"
)

set(HEADERS
    "CNFS.h"
    "nFS/nfs.h"
    "nFS/nfs_data.h"
    "nFS/nfs_dt.h"
    "nFS/nfs_fat.h"
    "nFS/nfs_iio.h"
    "nFS/nfs_nt.h"
    "nFS/nfs_pmatch.h"
    "CompressVirtualFileAccess.h"
    "IFileIO.h"
    "NormalFileIO.h"
    "PhoenixProtectedFile.h"
    "PhoenixProtectedFileIO.h"
    "VFA_Base.h"
    "VirtualFileAccess.h"
    "VirtualFileIO.h"
    "VirtualTempFileIO.h"
    "VFS_Macro.h"
    "VFS_Precompile.h"
    "vfstream.h"
    "VirtualFileSystem.h"
)

message(STATUS "Configuring project ${PROJECT_NAME}")
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _MT
    _LIB
    _MBCS
    NO_GAMEGUARD
    NO_DPK
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "nFS"
    "../MiniLZO"
    "../MZLib"
    "."
    "../SpriteLib"
    ".."
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME} PUBLIC bcrypt)
target_link_libraries(${PROJECT_NAME} PRIVATE XercesC::XercesC)
if(TARGET LZO::LZO)
    target_link_libraries(${PROJECT_NAME} PRIVATE LZO::LZO)
endif()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
if(MSVC)
    # Suppress warnings: C4566 (encoding), C4091 (ignored declaration), C4996 (deprecated/unsafe)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /MP /Zc:wchar_t /UTF-8 /wd4566 /wd4091 /wd4996 /FS)
endif()
