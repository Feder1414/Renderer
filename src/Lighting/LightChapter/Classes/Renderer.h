//
// Created by USUARIO on 1/11/2026.
//

#ifndef GRAFICOS_RENDERER_H
#define GRAFICOS_RENDERER_H
#include <map>
#include <unordered_map>
#include <string>


#include "FrameBuffer.h"
#include "Material.h"
#include "Mesh.h"
#include "ModelRenderInfo.h"
#include "../Components/OutlineComponent.h"


# define ENUM_FACE_CULLING(X) \
    X(FrontFace,0), \
    X(BackFace,0), \
    X(FrontAndBack,0), \
    X(Disable,0),


#define X(name, value) name

enum class FaceCulling
{
    ENUM_FACE_CULLING(X)
};
#undef X

class Scene;
class Material;
class Shader;
class ModelRenderInfo;
class Entity;


struct DrawItemInstancing
{
    Mesh* mesh;
    Material* material;
    ModelRenderInfo* renderInfo;
    std::vector<glm::mat4> transforms;
    std::vector<glm::mat3> normalsMatrix;
    glm::mat3 normalMatrix;
    unsigned int submeshIndex;
    bool usingOutline;
};


struct DrawKey
{
    Mesh* mesh;
    Material* material;
    unsigned int submeshIndex;
    bool usingOutline;

    bool operator==(const DrawKey& other) const
    {
        return mesh == other.mesh && material == other.material && submeshIndex == other.submeshIndex && usingOutline ==
            other.usingOutline;
    }
};

struct DrawKeyHasher
{
    std::size_t operator()(const DrawKey& key) const
    {
        std::size_t h = 0;
        auto hashCombine = [&h](std::size_t v)
        {
            h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2);
        };

        hashCombine(std::hash<Mesh*>{}(key.mesh));
        hashCombine(std::hash<Material*>{}(key.material));
        hashCombine(std::hash<uint32_t>{}(key.submeshIndex));
        hashCombine(std::hash<bool>{}(key.usingOutline));
        return h;
    }
};

# define ENUM_FACE_POST_PROCESS_EFFECT(X) \
X(None,0), \
X(Blur,1), \
X(Edge,2),


#define X(name, value) name

enum class PostProcessEffect
{
    ENUM_FACE_POST_PROCESS_EFFECT(X)
};
#undef X
class Renderer
{
public:
    Renderer(int width, int height);
    void SetScene(Scene* scene) { m_currScene = scene; }
    void DebugRenderLights(bool renderLights) { m_renderLights = renderLights; }
    void RenderScene();
    void PrintFrustumCullingResults();

    void SetResolution(int width, int height);

    void SetRenderBB(bool renderBB) { m_renderBB = renderBB; }
    bool GetIsRenderingBB() const { return m_renderBB; }
    bool GetIsRenderingNormals() const { return m_renderNormals; }
    void SetRenderNormals(bool renderNormals) { m_renderNormals = renderNormals; }
    FaceCulling GetFaceCulling() const { return m_faceCulling; }
    void SetFaceCulling(FaceCulling faceCulling);
    void SetFaceCulling(int faceCulling);

    void SetPostProcessEffect(PostProcessEffect postProcessEffect) { m_postProcessEffect = postProcessEffect; }
    void SkyboxPass();

    PostProcessEffect GetPostProcessEffect() { return m_postProcessEffect; }

private:
    // Draw key is (Mesh, material, submesh, outline)
    // FOr instancing to work


    struct EntityBoundingVolume
    {
        Entity* entity;
        std::unique_ptr<IBoundingVolume> boundingVolume;
    };

    std::shared_ptr<Mesh> m_fullScreenQuad;
    std::unordered_map<DrawKey, DrawItemInstancing, DrawKeyHasher> m_drawKeyToInstanceItem = {};
    std::map<float, Entity*> m_semiTransparentEntities = {};
    std::vector<Entity*> m_opaqueCutOutEntities = {};
    std::vector<Entity*> m_borderPassEntities = {};


    std::vector<EntityBoundingVolume> m_worldBBVolumes{};

    std::vector<Entity*> m_normalPassEntities{};

    std::vector<Entity*> m_visibleEntities = {};
    std::unique_ptr<OutlineComponent> m_globalOutlineInstancing = nullptr;

    std::unique_ptr<FrameBuffer> m_forwardFrameBuffer;


    static int MAX_TEXTURE_SLOTS;
    int m_width, m_height = 0;
    int m_currTextureSlot = 0;
    bool m_renderLights = false;
    bool m_renderBB = true;

    bool m_renderNormals = true;


    FaceCulling m_faceCulling = FaceCulling::Disable;
    PostProcessEffect m_postProcessEffect = PostProcessEffect::None;

    bool m_useFog = true;
    float m_fogDensity = 0.9f;
    glm::vec3 m_fogColor = glm::vec3(0.92f);

    unsigned int m_totalRenderableEntities = 0;

    double m_lastTimeDebugPrint = 0.0f;


    Scene* m_currScene;

    void BindTexture(Texture* texture);
    void UploadMaterialProperties(const Material* material, Shader* shader);

    void UploadUniformProperties(const std::unordered_map<std::string, UniformValue>& uniformValues,
                                 Shader* shader);

    void NormalPass();
    void ChangeResolution(float m_width, float m_height);
    void UploadPerFrameProperties(Shader* shader);
    void UploadPerModelProperties(Shader* shader, const ModelRenderInfo* object);
    void FrustumCulling();
    void UploadLightProperties(Shader* shader);
    void OutlinePass(ModelRenderInfo* renderInfo, OutlineComponent* outlineComponent);
    void InstancingBorderPass(DrawItemInstancing drawItemInstancing);
    Frustum CalculateFrustumCamera();
    void GenericPass(const ModelRenderInfo* object);
    void InstancingPass(const DrawItemInstancing& instancingDrawItem);


    void UpdateInstancingBufferInstances();
    void ClearRenderData();
    void BBPass();
    void SortRenderItems();


    void FullScreenQuadPass();
};


#endif //GRAFICOS_RENDERER_H
