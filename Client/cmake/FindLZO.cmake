# FindLZO.cmake - Find LZO library
# Result variables:
#  LZO_FOUND - True if LZO found
#  LZO_INCLUDE_DIRS - Location of lzo headers
#  LZO_LIBRARIES - Location of lzo libraries

find_path(LZO_INCLUDE_DIR NAMES lzo/lzo1x.h)
find_library(LZO_LIBRARY NAMES lzo2 lzo2d)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LZO DEFAULT_MSG LZO_LIBRARY LZO_INCLUDE_DIR)

if(LZO_FOUND)
    set(LZO_INCLUDE_DIRS ${LZO_INCLUDE_DIR})
    set(LZO_LIBRARIES ${LZO_LIBRARY})
    
    if(NOT TARGET LZO::LZO)
        add_library(LZO::LZO UNKNOWN IMPORTED)
        set_target_properties(LZO::LZO PROPERTIES
            IMPORTED_LOCATION "${LZO_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LZO_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(LZO_INCLUDE_DIR LZO_LIBRARY)
