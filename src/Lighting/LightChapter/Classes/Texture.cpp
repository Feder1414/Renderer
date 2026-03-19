//
// Created by USUARIO on 12/18/2025.
//

#include "Texture.h"

#include <array>
#include <iostream>

#include "stb_image.h"
#include <glad/glad.h>


namespace
{
    GLenum TexFormatToGlFormat(TextureFormat texFormat)
    {
        switch (texFormat)
        {
        case TextureFormat::RGBA: return GL_RGBA;
        case TextureFormat::RGB: return GL_RGB;
        default: throw std::exception("Invalid Texture format enum");
        }
    }


    GLenum TexIntFormatToGlFormat(TextureInternalFormat texFormat)
    {
        switch (texFormat)
        {
        case TextureInternalFormat::RGB_8: return GL_RGB8;
        case TextureInternalFormat::RGBA_8: return GL_RGBA8;
        default: throw std::exception("Invalid texture internal format enum");
        }
    }

    GLenum TexWrapToGlWrap(TextureWrapping texWrap)
    {
        switch (texWrap)
        {
        case TextureWrapping::ClampToBorder: return GL_CLAMP_TO_BORDER;
        case TextureWrapping::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case TextureWrapping::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case TextureWrapping::Repeat: return GL_REPEAT;
        default: throw std::exception("Invalid texture wrapping enum");
        }
    }

    GLenum TexFilterToGlFilter(TextureFilter filterMode)
    {
        switch (filterMode)
        {
        case TextureFilter::Linear: return GL_LINEAR;
        case TextureFilter::LinearMipMapLinear: return GL_LINEAR_MIPMAP_LINEAR;
        case TextureFilter::Nearest: return GL_NEAREST;
        default: throw std::exception("Invalid filter mode enum");
        }
    }
}

Texture::Texture(const TextureDesc& texDesc)
{
    m_texDesc = texDesc;
    m_texDesc.mipLevels = std::max(1, m_texDesc.mipLevels);
    CreateTextureFromTexDesc();
}

void Texture::CreateTextureFromTexDesc()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);

    auto uWrapping = TexWrapToGlWrap(m_texDesc.uWrapping);
    auto vWrapping = TexWrapToGlWrap(m_texDesc.vWrapping);

    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, uWrapping);
    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, vWrapping);

    auto minFilter = TexFilterToGlFilter(m_texDesc.minFilter);
    glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, minFilter);

    auto magFilter = TexFilterToGlFilter(m_texDesc.magFilter);
    if (m_texDesc.magFilter != TextureFilter::Linear && m_texDesc.magFilter != TextureFilter::Nearest)
    {
        magFilter = TexFilterToGlFilter(TextureFilter::Linear);
    }

    glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, magFilter);


    auto internalFormat = TexIntFormatToGlFormat(m_texDesc.texIntFormat);
    auto format = TexFormatToGlFormat(m_texDesc.texFormat);


    glTextureStorage2D(m_texture, m_texDesc.mipLevels, internalFormat, m_texDesc.width, m_texDesc.height);
    // glTextureSubImage2D(m_texture, 0, 0, 0, m_texDesc.width, m_texDesc.height, format, GL_UNSIGNED_BYTE,
    //                     nullptr);

    glGenerateTextureMipmap(m_texture);
}

void Texture::LoadImageFromFile(const std::string& filePath, const bool flipY)
{
    m_filePath = filePath;
    stbi_set_flip_vertically_on_load(flipY);
    unsigned char* data = stbi_load(filePath.c_str(), &m_texDesc.width, &m_texDesc.height, &m_texDesc.numberChannels,
                                    0);
    const bool hasAlphaChannel = m_texDesc.numberChannels == 4;
    m_texDesc.texFormat = hasAlphaChannel ? TextureFormat::RGBA : TextureFormat::RGB;
    m_texDesc.texIntFormat = hasAlphaChannel ? TextureInternalFormat::RGBA_8 : TextureInternalFormat::RGB_8;
    if (data)
    {
        GenerateTextureFromImage(data, hasAlphaChannel);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
}

void Texture::GenerateTextureFromImage(const unsigned char* data, bool hasAlphaChannel)
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

    int mipLevels = static_cast<int>(std::floor(std::log2(std::max(m_texDesc.width, m_texDesc.height)))) + 1;
    m_texDesc.mipLevels = mipLevels;


    GLenum fmt = hasAlphaChannel ? GL_RGBA : GL_RGB;

    glTextureStorage2D(m_texture, mipLevels, hasAlphaChannel ? GL_RGBA8 : GL_RGB8, m_texDesc.width, m_texDesc.height);
    glTextureSubImage2D(m_texture, 0, 0, 0, m_texDesc.width, m_texDesc.height, fmt, GL_UNSIGNED_BYTE,
                        data);
    glGenerateTextureMipmap(m_texture);
}

void Texture::LoadCubeMapFromFile(const std::array<std::string, 6>& facesFilePath,
                                  const std::optional<TextureDesc>& texDesc)
{
    if (texDesc.has_value())
    {
        m_texDesc = texDesc.value();
    }
    int width, height, nChannels;

    m_texDesc.texType = TextureType::CubeMap;
    bool loadedTexDesc = false;

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_texture);
    auto mipLevels = 1;
    for (int i = 0; i < facesFilePath.size(); i++)
    {
        unsigned char* data = stbi_load(facesFilePath[i].c_str(), &width, &height, &nChannels, 0);
        if (!data)
        {
            std::cerr << "Failed to load cubemap texture with name: " << facesFilePath[i] << std::endl;
            return;
        }
        if (!loadedTexDesc)
        {
            auto hasAlphaChannel = nChannels == 4;
            m_texDesc.height = height;
            m_texDesc.width = width;
            m_texDesc.texFormat = DetermineTextureFormat(hasAlphaChannel);
            m_texDesc.texIntFormat = DetermineTextureIntFormat(hasAlphaChannel);
            loadedTexDesc = true;
            glTextureStorage2D(m_texture, mipLevels, TexIntFormatToGlFormat(m_texDesc.texIntFormat), m_texDesc.width,
                               m_texDesc.height);
        }
        glTextureSubImage3D(m_texture, 0, 0, 0, i, m_texDesc.width, m_texDesc.height, 1,
                            TexFormatToGlFormat(m_texDesc.texFormat), GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, TexWrapToGlWrap(m_texDesc.uWrapping));
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, TexWrapToGlWrap(m_texDesc.vWrapping));
        glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, TexFilterToGlFilter(m_texDesc.minFilter));
        glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, TexFilterToGlFilter(m_texDesc.magFilter));
    }
}


void Texture::BindTexture() const
{
    glBindTextureUnit(m_textureSlot, m_texture);
}


Texture::Texture()
{
}

void Texture::ResizeTexture(int width, int height)
{
    if (m_texDesc.texType == TextureType::Texture_2D)
    {
        m_texDesc.width = width;
        m_texDesc.height = height;

        glDeleteTextures(1, &m_texture);

        CreateTextureFromTexDesc();
    }
}


TextureFormat Texture::DetermineTextureFormat(bool hasAlphaChannel)
{
    return hasAlphaChannel ? TextureFormat::RGBA : TextureFormat::RGB;
}

TextureInternalFormat Texture::DetermineTextureIntFormat(bool hasAlphaChannel)
{
    return hasAlphaChannel ? TextureInternalFormat::RGBA_8 : TextureInternalFormat::RGB_8;;
}
