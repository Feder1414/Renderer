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
}

namespace
{
    std::string normalViewerGeoPath = "Shaders/Debug/normalViewerGeo.glsl";
    std::string fragShaderViewerPath = "Shaders/Debug/normalViewerFrag.glsl";
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
        normalViewerShader->SetShaderKey(
            ShaderManager::DefaultShaderToStr(DefaultShader::NormalViewer, normalLocation));
        ShaderManager::AddDefaultShader(normalViewerShader, DefaultShader::NormalViewer, normalLocation);
    }

    return normalViewerShader;
}
