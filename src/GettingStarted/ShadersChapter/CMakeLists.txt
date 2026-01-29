add_executable(Shaders main.cpp
        Classes/Shader.h
        Classes/Shader.cpp

)

target_include_directories(Shaders PRIVATE
        Classes)

target_link_libraries(Shaders PRIVATE
        glfw3
        glad
        stb_image
        OpenGL::GL)




configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Shaders/fragmentShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Shaders/fragmentShader.vert COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Shaders/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Shaders/vertexShader.vert COPYONLY)
