#pragma once

#include "TextureInterfaces.hpp"
#include "../Images/ImageInterfaces.hpp"

#include <Singleton.hpp>

#include <unordered_set>

namespace mr {

class IGPUBuffer;
class TextureSettings;
class IImageLoader;

class TextureManager : public mu::StaticSingleton<TextureManager> {
public:
    static const int MaxBindlessTextures = 1000;

    struct TextureBindlessPageUBO {
        uint64_t handles[MaxBindlessTextures];
        IGPUBuffer* buf = nullptr;
        unsigned int index = 0;
    };

    ITexture* Create(ITexture::Types const& target);
    TextureSettings* CreateSettings();
    ITexture* CreateMipmapChecker();
    ITexture* FromFile(std::string const& path, IImageLoader::Options const& options = IImageLoader::Options());

    void DestroyAllTextures();

    inline IGPUBuffer* GetBindlessUBO(unsigned int const& index) {
        return _bindless_pages[index].buf;
    }

    inline bool FindFreeBindlessPage(unsigned int freeHandlesNum, unsigned int& out_PageIndex, unsigned int& out_FirstIndex) {
        for(std::pair<unsigned int, TextureBindlessPageUBO> const& page : _bindless_pages) {
            unsigned int free_num = 0;
            unsigned int first_index = 0;
            for(unsigned int i = 0; i < MaxBindlessTextures; ++i) {
                if(page.second.handles[i] == 0) {
                    if(free_num == 0) first_index = i;
                    free_num++;
                }
                if(free_num >= freeHandlesNum) {
                    out_PageIndex = page.second.index;
                    out_FirstIndex = first_index;
                    return true;
                }
            }
        }
        return false;
    }

    inline void PlaceHandleBindless(unsigned int const& pageIndex, unsigned int const& firstIndex, uint64_t* handles, size_t const& handlesNum) {
        uint64_t* pageHandles = _bindless_pages[pageIndex].handles;
        memcpy(&pageHandles[firstIndex], handles, sizeof(uint64_t)*handlesNum);
    }

    inline void SetImageLoader(IImageLoader* imageLoader) {
        _imageLoader = imageLoader;
    }

    inline IImageLoader* GetImageLoader() const {
        return _imageLoader;
    }

    TextureManager();
    virtual ~TextureManager();
private:
    IImageLoader* _imageLoader;

    void _RegisterTexture(ITexture* tex);
    void _UnRegisterTexture(ITexture* tex);
    void _RegisterTextureSampler(TextureSettings* sampler);
    void _UnRegisterTextureSampler(TextureSettings* sampler);
    std::unordered_set<ITexture*> _textures;
    std::unordered_set<TextureSettings*> _textureSamplers;
    std::unordered_map<unsigned int, TextureBindlessPageUBO> _bindless_pages; // <index, cache>
};

}
