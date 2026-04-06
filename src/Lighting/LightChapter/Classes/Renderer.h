//
// Created by USUARIO on 1/11/2026.
//

#ifndef GRAFICOS_RENDERER_H
#define GRAFICOS_RENDERER_H
#include <map>
#include <unordered_map>
#include <string>


#include "FrameBuffer.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "ModelRenderInfo.h"
#include "../Components/OutlineComponent.h"
#include "ViewPort.h"

#define SHADOW_MAP_SLOT 15


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


struct CamMatrices
{
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;
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

struct DrawItemInstancingShadow
{
    Mesh* mesh;
    std::vector<glm::mat4> transforms;
};


struct DrawKeyInstancingShadow
{
    Mesh* mesh;
    unsigned int submeshIndex;

    bool operator==(const DrawKeyInstancingShadow& other) const
    {
        return mesh == other.mesh && submeshIndex == other.submeshIndex;
    }
};

struct DrawKeyShadowHasher
{
    std::size_t operator()(const DrawKeyInstancingShadow& key) const
    {
        std::size_t h = 0;
        auto hashCombine = [&h](std::size_t v)
        {
            h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2);
        };

        hashCombine(std::hash<Mesh*>{}(key.mesh));
        hashCombine(std::hash<uint32_t>{}(key.submeshIndex));
        return h;
    }
};

#define MSAA_MODE_ENUM(X) \
X(SAMPLE2, 0), \
X(SAMPLE4, 1), \
X(SAMPLE8, 2),

#define X(name, val) name

enum class MSAA
{
    MSAA_MODE_ENUM(X)
    MAX
};
#undef X


#define X(name, value) name


#undef X


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

enum class PredefinedUBO
{
    CamMatrices,
    LightsGpu,
    LightMatrices
};


class Renderer
{
public:
    Renderer(int width, int height);
    void SetScene(Scene* scene);
    void DebugRenderLights(bool renderLights) { m_renderLights = renderLights; }
    void RenderScene();
    void PrintFrustumCullingResults();

    void SetResolution(int width, int height);

    void SetSceneViewPortResolution(unsigned int width, unsigned height);


    void SetRenderBB(bool renderBB) { m_renderBB = renderBB; }
    bool GetIsRenderingBB() const { return m_renderBB; }
    bool GetIsRenderingNormals() const { return m_renderNormals; }
    void SetRenderNormals(bool renderNormals) { m_renderNormals = renderNormals; }
    FaceCulling GetFaceCulling() const { return m_faceCulling; }
    static const std::vector<std::string>& GetFaceCullingNames();
    static const std::vector<std::string>& GetPostProcessEffectNames();
    static const std::vector<std::string>& GetMSAANames();
    void SetFaceCulling(FaceCulling faceCulling);
    void SetFaceCulling(int faceCulling);

    bool GetUsingMSAA() const { return m_usingMSAA; }
    void SetUsingMSAA(bool usingMsaa);
    void SetMSAAMode(MSAA msaaMode) { m_msaaMode = msaaMode; }
    MSAA GetCurrMsaaMode() const { return m_msaaMode; }

    void SetPostProcessEffect(PostProcessEffect postProcessEffect) { m_postProcessEffect = postProcessEffect; }
    void SkyboxPass();

    bool GetUsingGammaCorrection() const { return m_gammaCorrection; }

    unsigned int GetPostProccesEffectColTex() const;

    void SetUsingGammaCorrection(bool gammaCorrection) { m_gammaCorrection = gammaCorrection; }

    void ShadowPass();

    void InstancingShadowPass();

    ViewPort* GetShadowPassDebugViewport() const { return m_shadowDebugViewPort.get(); }


    PostProcessEffect GetPostProcessEffect() { return m_postProcessEffect; }

    void SetDebugShadowPass(bool debugShadowPass) { m_debugPassShadowMap = debugShadowPass; }
    bool GetDebugShadowPass() const { return m_debugPassShadowMap; }

    void SetRenderFrustums(bool renderFrustums) { m_renderFrustums = renderFrustums; }
    bool GetRenderFrustums() const { return m_renderFrustums; }

private:
    // Draw key is (Mesh, material, submesh, outline)
    // FOr instancing to work


