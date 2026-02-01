add_library(assimp STATIC IMPORTED GLOBAL)

set_target_properties(assimp PROPERTIES
        IMPORTED_CONFIGURATIONS "Debug;Release"


        IMPORTED_LOCATION_DEBUG
        "${CMAKE_CURRENT_LIST_DIR}/Debug/assimp-vc143-mtd.lib"

        IMPORTED_LOCATION_RELEASE
        "${CMAKE_CURRENT_LIST_DIR}/Release/assimp-vc143-mt.lib"

        INTERFACE_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/include"

)