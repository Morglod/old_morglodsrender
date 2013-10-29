#pragma once

#ifndef _MR_SHADER_H_
#define _MR_SHADER_H_

#include "MorglodsRender.h"

namespace MR{
    const unsigned short ShaderVar_INT_TYPE = 4; //int or sampler2D etc uniform1i
    const unsigned short ShaderVar_FLOAT_TYPE = 0; //"float" float[1]
    const unsigned short ShaderVar_VEC2_TYPE = 1; //"vec2" float[2]
    const unsigned short ShaderVar_VEC3_TYPE = 2; //"vec3" float[3]
    const unsigned short ShaderVar_VEC4_TYPE = 3; //"vec4" float[4]

    //--------------------------------------------------
    //Unique object for each shader; named var for shader
    class ShaderVar{
    public:
        const char* name = "unnamed";
        unsigned short type = ShaderVar_FLOAT_TYPE;
        void* value = NULL;
        int uniform_location = 0; //uniform location

        ShaderVar(){}
        ShaderVar(const char* Name, unsigned short Type, void* Value, GLenum shader_program){
            this->name = Name;
            this->uniform_location = glGetUniformLocationARB(shader_program, Name);
            this->type = Type;
            this->value = Value;
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

    class Shader{
        GLenum gl_PROGRAM; //OpenGL shader program
    public:
        ShaderVar** ShaderVars = NULL; //pointer to array of vars; used in Use function
        unsigned int ShaderVarsNum = 0; //num of elements in ShaderVars

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
            for(unsigned int i = 0; i < this->ShaderVarsNum; ++i){
                if(this->ShaderVars[i]->type == ShaderVar_INT_TYPE) glUniform1i(this->ShaderVars[i]->uniform_location, ((int*)this->ShaderVars[i]->value)[0]);
                if(this->ShaderVars[i]->type == ShaderVar_FLOAT_TYPE) glUniform1f(this->ShaderVars[i]->uniform_location, ((float*)this->ShaderVars[i]->value)[0]);
                if(this->ShaderVars[i]->type == ShaderVar_VEC2_TYPE) glUniform2f(this->ShaderVars[i]->uniform_location, ((float*)this->ShaderVars[i]->value)[0], ((float*)this->ShaderVars[i]->value)[1]);
                if(this->ShaderVars[i]->type == ShaderVar_VEC3_TYPE) glUniform3f(this->ShaderVars[i]->uniform_location, ((float*)this->ShaderVars[i]->value)[0], ((float*)this->ShaderVars[i]->value)[1], ((float*)this->ShaderVars[i]->value)[2]);
                if(this->ShaderVars[i]->type == ShaderVar_VEC4_TYPE) glUniform4f(this->ShaderVars[i]->uniform_location, ((float*)this->ShaderVars[i]->value)[0], ((float*)this->ShaderVars[i]->value)[1], ((float*)this->ShaderVars[i]->value)[2], ((float*)this->ShaderVars[i]->value)[3]);
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
