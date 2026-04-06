//
// Created by USUARIO on 3/19/2026.
//

#ifndef GRAFICOS_BUFFER_H
#define GRAFICOS_BUFFER_H
#include <memory>
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

enum class BufferStorage
{
    None = 0,
    DynamicStorage = 1 << 0,
    MapRead = 1 << 1,
    MapWrite = 1 << 2,
    MapPersistent = 1 << 3,
    MapCoherent = 1 << 4,
    ClientStorage = 1 << 5,
};

inline BufferStorage operator&(BufferStorage lo, BufferStorage ro)
{
    return static_cast<BufferStorage>(static_cast<int>(lo) & static_cast<int>(ro));
}

inline BufferStorage operator|(BufferStorage lo, BufferStorage ro)
{
    return static_cast<BufferStorage>(static_cast<int>(lo) | static_cast<int>(ro));
}


enum class BufferUsage
{
    DynamicDraw,
    StaticDraw,
};

struct BufferDesc
{
    std::string name;
    BufferType type;
    BufferStorage storage;
    size_t size;
    size_t factorRealloc = 1024;
};

struct ResizeHelper
{
    unsigned int factorRealloc = 1000;
};

class Buffer
{
public:
    [[nodiscard]] unsigned int GetBufferId() const { return m_buffer; }
    void CreateVao(VertexLayout* vertexLayout,
                   const std::unordered_map<BindingIndex, std::unique_ptr<Buffer>>& bindingIndexToBufferObject,
                   Buffer* ebo);
    void CreateBufferRaw(const BufferDesc& bufferDesc, const void* data);
    void ReassignVBOToVAO(const Buffer* buffer, BindingIndex bindingIndex, size_t offset, size_t stride);
    void CreateBufferFromBuffDesc(const void* data);
    void Resize(size_t newSize, const void* data = nullptr);

    void ResizeWithHelper(size_t factorRealloc, size_t totalSize, size_t extraRealloc, const void* data = nullptr);


    void BufferUploadData(const void* data, size_t offset, size_t size, bool& needsReallocate);

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
