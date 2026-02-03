add_executable(LightChapter main.cpp
        Classes/texture.cpp
        Classes/Shader.cpp
        Classes/Transform.cpp
        Classes/Camera.cpp
        Classes/Camera.h
        Classes/MouseHandler.cpp
        Classes/MouseHandler.h
        Classes/Mesh.cpp
        Classes/Mesh.h
        Classes/ModelRenderInfo.cpp
        Classes/ModelRenderInfo.h
        Classes/VertexLayout.cpp
        Classes/VertexLayout.h
        Classes/Light.cpp
        Classes/Light.h
        Classes/ShaderResource.cpp
        Classes/ShaderResource.h
        Classes/Material.cpp
        Classes/Material.h
        Classes/Renderer.cpp
        Classes/Renderer.h
        Classes/Scene.cpp
        Classes/Scene.h
        OpenGL/OpenGL.h
        Classes/Entity.cpp
        Classes/Entity.h
        Classes/UuidGenerator.cpp
        Classes/UuidGenerator.h
        Classes/IComponent.h
        Classes/IWindow.cpp
        Classes/IWindow.h
        Classes/RandomCoordinateGenerator.cpp
        Classes/RandomCoordinateGenerator.h
        Classes/Engine.cpp
        Classes/Engine.h
        Classes/AssimpLoader.cpp
        Classes/AssimpLoader.h
        Classes/AssetManager.cpp
        Classes/AssetManager.h
        Classes/Sampler.cpp
        Classes/Sampler.h

)

target_include_directories(LightChapter PRIVATE
        Classes
        OpenGL)

target_link_libraries(LightChapter PRIVATE
        glfw3
        glad
        stb_image
        glm::glm-header-only
        OpenGL::GL
        uuid
        assimp
)

target_compile_definitions(LightChapter PRIVATE GLM_ENABLE_EXPERIMENTAL)

copyDirectories(LightChapter textures Shaders)

#
#configure_file(Textures/fragmentShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/fragmentShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
