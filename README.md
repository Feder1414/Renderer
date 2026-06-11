
# Rendering Engine

Real-time graphics renderer/engine developed in C++ using OpenGL. It partially implements the ECS (Entity Component System) architectural pattern, prioritizing composition over inheritance to provide a generic and extensible solution.

# Features

- Basic 3D model loading
- Entity system partially based on ECS (Entity Component System)
- Spatial transformations
- Free camera system
- Basic material and texture system
- Real-time lighting using the Blinn–Phong model
- Basic shader management
- Frustum culling
- Shadows and Cascaded Shadow Maps (CSM)


<img width="1913" height="1018" alt="image" src="https://github.com/user-attachments/assets/c5f10088-1eaf-421a-bf82-f0450f81d683" />

## Used Dependencies

- OpenGL
- GLFW
- GLAD
- GLM
- Assimp
- stb_image

## Future Work
- Add skeletical animations
- Support more than just one light with shadows.
- Physically Based Rendering (PBR)
- Develop an RHI (Render Hardware Interface) and add Vulkan

## Build

```bash
git clone https://github.com/Feder1414/Renderer.git
cd RenderingEngine

cmake -B build
cmake --build build
```

The main development target is `src/Lighting/LightChapter`. Other targets are kept for reference and contain older experiments or previous engine iterations.
