#pragma once

#ifndef _MR_SHADER_H_
#define _MR_SHADER_H_

#include "pre.hpp"
#include "ResourceManager.hpp"
#include "Events.hpp"

#define SHADER_VERTEX_POSITION_ATTRIB_LOCATION 0
#define SHADER_VERTEX_NORMAL_ATTRIB_LOCATION 1
#define SHADER_VERTEX_COLOR_ATTRIB_LOCATION 2
#define SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION 3

namespace MR{
    class ShaderManager;

    enum ShaderUniformTypes : unsigned char {
        ShaderUniform_INT_TYPE = 5, //int or sampler2D etc uniform1i
        ShaderUniform_FLOAT_TYPE = 0, //"float" float[1]
        ShaderUniform_VEC2_TYPE = 1, //"vec2" float[2]
        ShaderUniform_VEC3_TYPE = 2, //"vec3" float[3]
        ShaderUniform_VEC4_TYPE = 3, //"vec4" float[4]
        ShaderUniform_MAT4_TYPE = 4 //"mat4" float[4][4]
    };

    /** Named uniform of shader
     *  After shader recompilation, call MapUniform method
     */
    class ShaderUniform {
    protected:
        const char* name;
        ShaderUniformTypes type;
        void* value;
        int uniform_location;

    public:
        MR::Event<void*> OnNewValuePtr;
        MR::Event<const GLenum&, const int&> OnMapped; //shader program, new uniform location

        inline const char* GetName(){ return name; }
        inline ShaderUniformTypes GetType(){ return type; }
        inline void* GetValuePtr(){ return value; }
        inline int GetOpenGLLocation(){ return uniform_location; }

        inline void SetValuePtr(void* p){
            value = p;
            OnNewValuePtr(this, p);
        }

        /** Get new uniform location from shader
         *  Call this method on each shader recompilation (linking)
         */
        inline void MapUniform(const GLenum& shader_program){
            uniform_location = glGetUniformLocationARB(shader_program, name);
            OnMapped(this, shader_program, uniform_location);
        }

        /** Constructor
         *  Name - shader uniform name in shader
         *  Type - type of shader uniform
         *  Value - pointer to value of uniform
         *  shader_program - OpenGL shader program object
         */
        ShaderUniform(const char* Name, const ShaderUniformTypes& Type, void* Value, const GLenum& shader_program);
    };

    class ShaderUniformBlock {
    protected:
        const char* name;
        unsigned char* data;
        int uniform_block_index;
        int block_size;

        int num_uniforms;
        const char** uniform_names; //array size of num_uniforms
        unsigned int * uniform_indecies; //array size of num_uniforms
        int * uniform_offsets;

        GLuint ubo;
    public:
        inline int GetOpenGLIndex() { return uniform_block_index; }
        inline unsigned char* GetData() { return data; }
        inline const char* GetName() { return name; }
        inline int GetBlockSize() { return block_size; }
        inline int GetNumUniforms() { return num_uniforms; }
        inline const char** GetUniformNames() { return uniform_names; }
        inline unsigned int* GetUniformIndecies() { return uniform_indecies; }
        inline int* GetUniformOffsets() { return uniform_offsets; }

        //Call it before BufferData method (one time)
        void MapBlock(const GLenum& shader_program);

        //Generate new ubo and buffer this data
        void BufferData(unsigned char* Data);

        //Firstly change smth in data and then tell, what changed (offset from zero and size)
        void ChangeBufferedData(const int& ChangedPos, const int& ChangedSize);

        ShaderUniformBlock(const char* Name, const int& NumUniforms, const char** UniformNames, const GLenum& shader_program);
        ~ShaderUniformBlock();
    };

    /** OpenGL shader
    */
    class SubShader{
    protected:
        GLenum gl_SHADER, gl_SHADER_TYPE;
    public:
        /** sender - SubShader
         *  arg1 - new code
         *  arg2 - new shader_type
         */
        MR::Event<const char*, GLenum> OnCompiled;

        /** Compiles or Recompiles OpenGL shader
         *  code - OpenGL shader code
         *  shader_type - GL_VERTEX_SHADER_ARB/GL_FRAGMENT_SHADER_ARB etc
         */
        void Compile(const char* code, GLenum shader_type);

        /** Returns OpenGL shader object
         */
        inline GLenum Get(){return this->gl_SHADER;}

        /** Returns OpenGL shader type
         */
        inline GLenum GetType(){return this->gl_SHADER_TYPE;}

        /** Compiles OpenGL shader
         *  code - OpenGL shader code
         *  shader_type - GL_VERTEX_SHADER_ARB/GL_FRAGMENT_SHADER_ARB etc
         */
        SubShader(const char* code, GLenum shader_type);

        /** Deletes OpenGL shader
        */
        ~SubShader();

        /** Load sub shader code from text file and create it.
         *  file - path to file
         *  shader_type - type of OpenGL shader (GL_VERTEX_SHADER_ARB/GL_FRAGMENT_SHADER_ARB etc)
         */
        static SubShader* FromFile(std::string file, GLenum shader_type);
    };

