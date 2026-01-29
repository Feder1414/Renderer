add_executable(Textures main.cpp
        Classes/texture.cpp
        Classes/Shader.cpp

)

target_include_directories(Textures PRIVATE
        Classes)

target_link_libraries(Textures PRIVATE
        glfw3
        glad
        stb_image
        glm::glm-header-only
        OpenGL::GL)


add_custom_target(copy_data
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
        "$<TARGET_FILE_DIR:Textures>/Shaders"
        COMMENT "Copying data"
)

add_custom_target(copy_textures
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/textures"
        "$<TARGET_FILE_DIR:Textures>/textures"
        COMMENT "Copying data"
)

add_dependencies(Textures copy_data copy_textures)

#
#configure_file(Textures/fragmentShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/fragmentShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
