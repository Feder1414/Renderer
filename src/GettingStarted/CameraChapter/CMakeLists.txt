add_executable(CameraChapter main.cpp
        Classes/texture.cpp
        Classes/Shader.cpp
        Classes/Transform.cpp
        Classes/Camera.cpp
        Classes/Camera.h
        Classes/MouseHandler.cpp
        Classes/MouseHandler.h

)

target_include_directories(CameraChapter PRIVATE
        Classes)

target_link_libraries(CameraChapter PRIVATE
        glfw3
        glad
        stb_image
        glm::glm-header-only
        OpenGL::GL)


copyDirectories(CameraChapter textures Shaders)

#
#configure_file(Textures/fragmentShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/fragmentShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
