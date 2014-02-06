#pragma once

#ifndef _MR_TEXTURE_H_
#define _MR_TEXTURE_H_

#include "pre.hpp"
#include "ResourceManager.hpp"
#include "Log.hpp"

#define MR_TEXTURE_DIFFUSE_STAGE GL_TEXTURE0

namespace MR{
    class Texture;
    class TextureManager;

    class TextureSettings : public Copyable<TextureSettings> {
    public:
        TextureSettings* Copy();

        void SetLodBias(const float& v);
        void SetBorderColor(float* rgba);
        void SetBorderColor(const float& r, const float& g, const float& b, const float& a);
        void SetMagFilter(const int& v);
        void SetMinFilter(const int& v);
        void SetMinLod(const int& v);
        void SetMaxLod(const int& v);
        void SetWrapS(const int& v);
        void SetWrapR(const int& v);
        void SetWrapT(const int& v);
        void SetCompareMode(const int& v);
        void SetCompareFunc(const int& v);

        inline GLuint& GetGLSampler(){return _sampler;}
        inline float& GetLodBias(){return _GL_TEXTURE_LOD_BIAS;}
        inline float* GetBorderColor(){return _GL_TEXTURE_BORDER_COLOR;}
        inline int& GetMagFilter(){return _GL_TEXTURE_MAG_FILTER;}
        inline int& GetMinFilter(){return _GL_TEXTURE_MIN_FILTER;}
        inline int& GetMinLod(){return _GL_TEXTURE_MIN_LOD;}
        inline int& GetMaxLod(){return _GL_TEXTURE_MAX_LOD;}
        inline int& GetWrapS(){return _GL_TEXTURE_WRAP_S;}
        inline int& GetWrapR(){return _GL_TEXTURE_WRAP_R;}
        inline int& GetWrapT(){return _GL_TEXTURE_WRAP_T;}
        inline int& GetCompareMode(){return _GL_TEXTURE_COMPARE_MODE;}
        inline int& GetCompareFunc(){return _GL_TEXTURE_COMPARE_FUNC;}

        TextureSettings();
        virtual ~TextureSettings();
    protected:
        GLuint _sampler;
        float _GL_TEXTURE_LOD_BIAS;
        float _GL_TEXTURE_BORDER_COLOR[4];
        int _GL_TEXTURE_MAG_FILTER, _GL_TEXTURE_MIN_FILTER,
            _GL_TEXTURE_MIN_LOD, _GL_TEXTURE_MAX_LOD,
            _GL_TEXTURE_WRAP_S, _GL_TEXTURE_WRAP_T, _GL_TEXTURE_WRAP_R,
            _GL_TEXTURE_COMPARE_MODE, _GL_TEXTURE_COMPARE_FUNC;
    };

    class Texture : public virtual MR::Resource {
    protected:
        unsigned short gl_width = 0, gl_height = 0, gl_depth = 0, gl_internal_format = 0;
        unsigned char gl_red_bits_num = 0, gl_green_bits_num = 0, gl_blue_bits_num = 0, gl_depth_bits_num = 0, gl_alpha_bits_num = 0;
        unsigned int gl_mem_image_size = 0, gl_mem_compressed_img_size = 0;
        bool gl_compressed = false;
        GLuint gl_texture; //OpenGL texture
        TextureSettings* _settings;

    public:
        /** sender - Texture object
         *  arg1 - new gl_texture
         */
        MR::Event<const GLuint&> OnGLTextureChanged;

        /** sender - this
         *  arg1 - ModeS
         *  arg2 - ModeT
         */
        MR::Event<const GLint&, const GLint&> OnWrappingChanged;

        /** sender - this
         *  arg1 - NULL
         */
        MR::Event<void*> OnInfoReset;

        /** mipMapLevel, internalFormat, width, height, format, type, data
         *  invokes from SetData method
         */
        MR::Event<const int&, const int&, const int&, const int&, const GLenum&, const GLenum&, void*> OnDataChanged;

        /** mipMapLevel, xOffset, yOffset, width, height, format, type, data
         *  invokes form UpdateData method
         */
        MR::Event<const int&, const int&, const int&, const int&, const int&, const GLenum&, const GLenum&, void*> OnDataUpdated;

        inline TextureSettings* GetSettings() const { return _settings; } //May be nullptr
        inline unsigned short GetWidth() const { return gl_width; }
        inline unsigned short GetHeight() const { return gl_height; }
        inline unsigned short GetDepth() const { return gl_depth; }
        inline unsigned short GetInternalFormat() const { return gl_internal_format; }
        inline unsigned int GetImageSize() const { return gl_mem_image_size; }
        inline unsigned int GetCompressedImageSize() const { return gl_mem_compressed_img_size; }
        inline unsigned char GetRedBitsNum() const { return gl_red_bits_num; }
        inline unsigned char GetGreenBitsNum() const { return gl_green_bits_num; }
        inline unsigned char GetBlueBitsNum() const { return gl_blue_bits_num; }
        inline unsigned char GetDepthBitsNum() const { return gl_depth_bits_num; }
        inline unsigned char GetAlphaBitsNum() const { return gl_alpha_bits_num; }
        inline bool IsCompressed() const { return gl_compressed; }
        inline GLuint GetGLTexture() const { return this->gl_texture; }
        inline unsigned int * GetGLTextureP() { return &this->gl_texture; }

        inline void SetGLTexture(const GLuint& tex){
            if(gl_texture != tex){
                gl_texture = tex;
                OnGLTextureChanged(this, tex);
            }
        }

        inline void SetSettings(TextureSettings* ts) {
            if(_settings != ts){
                _settings = ts;
                ///TODO: EVENTS
            }
        }

        /** Get fresh info about texture */
        void ResetInfo();

        void GetData(const int& mipMapLevel, const GLenum& format, const GLenum& type, void* dstBuffer);
        void SetData(const int& mipMapLevel, const int& internalFormat, const int& width, const int& height, const GLenum& format, const GLenum& type, void* data);
        void UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const GLenum& format, const GLenum& type, void* data);

        virtual bool Load();
        virtual void UnLoad();

        //OPENGL MISC
        static void CreateOpenGLTexture(GLuint* TexDst, GLint InternalFormat = GL_RGB, GLsizei Width = 512, GLsizei Height = 512, GLint Format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE);
        //-----------

        Texture(MR::ResourceManager* manager, std::string name, std::string source);
        virtual ~Texture();
    };

    class TextureManager : public virtual MR::ResourceManager {
    public:
        virtual Resource* Create(std::string name, std::string source);
        virtual Resource* Create(std::string name, unsigned int gl_texture);
        inline Texture* NeedTexture(const std::string& source) { return dynamic_cast<Texture*>(Need(source)); }

        TextureManager() : ResourceManager() {}
        virtual ~TextureManager(){}

        static TextureManager* Instance(){
            static TextureManager* m = new TextureManager();
            return m;
        }
    };

    /** Creates and loads OpenGL texture from file
     *  Returns 0 if error has occured, you can get error text SOIL_last_result()
     */
    inline GLuint LoadGLTexture(std::string file){return SOIL_load_OGL_texture(file.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS);}

    /** Loads OpenGL texture from file to specific OpenGL texture id
     *  t - OpenGL texture id
     *  Returns 0 if error has occured, you can get error text SOIL_last_result()
     */
    inline GLuint LoadGLTexture(std::string file, GLuint t){return SOIL_load_OGL_texture(file.c_str(),SOIL_LOAD_AUTO,t,SOIL_FLAG_MIPMAPS);}
}

#endif // _MR_TEXTURE_H_
