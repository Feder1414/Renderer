add_executable(LightChapter main.cpp
        Classes/texture.cpp
        Classes/Shader.cpp
        Classes/Transform.cpp
        Components/Camera.cpp
        Components/Camera.h
        Classes/MouseHandler.cpp
        Classes/MouseHandler.h
        Classes/Mesh.cpp
        Classes/Mesh.h
        Components/ModelRenderInfo.cpp
        Components/ModelRenderInfo.h
        Classes/VertexLayout.cpp
        Classes/VertexLayout.h
        Components/Light.cpp
        Components/Light.h
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
        Components/IComponent.h
        Classes/Window.cpp
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
        Classes/Event.cpp
        Classes/Event.h
        Classes/Window.h
        Classes/BasicShapesMeshGenerator.cpp
        Classes/BasicShapesMeshGenerator.h
        Classes/ShaderManager.cpp
        Classes/ShaderManager.h
        Classes/GUI/Editor.cpp
        Classes/GUI/Editor.h
        Classes/GUI/IWidget.cpp
        Classes/GUI/IWidget.h
        Classes/GUI/EntityTree.cpp
        Classes/GUI/EntityTree.h
        Classes/GUI/ComponentInspector.cpp
        Classes/GUI/ComponentInspector.h
        Classes/GUI/TransformComponent.cpp
        Classes/GUI/TransformComponent.h
        Classes/GlmConverter.cpp
        Classes/GlmConverter.h
        Components/OutlineComponent.cpp
        Components/OutlineComponent.h
        Components/Metadata/ComponentProperty.cpp
        Components/Metadata/ComponentProperty.h
        Classes/GUI/GenericComponent.cpp
        Classes/GUI/GenericComponent.h
        Components/Metadata/ComponentPropertiesMetadata.cpp
        Components/Metadata/ComponentPropertiesMetadata.h
        Components/Metadata/GlobalEnumMetadata.cpp
        Components/Metadata/GlobalEnumMetadata.h
        Classes/BV/IBoundingVolume.cpp
        Classes/BV/IBoundingVolume.h
        Classes/BV/Frustum.cpp
        Classes/BV/Frustum.h
        Classes/BV/AABB.cpp
        Classes/BV/AABB.h
        Classes/BV/SphereBv.cpp
        Classes/BV/SphereBv.h
        Classes/GUI/GeneralRendererSettingsComponent.cpp
        Classes/GUI/GeneralRendererSettingsComponent.h
        Classes/ShaderPermutatorGenerator.cpp
        Classes/ShaderPermutatorGenerator.h
        Classes/FrameBuffer.cpp
        Classes/FrameBuffer.h
        Classes/RenderBuffer.cpp
        Classes/RenderBuffer.h
        Classes/Skybox.cpp
        Classes/Skybox.h

)

target_include_directories(LightChapter PRIVATE
        Classes
        Classes/BV
        Components
        OpenGL)

target_link_libraries(LightChapter PRIVATE
        glfw3
        glad
        stb_image
        glm::glm-header-only
        OpenGL::GL
        uuid
        assimp
        imgui

)


target_link_libraries(LightChapter PUBLIC
        TracyClient
)



target_compile_definitions(LightChapter PRIVATE GLM_ENABLE_EXPERIMENTAL)

# TRACY_ENABLE

copyDirectories(LightChapter textures Shaders assets)

#
#configure_file(Textures/fragmentShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/fragmentShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
#configure_file(Textures/vertexShader.vert ${CMAKE_CURRENT_BINARY_DIR}/Textures/vertexShader.vert COPYONLY)
