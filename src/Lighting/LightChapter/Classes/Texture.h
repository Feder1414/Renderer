//
// Created by USUARIO on 12/18/2025.
//

#ifndef GRAFICOS_TEXTURE_H
#define GRAFICOS_TEXTURE_H
#include <set>
#include <string>

#include <glm.hpp>
#include <optional>


enum class TextureFormat
{
    RGBA,
    RGB
};

enum class TextureInternalFormat
{
    RGBA_8,
    RGB_8
};

enum class TextureWrapping
{
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
};

enum class TextureFilter
{
    Nearest,
    Linear,
    LinearMipMapLinear
};


enum class TextureType
{
    Texture_2D,
    CubeMap
};


struct TextureDesc
{
    TextureFormat texFormat = TextureFormat::RGBA;
    TextureInternalFormat texIntFormat = TextureInternalFormat::RGBA_8;
    int mipLevels = 1;
    TextureWrapping uWrapping = TextureWrapping::Repeat;
    TextureWrapping vWrapping = TextureWrapping::Repeat;
    glm::vec3 borderColor = glm::vec3(0.0f);
    int width;
    int height;
    int numberChannels;
    TextureFilter magFilter = TextureFilter::Linear;
    TextureFilter minFilter = TextureFilter::Linear;
    TextureType texType = TextureType::Texture_2D;
};

class Texture
{
private:
    int m_textureSlot;
    TextureDesc m_texDesc;
    unsigned int m_texture;
    std::string m_filePath;
    void GenerateTextureFromImage(const unsigned char* data, bool hasAlphaChannel);
    static void InitializeTextureSlots();

public:
    explicit Texture();
    void ResizeTexture(int width, int height);
    static TextureFormat DetermineTextureFormat(bool hasAlphaChannel);
    static TextureInternalFormat DetermineTextureIntFormat(bool hasAlphaChannel);
    Texture(const TextureDesc& texDesc);
    void CreateTextureFromTexDesc();
    void LoadImageFromFile(const std::string& filePath, const bool flipY = true);
    void LoadCubeMapFromFile(const std::array<std::string, 6>& facesFilePath,
                             const std::optional<TextureDesc>& texDesc = std::nullopt);
    int GetTextureSlot() const { return m_textureSlot; };
    unsigned int GetTextureId() const { return m_texture; }
    void SetTextureSlot(unsigned int textureSlot) { m_textureSlot = textureSlot; }
    std::string GetFilePath() { return m_filePath; }
    void BindTexture() const;

    TextureType GetTextureType() { return m_texDesc.texType; }
};


#endif //GRAFICOS_TEXTURE_H
