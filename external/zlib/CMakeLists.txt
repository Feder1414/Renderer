add_library(zlib STATIC IMPORTED GLOBAL)
set_target_properties(zlib PROPERTIES
        IMPORTED_CONFIGURATIONS "Debug;Release"


        IMPORTED_LOCATION_DEBUG
        "${CMAKE_CURRENT_LIST_DIR}/Debug/zlibstaticd.lib"

        IMPORTED_LOCATION_RELEASE
        "${CMAKE_CURRENT_LIST_DIR}/Release/zlibstatic.lib"

        INTERFACE_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/include"

)