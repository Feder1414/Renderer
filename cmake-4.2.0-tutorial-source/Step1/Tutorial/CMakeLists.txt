# TODO10: Move all the Tutorial target commands to this CMakeLists.txt. Ensure
#          that all paths are updated to be relative to this new location.
add_executable(Tutorial)

# TODO4: Add the Tutorial/Tutorial.cxx source file to the Tutorial target
target_sources(Tutorial PRIVATE Tutorial.cxx)

target_link_libraries(Tutorial
        PRIVATE MathFunctions )