    //!TODO: add Shader resource manager
    class Shader : public virtual Resource {
        GLenum gl_PROGRAM; //OpenGL shader program
        std::vector<ShaderUniform*> shaderUniforms;
        std::vector<ShaderUniformBlock*> shaderUniformBlocks;

    public:
        /** Create new shader uniform
         *  uniform_name - name of new shader's uniform in shader
         *  type - type of shader uniform
         *  value - pointer to uniform's value
         */
        inline ShaderUniform* CreateUniform(std::string uniform_name, MR::ShaderUniformTypes type, void* value){
            MR::ShaderUniform* p = new MR::ShaderUniform(uniform_name.c_str(), type, value, gl_PROGRAM);
            shaderUniforms.push_back(p);
            return p;
        }

        inline ShaderUniform* CreateUniform(std::string uniform_name, int* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::ShaderUniform_INT_TYPE, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, float* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::ShaderUniform_FLOAT_TYPE, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::vec2* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::ShaderUniform_VEC2_TYPE, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::vec3* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::ShaderUniform_VEC3_TYPE, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::vec4* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::ShaderUniform_VEC4_TYPE, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::mat4* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::ShaderUniform_MAT4_TYPE, value); }

        inline ShaderUniformBlock* CreateUniformBlock(const char* Name, const int& NumUniforms, const char** UniformNames, const GLenum& shader_program) {
            MR::ShaderUniformBlock* p = new MR::ShaderUniformBlock(Name, NumUniforms,UniformNames,shader_program);
            shaderUniformBlocks.push_back(p);
            return p;
        }

        /** Delete created uniform
         *  su - shader uniform from shaderUniforms;
         */
        inline void DeleteUniform(ShaderUniform* su){
            std::vector<ShaderUniform*>::iterator it = std::find(shaderUniforms.begin(), shaderUniforms.end(), su);
            if(it == shaderUniforms.end()) return;
            delete (*it);
            shaderUniforms.erase(it);
        }

        /** Find shader uniform by it's name
         *  uniform_name - name of shader's uniform
         */
        inline ShaderUniform* FindShaderUniformByName(std::string uniform_name){
            for(std::vector<ShaderUniform*>::iterator it = shaderUniforms.begin(); it != shaderUniforms.end(); ++it){
                if((*it)->GetName() == uniform_name) return (*it);
            }
            return nullptr;
        }

        /** Returns OpenGL shader object
        */
        inline GLenum Get(){return this->gl_PROGRAM;}

        /** Links sub shaders together (in OpenGL program)
         *  sub_shaders - Array of SubShader objects
         *  num - num of elements in array
         */
        void Link(SubShader** sub_shaders, unsigned int num);

        /** Use this shader
         */
        inline void Use(){
            glUseProgramObjectARB(this->gl_PROGRAM);
            for(auto it = shaderUniforms.begin(); it != shaderUniforms.end(); ++it){
                if((*it)->GetValuePtr() == nullptr) continue;
                if((*it)->GetType() == ShaderUniform_INT_TYPE) glUniform1i((*it)->GetOpenGLLocation(), ((int*)(*it)->GetValuePtr())[0]);
                if((*it)->GetType() == ShaderUniform_FLOAT_TYPE) glUniform1f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0]);
                if((*it)->GetType() == ShaderUniform_VEC2_TYPE) glUniform2f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0], ((float*)(*it)->GetValuePtr())[1]);
                if((*it)->GetType() == ShaderUniform_VEC3_TYPE) glUniform3f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0], ((float*)(*it)->GetValuePtr())[1], ((float*)(*it)->GetValuePtr())[2]);
                if((*it)->GetType() == ShaderUniform_VEC4_TYPE) glUniform4f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0], ((float*)(*it)->GetValuePtr())[1], ((float*)(*it)->GetValuePtr())[2], ((float*)(*it)->GetValuePtr())[3]);
                if((*it)->GetType() == ShaderUniform_MAT4_TYPE) {
                    glUniformMatrix4fv((*it)->GetOpenGLLocation(), 1, GL_FALSE, (float*)&(((glm::mat4*)(*it)->GetValuePtr())[0][0]));
                }
            }
        }

        virtual bool Load();
        virtual void UnLoad();

        /** Links sub shaders together (in OpenGL program)
         *  sub_shaders - Array of SubShader objects
         *  num - num of elements in array
         */
        Shader(ResourceManager* manager, std::string name, std::string source);

        /** Deletes OpenGL shader object
         */
        virtual ~Shader();
    };

    class ShaderManager : public virtual MR::ResourceManager{
    public:
        virtual Resource* Create(std::string name, std::string source);
        inline Shader* NeedShader(const std::string& source){ return dynamic_cast<Shader*>(Need(source)); }

        ShaderManager() : ResourceManager() {}
        virtual ~ShaderManager(){}

        static ShaderManager* Instance(){
            static ShaderManager* m = new ShaderManager();
            return m;
        }
    };
}

#endif
