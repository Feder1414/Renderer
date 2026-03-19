//
// Created by USUARIO on 12/18/2025.
//

#ifndef GRAFICOS_TEXTURE_H
#define GRAFICOS_TEXTURE_H
#include <set>

#include "Shader.h"


struct TextureInfo
{
    Shader* shader;
    std::string samplerName;
};


class Texture
{
private:
    int m_width, m_height, m_numberChannels, m_textureSlot;
    std::string m_samplerName;
    inline static bool textureSlotsInitialized;
    unsigned int m_texture;
    inline static std::set<int> avalaibleTextureSlots = std::set<int>();
    inline static int amountSlots = 16;
    Shader* m_shader;

public:
    explicit Texture(TextureInfo texture_info);
    void LoadImage(std::string filePath);
    void GenerateTextureFromImage(const unsigned char* data, bool hasAlphaChannel);
    void BindTexture() const;
    static void InitializeTextureSlots();
    unsigned int GetTextureId() const { return m_texture; }
};


#endif //GRAFICOS_TEXTURE_H