    std::shared_ptr<Mesh> m_fullScreenQuad;
    std::unordered_map<DrawKey, DrawItemInstancing, DrawKeyHasher> m_drawKeyToInstanceItem = {};
    std::unordered_map<DrawKeyInstancingShadow, DrawItemInstancingShadow, DrawKeyShadowHasher>
    m_drawKeyToShadowInstanceItem = {};
    std::vector<Entity*> m_visibleLightEntities = {};
    std::vector<Entity*> m_shadowPassEntities = {};
    std::map<float, Entity*> m_semiTransparentEntities = {};
    std::vector<Entity*> m_opaqueCutOutEntities = {};
    std::vector<Entity*> m_borderPassEntities = {};
    std::vector<Entity*> m_frustumPass = {};


    std::vector<Entity*> m_worldBBVolumes{};

    std::vector<Entity*> m_normalPassEntities{};

    std::vector<Entity*> m_visibleEntities = {};
    std::unique_ptr<OutlineComponent> m_globalOutlineInstancing = nullptr;

    std::unique_ptr<FrameBuffer> m_forwardFrameBuffer;
    std::unique_ptr<FrameBuffer> m_shadowFrameBuffer;
    std::unique_ptr<FrameBuffer> m_postProcessFrameBuffer;
    std::unique_ptr<FrameBuffer> m_shadowFrameBufferDebug;


    bool m_usingMSAA = true;
    bool m_usingBlin = true;

    bool m_gammaCorrection = true;

    MSAA m_msaaMode = MSAA::SAMPLE2;

    std::array<std::unique_ptr<FrameBuffer>, static_cast<int>(MSAA::MAX)> m_msaaFB = {};

    std::unique_ptr<Buffer> m_UBOCamMatrix;
    std::unique_ptr<Buffer> m_SSBOLights;
    std::unique_ptr<Buffer> m_UBOLightMatrix;


    static unsigned int MAX_LIGHTS_RENDER;

    std::vector<LightGPU> m_lightsGpu;


    static int MAX_TEXTURE_SLOTS;
    int m_width, m_height = 0;
    int m_currTextureSlot = 0;
    bool m_renderLights = false;
    bool m_renderBB = true;

    bool m_renderNormals = true;
    bool m_renderFrustums = true;


    FaceCulling m_faceCulling = FaceCulling::Disable;
    PostProcessEffect m_postProcessEffect = PostProcessEffect::None;

    bool m_useFog = true;
    float m_fogDensity = 0.9f;
    glm::vec3 m_fogColor = glm::vec3(0.92f);

    unsigned int m_totalRenderableEntities = 0;

    double m_lastTimeDebugPrint = 0.0f;

    bool m_debugPassShadowMap = false;

    std::unique_ptr<ViewPort> m_sceneViewport = nullptr;
    std::unique_ptr<ViewPort> m_shadowViewPort = nullptr;
    std::unique_ptr<ViewPort> m_shadowDebugViewPort = nullptr;


    Scene* m_currScene;

    void BindTexture(Texture* texture);
    void BindFrameBuffer();
    FrameBuffer* ResolveFrameBuffer();
    void ResolveMSAAFrameBuffer();
    void UploadMaterialProperties(const Material* material, Shader* shader);

    void InitializeFrameBuffers();

    void UploadUniformProperties(const std::unordered_map<std::string, UniformValue>& uniformValues,
                                 Shader* shader);

    void NormalPass();
    void ChangeResolution(float m_width, float m_height);
    void UploadPerFrameProperties(Shader* shader);
    void UploadPerModelProperties(Shader* shader, const ModelRenderInfo* object);
    void UploadShadowProperties(Shader* shader);
    void FrustumCulling();
    void UploadLightProperties(Shader* shader);
    void OutlinePass(ModelRenderInfo* renderInfo, OutlineComponent* outlineComponent);
    void FrustumPass();
    void InstancingBorderPass(DrawItemInstancing drawItemInstancing);
    Frustum CalculateFrustumCamera();
    void ShadowPassDebug();
    void GenericPass(const ModelRenderInfo* object);
    void InstancingPass(const DrawItemInstancing& instancingDrawItem);


    void UpdateInstancingBufferInstances();
    void ClearRenderData();
    void BBPass();
    void SortRenderItems();


    void FullScreenQuadPass();
    void UploadDataPerFrameUBOSAndSSBO();
};


#endif //GRAFICOS_RENDERER_H
