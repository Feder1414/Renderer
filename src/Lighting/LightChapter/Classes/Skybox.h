//
// Created by USUARIO on 3/18/2026.
//

#ifndef GRAFICOS_SKYBOX_H
#define GRAFICOS_SKYBOX_H
#include <memory>


class Mesh;
class Texture;

class Skybox
{
public:
    Skybox(const std::shared_ptr<Texture>& cubeMapTexture, const std::shared_ptr<Mesh>& skyboxCube);
    [[nodiscard]] Mesh* GetMesh() const { return m_skyboxCube.get(); }
    [[nodiscard]] Texture* GetCubeMapTexture() const { return m_cubeMapTexture.get(); }

private:
    std::shared_ptr<Texture> m_cubeMapTexture;
    std::shared_ptr<Mesh> m_skyboxCube;
};


#endif //GRAFICOS_SKYBOX_H
