//
// Created by USUARIO on 12/24/2025.
//

#ifndef GRAFICOS_VERTEXLAYOUT_H
#define GRAFICOS_VERTEXLAYOUT_H
#include <string>
#include <vector>
#include <unordered_map>
#include <windows.h>
#include "OpenGL.h"

enum class VertexAttributeType
{
    BOOL,
    INT,
    UINT,
    FLOAT,
    DOUBLE,
};

struct VertexAttributeTypeInfo
{
    GLenum glType;
    int size;
};

struct VertexAttribute
{
    VertexAttributeType type; // Usuario
    GLenum glType;
    std::string attributeName; // Usuario
    int sizeOfType;
    int amountComponents; //Usuario
    std::size_t offset;
    int stride;
};


class VertexLayout
{
    inline static std::unordered_map<VertexAttributeType, VertexAttributeTypeInfo> attribTypeToGlType =
    {
        {VertexAttributeType::BOOL, {GL_BOOL, sizeof(bool)}},
        {VertexAttributeType::INT, {GL_INT, sizeof(int)}},
        {VertexAttributeType::UINT, {GL_UNSIGNED_INT, sizeof(unsigned int)}},
        {VertexAttributeType::FLOAT, {GL_FLOAT, sizeof(float)}},
        {VertexAttributeType::DOUBLE, {GL_DOUBLE, sizeof(double)}}


    };
    std::vector<VertexAttribute> m_verticesAttributes = {};
    std::vector<int> accumulatedSize = {};


    std::vector<VertexAttribute> GetVertexAttribute()
    {
        return m_verticesAttributes;
    }

public:
    VertexLayout(const std::vector<VertexAttribute>& verticesAttributes);

    void SetAttribsVao(unsigned int vao) const;
};


#endif //GRAFICOS_VERTEXLAYOUT_H
