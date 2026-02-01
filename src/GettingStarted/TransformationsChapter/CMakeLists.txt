add_executable(TransformationsChapter main.cpp
        Classes/texture.cpp
        Classes/Shader.cpp
        Classes/Transform.cpp

)

target_include_directories(TransformationsChapter PRIVATE
        Classes)

target_link_libraries(TransformationsChapter PRIVATE
        glfw3
        glad
        stb_image
        glm::glm-header-only
        OpenGL::GL)



copyDirectories(TransformationsChapter textures Shaders)

#
#configure_file(Textures/fragmentShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/fragmentShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
