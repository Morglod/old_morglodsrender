#pragma once

#ifndef _MR_TEXTURE_H_
#define _MR_TEXTURE_H_

//GLEW
#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

//SIMPLE IMAGE LIBRARY
#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#   include <SOIL.h>
#endif

#include "ResourceManager.h"
#include "Log.h"

#include <algorithm>
#include <vector>

namespace MR{
    class Texture;
    class TextureManager;

    class Texture : public virtual MR::Resource{
    protected:
        std::string _name = "";
        std::string _source = "";
        bool _loaded = false;
        TextureManager* _manager = nullptr;
    public:
        GLuint gl_texture; //OpenGL

        //----------------------
        //Returns OpenGL texture
        inline GLuint Get(){return this->gl_texture;}

        //Binds this OpenGL texture to specific texture slot (gl_tex_num)
        //gl_tex_num - GL_TEXTURE0 GL_TEXTURE1 etc
        inline void Bind(GLenum gl_tex_num){
            MR::Log::LogString("TEXTURE BIND "+this->_name+" "+this->_source);
            glActiveTexture(gl_tex_num);
            glBindTexture(GL_TEXTURE_2D, this->gl_texture);
        }

        virtual inline std::string GetName(){return this->_name;}
        virtual inline std::string GetSource(){return this->_source;}
        virtual bool Load();
        virtual void UnLoad();
        virtual bool ReLoad();
        virtual inline bool IsLoaded(){return this->_loaded;}
        inline ResourceManager* GetManager(){return (ResourceManager*)this->_manager;}

        Texture();
        Texture(MR::TextureManager* manager, std::string name, std::string source);

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
