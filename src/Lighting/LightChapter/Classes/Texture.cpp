//
// Created by USUARIO on 12/18/2025.
//

#include "Texture.h"

#include <iostream>

#include "stb_image.h"
#include <glad/glad.h>

class Shader;

void Texture::LoadImageFromFile(std::string filePath)
{
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
    if (!textureSlotsInitialized)
    {
        InitializeTextureSlots();
    }

    if (avalaibleTextureSlots.empty())
    {
        std::cout << "There is no available texture slots or maybe the textureSlotSet is not initialized";
        return;
    }

    m_textureSlot = *avalaibleTextureSlots.begin();
    avalaibleTextureSlots.erase(m_textureSlot);

    std::cout << "Texture with id: " << m_texture << " assigned to slot: " << m_textureSlot << std::endl;


    // for (auto it = avalaibleTextureSlots.begin(); it != avalaibleTextureSlots.end(); ++it)
    // {
    //     // Dereference the iterator using * to access the element
    //     std::cout << *it << " " << std::endl;
    // }


    glGenTextures(1, &m_texture);

    BindTexture();


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_widht, m_height, 0, hasAlphaChannel ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::BindTexture() const
{
    glActiveTexture(GL_TEXTURE0 + m_textureSlot);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::InitializeTextureSlots()
{
    avalaibleTextureSlots = std::set<int>();
    for (int i = 0; i < amountSlots; i++)
    {
        avalaibleTextureSlots.insert(i);
    }
    textureSlotsInitialized = true;
}

Texture::Texture()
{
}
