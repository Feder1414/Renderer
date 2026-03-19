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

enum class VertexPredefinedAttributes
{
    POSITION,
    COLOR,
    UV,
    NORMAL,
    TANGENT,
    BITANGENT,
    BONES,
    BONES_WEIGHT,
    TRANSFORM,
};

struct VertexAttributeTypeInfo
{
    GLenum glType;
    int size;
};

enum class BindingIndex
{
    Vertex,
    Instance,
};

struct VertexAttribute
{
    VertexAttributeType type; // Usuario
    GLenum glType;
    std::string attributeName; // Usuario
    int sizeOfType;
    int amountComponents; //Usuario
    std::size_t offset;
    unsigned int offsetComponents;
    int stride;
    bool perInstance = false; // Ususario
    BindingIndex vbo = BindingIndex::Vertex;
    unsigned int location; // Automatic
    bool interLeaved = true;
};


struct VBOAttribs
{
    std::vector<unsigned int> accumulatedSize = {};
    std::vector<VertexAttribute> vertexAttribs = {};
    unsigned int amountComponents = 0;
    bool isPerInstance = false;
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


    //Have the info of all the vertex layout as it was only in one VBO, helping to read the vertex data from the cpu;
    VBOAttribs m_linearVboAttribs = {};


    unsigned int m_componentsPerVertex = 0;
    std::size_t m_accumulatedSizeInstanceComponents = 0;

    std::unordered_map<BindingIndex, VBOAttribs> m_vboToAttributes = {};
    std::unordered_map<std::string, VertexAttribute*> m_vertexNameToLinearVertexAttrib = {};
    std::unordered_map<std::string, unsigned int> m_attribNameToLocation = {};

    std::vector<VertexAttribute> m_originalVertexAttribs = {};

public:
    VertexLayout(const std::vector<VertexAttribute>& verticesAttributes, bool oneVBO = true);
    void ProcessVertexAttribsInterleaved(VBOAttribs& vboAttribs);
    const auto& GetVertexNameToLinearAttrib() const { return m_vertexNameToLinearVertexAttrib; } ;

    void SetAttribsVao(unsigned int vao) const;

    const auto& GetVboAttribs() const { return m_vboToAttributes; }

    ;

    unsigned int GetComponentsPerVertex() const { return m_componentsPerVertex; }

    const auto& GetAttribNameToLocation() { return m_attribNameToLocation; }

    std::string static VertexPredefinedAttrToString(VertexPredefinedAttributes attr, unsigned int uvIndex = 0);
    //std::size_t GetBytesPerVertex();

    const auto& GetVertexNameToLinearVertexAttrib()
    {
        return m_vertexNameToLinearVertexAttrib;
    }

    const auto& GetLinearVboAttribs()
    {
        return m_linearVboAttribs;
    }

    const auto& GetOriginalVerticesAttribs()
    {
        return m_originalVertexAttribs;
    }

    std::size_t GetSizeBytesInstanceAttributes() const
    {
        return m_accumulatedSizeInstanceComponents;
    }

    unsigned int GetVertexAttribLocation(const std::string& attribName) const;
    //const auto& GetAttribNameToLocation() { return m_attribNameToLocation; }
};


#endif //GRAFICOS_VERTEXLAYOUT_H
