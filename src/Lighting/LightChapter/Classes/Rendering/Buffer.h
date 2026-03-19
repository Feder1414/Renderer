//
// Created by USUARIO on 3/19/2026.
//

#ifndef GRAFICOS_BUFFER_H
#define GRAFICOS_BUFFER_H
#include <span>
#include <string>
#include <unordered_map>


class VertexLayout;
enum class BindingIndex;

enum class BufferType
{
    VAO, // Only openGL
    Vertex,
    Index,
    Instance,
    Constant,
    Storage
};

enum class BufferUsage
{
    DynamicDraw,
    StaticDraw,
};

struct BufferDesc
{
    std::string name;
    BufferType type;
    BufferUsage usage;
    size_t size;
};

class Buffer
{
public:
    [[nodiscard]] unsigned int GetBufferId() const { return m_buffer; }
    void CreateVao(VertexLayout* vertexLayout, const std::unordered_map<BindingIndex, Buffer*>& bindingIndexToBufferObject);
    void CreateBufferRaw(const BufferDesc& bufferDesc, const void* data);
    void BufferUploadData(const void* data, size_t offset, size_t size);

    template <class T>
    void CreateBuffer(const BufferDesc& bufferDesc, std::span<T> data)
    {
        CreateBufferRaw(bufferDesc, data);
    }

private:
    unsigned int m_buffer;
    BufferDesc m_BufferDesc;
};


#endif //GRAFICOS_BUFFER_H
