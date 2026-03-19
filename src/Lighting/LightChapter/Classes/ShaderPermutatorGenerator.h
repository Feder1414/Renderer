//
// Created by USUARIO on 3/10/2026.
//

#ifndef GRAFICOS_SHADERPERMUTATORGENERATOR_H
#define GRAFICOS_SHADERPERMUTATORGENERATOR_H
#include <memory>


class VertexLayout;
class Shader;

class ShaderPermutatorGenerator
{
public:
    static std::shared_ptr<Shader> CreateNormalViewerShader(VertexLayout* vertexLayout);
    static std::shared_ptr<Shader> CreateNormalViewerShader(unsigned int normalLocation);
};


#endif //GRAFICOS_SHADERPERMUTATORGENERATOR_H
