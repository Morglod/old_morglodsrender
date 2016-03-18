#include "mr/batch.hpp"
#include "mr/mesh.hpp"
#include "mr/buffer.hpp"
#include "mr/vbuffer.hpp"
#include "mr/ibuffer.hpp"

#include <set>

namespace mr {

class Material;
class VertexDecl;

bool Batch::Create(std::vector<MeshPtr> const& meshes, std::vector<MeshPtr>& out_Meshes) {
    const uint32_t MaxSuperBufferSize = UINT_MAX;
    std::vector<uint32_t> superBufferSizes;

    /**
    Добавить в Primitive
    baseVertex и baseIndex
    и в superBuffer делать отступы, кратные размерам форматов вершин
    тогда при Primitive::Draw не надо будет ребиндить буферы по их отступу; отступ будет делаться непосредственно в glDraw*
    **/

    std::set<Material*> materialsCache;
    std::set<VertexDecl*> vertexDeclCache;
    std::set<IndexDataType> indexDeclCache;

    for(size_t i = 0, n = meshes.size(); i < n; ++i) {
        if(meshes[i] == nullptr) continue;

        auto const& primitive = meshes[i]->GetPrimitive();
        if(primitive == nullptr) continue;

        auto const& vbuffer = primitive->GetVertexBuffer();
        auto const& ibuffer = primitive->GetIndexBuffer();

        uint64_t superSize = 0;
        if(vbuffer != nullptr) {
            superSize += vbuffer->GetBuffer()->GetSize();
            auto const& vertexDeclPtr = vbuffer->GetVertexDecl();
            if(vertexDeclPtr != nullptr) vertexDeclCache.insert(vertexDeclPtr.get()); // because of std::set, only unique ptrs will be inserted.
        }
        if(ibuffer != nullptr) {
            superSize += ibuffer->GetBuffer()->GetSize();
            indexDeclCache.insert(ibuffer->GetDataType());
        }

        while(superSize > MaxSuperBufferSize) {
            superBufferSizes.push_back(MaxSuperBufferSize);
            superSize -= MaxSuperBufferSize;
        }
        if(superSize > 0) {
            if(!superBufferSizes.empty()) {
                if(superBufferSizes[superBufferSizes.size()-1] + superSize > MaxSuperBufferSize) superBufferSizes.push_back(superSize);
                else superBufferSizes[superBufferSizes.size()-1] += superSize;
            }
            else superBufferSizes.push_back(superSize);
        }

        auto const& materialPtr = meshes[i]->GetMaterial();
        if(materialPtr != nullptr) materialsCache.insert(materialPtr.get()); // because of std::set, only unique ptrs will be inserted.
    }

    std::vector<BufferPtr> superBuffers;
    for(size_t i = 0, n = superBufferSizes.size(); i < n; ++i) {
        superBuffers.push_back(Buffer::Create(Memory::Zero(superBufferSizes[i]), Buffer::CreationFlags()));
    }

    // Create new meshes
/*
    for()

    for(size_t i = 0, n = meshes.size(); ++i) {
        if(meshes[i] == nullptr) continue;

        auto const& primitive = meshes[i]->GetPrimitive();
        if(primitive == nullptr) continue;

        auto const& vbuffer = primitive->GetVertexBuffer();
        auto const& ibuffer = primitive->GetIndexBuffer();

        if(vbuffer != nullptr) superSize += vbuffer->GetBuffer()->GetSize();
        if(ibuffer != nullptr) superSize += ibuffer->GetBuffer()->GetSize();
    }*/
    return true;
}

}
