//
// Created by USUARIO on 3/10/2026.
//

#include "ShaderPermutatorGenerator.h"

#include <string>

#include "VertexLayout.h"
#include <format>

#include "Shader.h"
#include "ShaderManager.h"

namespace
{
    constexpr std::string_view vsNormalViewerTemplate = R"(
    #version 460
    layout (location = 0) in vec3 position;
    layout (location = {}) in vec3 normal;

    out VS_OUT{{
        vec3 normal;
    }} vs_out;

    void main() {{
        gl_Position = vec4(position, 1.0f);

        vs_out.normal = normal;
    }}
)";


    constexpr std::string_view vsInstancingShadowPassTemplate = R"(
    #version 460

    layout (location = 0) in vec3 position;


    layout(location = {}) in mat4 instance;


    layout(std140, binding = 0) uniform CameraData{{
        mat4 viewTransform;
        mat4 projectionTransform;
    }};

    layout(std140, binding = 2) uniform LightData{{
        mat4 lightView;
        mat4 lightProj;
    }};

    void main() {{
        gl_Position = lightProj* lightView* instance * vec4(position, 1.0f);
    }}

)";
}

namespace
{
    std::string normalViewerGeoPath = "Shaders/Debug/normalViewerGeo.glsl";
    std::string fragShaderViewerPath = "Shaders/Debug/normalViewerFrag.glsl";

    std::string shadowPassFragPath = "Shaders/shadowPassFrag.glsl";
}

std::shared_ptr<Shader> ShaderPermutatorGenerator::CreateNormalViewerShader(VertexLayout* vertexLayout)
{
    auto normalAttribLocation = vertexLayout->GetVertexAttribLocation(
        VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::NORMAL));

    return CreateNormalViewerShader(normalAttribLocation);
}

std::shared_ptr<Shader> ShaderPermutatorGenerator::CreateNormalViewerShader(unsigned int normalLocation)
{
    std::shared_ptr<Shader> normalViewerShader = ShaderManager::GetDefaultShader(
        DefaultShader::NormalViewer, normalLocation);
    if (!normalViewerShader)
    {
        auto vsSourceCode = std::vformat(vsNormalViewerTemplate, std::make_format_args(normalLocation));

        ShaderSource vsSource{.source = vsSourceCode, .isSourceCode = true};
        ShaderSource fsSource{.source = fragShaderViewerPath};
        ShaderSource geoSource{.source = normalViewerGeoPath};

        normalViewerShader = std::make_shared<Shader>(vsSource, fsSource, geoSource);
        ShaderManager::AddDefaultShader(normalViewerShader, DefaultShader::NormalViewer, normalLocation);
    }

    return normalViewerShader;
}

std::shared_ptr<Shader> ShaderPermutatorGenerator::CreateShadowInstancingShader(unsigned int transformLocation)
{
    std::shared_ptr<Shader> shadowInstancing = ShaderManager::GetDefaultShader(
        DefaultShader::ShadowPassInstancing, transformLocation);
    if (!shadowInstancing)
    {
        auto vsSourceCode = std::vformat(vsInstancingShadowPassTemplate, std::make_format_args(transformLocation));

        ShaderSource vsSource{.source = vsSourceCode, .isSourceCode = true};
        ShaderSource fsSource{.source = shadowPassFragPath};

        shadowInstancing = std::make_shared<Shader>(vsSource, fsSource);

        ShaderManager::AddDefaultShader(shadowInstancing, DefaultShader::ShadowPassInstancing, transformLocation);
    }

    return shadowInstancing;
}
