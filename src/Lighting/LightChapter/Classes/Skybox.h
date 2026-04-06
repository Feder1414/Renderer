//
// Created by USUARIO on 3/18/2026.
//

#ifndef GRAFICOS_SKYBOX_H
#define GRAFICOS_SKYBOX_H
#include <memory>
#include <string>


class Mesh;
class Texture;

class Skybox
{
public:
    std::string skyboxName;
    Skybox(const std::shared_ptr<Texture>& cubeMapTexture, const std::shared_ptr<Mesh>& skyboxCube);

    [[nodiscard]] Mesh* GetMesh() const { return m_skyboxCube.get(); }
    [[nodiscard]] Texture* GetCubeMapTexture() const { return m_cubeMapTexture.get(); }
    void SetName(const std::string& name) { skyboxName = name; }

private:
    std::shared_ptr<Texture> m_cubeMapTexture;
    std::shared_ptr<Mesh> m_skyboxCube;
};


#endif //GRAFICOS_SKYBOX_H
