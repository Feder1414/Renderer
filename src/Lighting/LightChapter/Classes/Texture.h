//
// Created by USUARIO on 12/18/2025.
//

#ifndef GRAFICOS_TEXTURE_H
#define GRAFICOS_TEXTURE_H
#include <set>
#include <string>


class Texture
{
private:
    int m_widht, m_height, m_numberChannels, m_textureSlot;
    std::string m_samplerName;
    inline static bool textureSlotsInitialized;
    unsigned int m_texture;
    inline static std::set<int> avalaibleTextureSlots = std::set<int>();
    inline static int amountSlots = 16;
    void GenerateTexture(const unsigned char* data, bool hasAlphaChannel);
    static void InitializeTextureSlots();

public:
    explicit Texture();
    void LoadImageFromFile(std::string filePath);
    int GetTextureSlot() const { return m_textureSlot; };

    void BindTexture() const;
};


#endif //GRAFICOS_TEXTURE_H
