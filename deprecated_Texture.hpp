#pragma once

#ifndef _MR_TEXTURE_H_
#define _MR_TEXTURE_H_

#include "Config.hpp"
#include "Resources/ResourceManager.hpp"
#include "Utils/Events.hpp"
#include "Utils/Singleton.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

namespace MR{

}

#endif // _MR_TEXTURE_H_


 /*class CubeMap : public virtual Texture {
    public:
        MR::EventListener<CubeMap*, const glm::vec3&> OnCapturePointChanged;

        void SetCapturePoint(const glm::vec3& p);
        inline glm::vec3 GetCapturePoint();

        void Capture(IRenderSystem* context);

        MR::RenderTarget* GetRenderTarget();

        CubeMap(MR::ResourceManager* m, const std::string& name, const std::string& source, const unsigned short & Width, const unsigned short & Height);
        virtual ~CubeMap();
    protected:
        glm::vec3 _world_point_pos;
        MR::Camera* _cam;
        MR::RenderTarget* _rtarget;
    };*/

/*glm::vec3 MR::CubeMap::GetCapturePoint(){
    return _world_point_pos;
}*/

    /*class ITextureArray {
    public:
        virtual unsigned int GetGPUHandle() = 0;
        virtual Texture::InternalFormat GetInternalFormat() = 0;
        virtual Texture::Format GetFormat() = 0;
        virtual Texture::Type GetType() = 0;
        virtual unsigned short GetWidth() = 0;
        virtual unsigned short GetHeight() = 0;
        virtual unsigned short GetDepth() = 0;
        virtual int GetTexturesNum() = 0;

        virtual unsigned int Add(void* newData) = 0; //returns texture index in array
        virtual void Update(const unsigned int& textureIndex, void* newData) = 0;

        virtual bool CanStore(const ITexture::InternalFormat& iformat, const ITexture::Format& format, const ITexture::Type& type, const unsigned short& width, const unsigned short& height) = 0;
    };

    class TextureArray : public IObject, public ITextureArray {
        friend class Texture;
        friend class TextureManager;
    public:
        inline unsigned int GetGPUHandle() override {return _gl_texture;}
        inline Texture::InternalFormat GetInternalFormat() override {return _internal_format;}
        inline Texture::Format GetFormat() override {return _format;}
        inline Texture::Type GetType() override {return _type;}
        inline unsigned short GetWidth() override {return _width;}
        inline unsigned short GetHeight() override {return _height;}
        inline unsigned short GetDepth() override {return _depth;}
        inline int GetTexturesNum() override {return _storedTexNum;}

        //  Should be with current TextureArray.GetWidth/Height/Format/Type/InternalFormat
         //   textureIndex - index of texture in array

        virtual unsigned int Add(void* newData) override; //returns texture index in array
        virtual void Update(const unsigned int& textureIndex, void* newData) override;

        inline bool CanStore(const ITexture::InternalFormat& iformat, const ITexture::Format& format, const ITexture::Type& type, const unsigned short& width, const unsigned short& height) override {
            return ((iformat == _internal_format)&&(format == _format)&&(type == _type)&&(width == _width)&&(height == _height)&&(_storedTexNum < _maxTextures));
        }

        TextureArray(const Texture::InternalFormat& iformat, const Texture::Format& format, const Texture::Type& type, const unsigned short& width, const unsigned short& height, const unsigned int& maxTextures);
        virtual ~TextureArray();

        static bool Support(const Texture::InternalFormat& iformat);
    protected:
        unsigned int _storedTexNum = 0;
        unsigned int _maxTextures;
        unsigned int _gl_texture;
        Texture::InternalFormat _internal_format;
        Texture::Format _format;
        Texture::Type _type;
        unsigned short _width, _height, _depth;
    };
*/
