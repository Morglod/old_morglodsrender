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

namespace MR{
    class Texture;
    class TextureManager;

    class Texture : public virtual MR::Resource{
    protected:
        std::string _name;
        std::string _source;
        bool _loaded = false;
        TextureManager* _manager = nullptr;

        unsigned short gl_width = 0, gl_height = 0, gl_depth = 0, gl_internal_format = 0;
        unsigned char gl_red_bits_num = 0, gl_green_bits_num = 0, gl_blue_bits_num = 0, gl_depth_bits_num = 0, gl_alpha_bits_num = 0;
        unsigned int gl_mem_image_size = 0, gl_mem_compressed_img_size = 0;
        bool gl_compressed = false;

        bool _res_free_state = true;

    public:

        //TEXTURE-----------------------------

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

        GLuint gl_texture; //OpenGL texture

        //----------------------
        //Returns OpenGL texture
        inline GLuint Get(){return this->gl_texture;}

        //Binds this OpenGL texture to specific texture slot (gl_tex_num)
        //gl_tex_num - GL_TEXTURE0 GL_TEXTURE1 etc
        inline void Bind(GLenum gl_tex_num){
            //if(_manager->debugMessages) MR::Log::LogString("TEXTURE BIND "+this->_name+" "+this->_source);
            glActiveTexture(gl_tex_num);
            glBindTexture(GL_TEXTURE_2D, this->gl_texture);
        }

        //RESOURCE-----------------------------
        inline void SetResourceFreeState(bool s){
            _res_free_state = s;
        }

        inline bool GetResourceFreeState(){
            return _res_free_state;
        }

        virtual inline std::string GetName(){return this->_name;}
        virtual inline std::string GetSource(){return this->_source;}
        virtual bool Load();
        virtual void UnLoad();
        virtual bool ReLoad();
        virtual inline bool IsLoaded(){return this->_loaded;}
        inline ResourceManager* GetManager(){return reinterpret_cast<ResourceManager*>(this->_manager);}

        Texture();
        Texture(MR::ResourceManager* manager, std::string name, std::string source);

        //----------------------
        //Deletes OpenGL texture
        virtual ~Texture();
    };

    class TextureManager : public virtual MR::ResourceManager{
    protected:
        std::vector<Texture*> _resources;
    public:
        bool debugMessages = false;
        inline void Add(Resource* res){
            this->_resources.push_back( dynamic_cast<Texture*>(res) );
        }
        inline void Remove(Resource* res){
            auto it = std::find(this->_resources.begin(), this->_resources.end(), res);
            if(it != this->_resources.end()){
                delete (*it);
                this->_resources.erase(it);
            }
        }
        virtual Resource* Create(std::string name, std::string source);
        virtual Resource* Create(std::string name, unsigned int gl_texture);
        virtual Resource* CreateAndLoad(std::string name, std::string source);
        virtual Resource* Get(std::string source);
        virtual Resource* Find(std::string name);
        virtual Resource* Need(std::string source);
        virtual Resource** Need(std::string* sources, const unsigned int num);
        virtual void Remove(std::string name, std::string source);
        virtual void RemoveAll();

        TextureManager(){}
        virtual ~TextureManager(){}

        static TextureManager* Instance(){
            static TextureManager* m = new TextureManager();
            return m;
        }
    };

    //------------------------------------------
    //Creates and loads OpenGL texture from file
    //Returns 0 if error has occured, you can get error text SOIL_last_result()
    inline GLuint LoadGLTexture(std::string file){return SOIL_load_OGL_texture(file.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS);}

    //------------------------------------------------------------
    //Loads OpenGL texture from file to specific OpenGL texture id
    //t - OpenGL texture id
    //Returns 0 if error has occured, you can get error text SOIL_last_result()
    inline GLuint LoadGLTexture(std::string file, GLuint t){return SOIL_load_OGL_texture(file.c_str(),SOIL_LOAD_AUTO,t,SOIL_FLAG_MIPMAPS);}
}

#endif // _MR_TEXTURE_H_
