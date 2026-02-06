//
// Created by USUARIO on 12/18/2025.
//

#include "Texture.h"

#include <iostream>

#include "stb_image.h"
#include <glad/glad.h>

class Shader;

void Texture::LoadImageFromFile(std::string filePath, bool flipY)
{
    stbi_set_flip_vertically_on_load(flipY);
    unsigned char* data = stbi_load(filePath.c_str(), &m_widht, &m_height, &m_numberChannels, 0);
    const bool hasAlphaChannel
        =
        m_numberChannels == 4;
    if (data)
    {
        GenerateTexture(data, hasAlphaChannel);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
}

void Texture::GenerateTexture(const unsigned char* data, bool hasAlphaChannel)
{
    std::cout << "Texture with id: " << m_texture << " loaded " << std::endl;


    // for (auto it = avalaibleTextureSlots.begin(); it != avalaibleTextureSlots.end(); ++it)
    // {
    //     // Dereference the iterator using * to access the element
    //     std::cout << *it << " " << std::endl;
    // }


    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);


    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int mipLevels = static_cast<int>(std::floor(std::log2(std::max(m_widht, m_height)))) + 1;

    GLenum fmt = hasAlphaChannel ? GL_RGBA : GL_RGB;

    glTextureStorage2D(m_texture, mipLevels, hasAlphaChannel ? GL_RGBA8 : GL_RGB8, m_widht, m_height);
    glTextureSubImage2D(m_texture, 0, 0, 0, m_widht, m_height, fmt, GL_UNSIGNED_BYTE,
                        data);
    glGenerateTextureMipmap(m_texture);
}

void Texture::BindTexture() const
{
    glBindTextureUnit(m_textureSlot, m_texture);
}


Texture::Texture()
{
}
