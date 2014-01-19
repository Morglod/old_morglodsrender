#pragma once

#ifndef _MR_TEXTURE_H_
#define _MR_TEXTURE_H_

//GLEW
#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

#include "ResourceManager.hpp"
#include "Log.hpp"

#define MR_TEXTURE_DIFFUSE_STAGE GL_TEXTURE0

namespace MR{
    class Texture;
    class TextureManager;

    class Texture : public virtual MR::Resource{
    protected:
        unsigned short gl_width = 0, gl_height = 0, gl_depth = 0, gl_internal_format = 0;
        unsigned char gl_red_bits_num = 0, gl_green_bits_num = 0, gl_blue_bits_num = 0, gl_depth_bits_num = 0, gl_alpha_bits_num = 0;
        unsigned int gl_mem_image_size = 0, gl_mem_compressed_img_size = 0;
        bool gl_compressed = false;
        GLuint gl_texture; //OpenGL texture

    public:
        /** sender - Texture object
         *  arg1 - new gl_texture
         */
        MR::Event<GLuint&> OnNewOpenGLTexture;

        inline unsigned short GetWidth() const {
            return gl_width;
        }
        inline unsigned short GetHeight() const {
            return gl_height;
        }
        inline unsigned short GetDepth() const {
            return gl_depth;
        }
        inline unsigned short GetInternalFormat() const {
            return gl_internal_format;
        }
        inline unsigned int GetImageSize() const {
            return gl_mem_image_size;
        }
        inline unsigned int GetCompressedImageSize() const {
            return gl_mem_compressed_img_size;
        }
        inline unsigned char GetRedBitsNum() const {
            return gl_red_bits_num;
        }
        inline unsigned char GetGreenBitsNum() const {
            return gl_green_bits_num;
        }
        inline unsigned char GetBlueBitsNum() const {
            return gl_blue_bits_num;
        }
        inline unsigned char GetDepthBitsNum() const {
            return gl_depth_bits_num;
        }
        inline unsigned char GetAlphaBitsNum() const {
            return gl_alpha_bits_num;
        }
        inline bool IsCompressed() const {
            return gl_compressed;
        }

        /** Returns OpenGL texture
         */
        inline GLuint Get(){return this->gl_texture;}

        /** Sets new OpenGL texture
         */
        inline void Set(GLuint tex){
            gl_texture = tex;
            OnNewOpenGLTexture(this, tex);
        }

        /** Binds this OpenGL texture to specific texture slot (gl_tex_num)
         *  gl_tex_num - GL_TEXTURE0 GL_TEXTURE1 etc
         */
        inline void Bind(GLenum gl_tex_num){
            //if(_manager->debugMessages) MR::Log::LogString("TEXTURE BIND "+this->_name+" "+this->_source);
            glActiveTexture(gl_tex_num);
            glBindTexture(GL_TEXTURE_2D, this->gl_texture);
        }

        virtual bool Load();
        virtual void UnLoad();

        Texture(MR::ResourceManager* manager, std::string name, std::string source);
        virtual ~Texture();
    };

    class TextureManager : public virtual MR::ResourceManager{
    public:

        virtual Resource* Create(std::string name, std::string source);
        virtual Resource* Create(std::string name, unsigned int gl_texture);

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
