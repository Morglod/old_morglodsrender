#pragma once

#ifndef _MR_SHADER_H_
#define _MR_SHADER_H_

#include "MorglodsRender.h"
//#include "ResourceManager.h"

namespace MR{
    enum ShaderUniformTypes : unsigned short {
        ShaderUniform_INT_TYPE = 4, //int or sampler2D etc uniform1i
        ShaderUniform_FLOAT_TYPE = 0, //"float" float[1]
        ShaderUniform_VEC2_TYPE = 1, //"vec2" float[2]
        ShaderUniform_VEC3_TYPE = 2, //"vec3" float[3]
        ShaderUniform_VEC4_TYPE = 3 //"vec4" float[4]
    };

    //--------------------------------------------------
    //Unique object for each shader; named var for shader
    struct ShaderUniform{
    public:
        const char* name;
        ShaderUniformTypes type;
        void* value;
        int uniform_location; //uniform location

        ShaderUniform() : name("none"), type(ShaderUniformTypes::ShaderUniform_FLOAT_TYPE), value(nullptr), uniform_location(0){}
        ShaderUniform(const char* Name, ShaderUniformTypes Type, void* Value, GLenum shader_program) : name(Name), type(Type), value(Value){
            uniform_location = glGetUniformLocationARB(shader_program, Name);
        }
    };

    class SubShader{
    protected:
        GLenum gl_SHADER, gl_SHADER_TYPE;
    public:
        //------------------------------------
        //Compiles or Recompiles OpenGL shader
        //code - OpenGL shader code
        //shader_type - GL_VERTEX_SHADER_ARB/GL_FRAGMENT_SHADER_ARB etc
        void Compile(const char* code, GLenum shader_type);

        //----------------------------
        //Returns OpenGL shader object
        inline GLenum Get(){return this->gl_SHADER;}

        //--------------------------
        //Returns OpenGL shader type
        inline GLenum GetType(){return this->gl_SHADER_TYPE;}

        //----------------------
        //Compiles OpenGL shader
        //code - OpenGL shader code
        //shader_type - GL_VERTEX_SHADER_ARB/GL_FRAGMENT_SHADER_ARB etc
        SubShader(const char* code, GLenum shader_type);

        //---------------------
        //Deletes OpenGL shader
        ~SubShader();
    };

    //!TODO: add Shader resource manager
    class Shader/* : public virtual Resource*/{
        GLenum gl_PROGRAM; //OpenGL shader program
    public:
        std::vector<ShaderUniform*> ShaderUniforms;

        //----------------------------
        //Returns OpenGL shader object
        inline GLenum Get(){return this->gl_PROGRAM;}

        //Links sub shaders together (in OpenGL program)
        //sub_shaders - Array of SubShader objects
        //num - num of elements in array
        void Link(SubShader** sub_shaders, unsigned int num);

        //----------------
        //Uses this shader
        inline void Use(){
            glUseProgramObjectARB(this->gl_PROGRAM);
            ShaderUniform* currentUni = nullptr;
            for(auto it = ShaderUniforms.begin(); it != ShaderUniforms.end(); ++it){
                currentUni = (*it);
                if(currentUni->type == ShaderUniform_INT_TYPE) glUniform1i(currentUni->uniform_location, ((int*)currentUni->value)[0]);
                if(currentUni->type == ShaderUniform_FLOAT_TYPE) glUniform1f(currentUni->uniform_location, ((float*)currentUni->value)[0]);
                if(currentUni->type == ShaderUniform_VEC2_TYPE) glUniform2f(currentUni->uniform_location, ((float*)currentUni->value)[0], ((float*)currentUni->value)[1]);
                if(currentUni->type == ShaderUniform_VEC3_TYPE) glUniform3f(currentUni->uniform_location, ((float*)currentUni->value)[0], ((float*)currentUni->value)[1], ((float*)currentUni->value)[2]);
                if(currentUni->type == ShaderUniform_VEC4_TYPE) glUniform4f(currentUni->uniform_location, ((float*)currentUni->value)[0], ((float*)currentUni->value)[1], ((float*)currentUni->value)[2], ((float*)currentUni->value)[3]);
            }
        }

        //----------------------------------------------
        //Links sub shaders together (in OpenGL program)
        //sub_shaders - Array of SubShader objects
        //num - num of elements in array
        Shader(SubShader** sub_shaders, unsigned int num);

        //----------------------------
        //Deletes OpenGL shader object
        ~Shader();
    };
}

#endif
