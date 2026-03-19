//
// Created by USUARIO on 3/18/2026.
//

#include "Skybox.h"

#include <iostream>

#include "Texture.h"

Skybox::Skybox(const std::shared_ptr<Texture>& cubeMapTexture, const std::shared_ptr<Mesh>& skyboxCube)
{
    if (!cubeMapTexture)
    {
        std::cerr << "The cubemap texture attached is empty" << std::endl;
        return;
    }
    if (!skyboxCube)
    {
        std::cerr << "The skybox texture attached is empty" << std::endl;
        return;
    }

    if (cubeMapTexture->GetTextureType() != TextureType::CubeMap)
    {
        std::cerr << "The cubemap texture must be of type cubemap" << std::endl;
        return;
    }

    m_cubeMapTexture = cubeMapTexture;
    m_skyboxCube = skyboxCube;
}


