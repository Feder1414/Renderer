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
    unsigned int m_texture;
    void GenerateTexture(const unsigned char* data, bool hasAlphaChannel);
    static void InitializeTextureSlots();

public:
    explicit Texture();
    void LoadImageFromFile(std::string filePath, bool flipY = true);
    int GetTextureSlot() const { return m_textureSlot; };
    unsigned int GetTextureId() const { return m_texture; }
    void SetTextureSlot(unsigned int textureSlot) { m_textureSlot = textureSlot; }

    void BindTexture() const;
};


#endif //GRAFICOS_TEXTURE_H
