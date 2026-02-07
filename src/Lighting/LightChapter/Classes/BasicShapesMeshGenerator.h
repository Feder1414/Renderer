//
// Created by USUARIO on 2/7/2026.
//

#ifndef GRAFICOS_BASICSHAPESMESHGENERATOR_H
#define GRAFICOS_BASICSHAPESMESHGENERATOR_H
#include <memory>


class VertexLayout;
class Mesh;
class Material;

class BasicShapesMeshGenerator
{
public:
    static std::shared_ptr<Mesh> CreateCubeMesh(std::shared_ptr<Material> material,
                                                std::shared_ptr<VertexLayout> vertexLayout);
};


#endif //GRAFICOS_BASICSHAPESMESHGENERATOR_H
