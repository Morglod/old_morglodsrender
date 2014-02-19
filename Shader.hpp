#pragma once

#ifndef _MR_SHADER_H_
#define _MR_SHADER_H_

#include "Config.hpp"
#include "ResourceManager.hpp"
#include "Events.hpp"

namespace MR{
    class SubShader;
    class Shader;
    class ShaderManager;

    enum class ShaderUniformTypes : unsigned char {
        FLOAT = 0, //"float" float[1]
        VEC2 = 1, //"vec2" float[2]
        VEC3 = 2, //"vec3" float[3]
        VEC4 = 3, //"vec4" float[4]
        MAT4 = 4, //"mat4" float[4][4]
        INT = 5 //int or sampler2D etc uniform1i
    };

    /** Named uniform of shader
     *  After shader recompilation, call MapUniform method
     */
    class ShaderUniform {
        friend class SubShader;
        friend class Shader;
        friend class ShaderManager;
    public:
        MR::Event<void*> OnNewValuePtr;
        MR::Event<const GLenum&, const int&> OnMapped; //shader program, new uniform location

        inline const char* GetName(){ return _name; }
        inline ShaderUniformTypes GetType(){ return _type; }
        inline void* GetValuePtr(){ return _value; }
        inline int GetOpenGLLocation(){ return _uniform_location; }

        inline void SetValuePtr(void* p){
            _value = p;
            OnNewValuePtr(this, p);
        }

        /** Get new uniform location from shader
         *  Call this method on each shader recompilation (linking)
         */
        void MapUniform(const GLenum& shader_program);

        /** Constructor
         *  Name - shader uniform name in shader
         *  Type - type of shader uniform
         *  Value - pointer to value of uniform
         *  shader_program - OpenGL shader program object
         */
        ShaderUniform(const char* Name, const ShaderUniformTypes& Type, void* Value, const unsigned int& shader_program);
    protected:
        const char* _name;
        ShaderUniformTypes _type;
        void* _value;
        int _uniform_location;
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
        void MapBlock(const unsigned int& shader_program);

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
    public:
        enum class Type : unsigned int {
            Vertex = 0x8B31,
            Fragment = 0x8B30,
            Compute = 0x91B9,
            TessControl = 0x8E88,
            TessEvaluation = 0x8E87,
            Geometry = 0x8DD9
        };

        /** sender - SubShader
         *  arg1 - new code
         *  arg2 - new shader_type
         */
        MR::Event<const char*, const SubShader::Type&> OnCompiled;

        /** Compiles or Recompiles OpenGL shader
         *  code - OpenGL shader code */
        void Compile(const char* code, const SubShader::Type& type);

        inline unsigned int Get(){return this->_shader;}
        inline SubShader::Type GetType(){return this->_type;}

        /** Compiles OpenGL shader */
        SubShader(const char* code, const SubShader::Type& type);

        /** Deletes OpenGL shader */
        ~SubShader();

        /** Load sub shader code from text file and create it.
         *  file - path to file */
        static SubShader* FromFile(const std::string& file, const SubShader::Type& shader_type);
    protected:
        SubShader::Type _type;
        unsigned int _shader;
    };


    class Shader : public virtual Resource {
    public:
        /** Create new shader uniform
         *  uniform_name - name of new shader's uniform in shader
         *  type - type of shader uniform
         *  value - pointer to uniform's value
         */
        inline ShaderUniform* CreateUniform(std::string uniform_name, MR::ShaderUniformTypes type, void* value){
            MR::ShaderUniform* p = new MR::ShaderUniform(uniform_name.c_str(), type, value, _program);
            shaderUniforms.push_back(p);
            return p;
        }

        inline ShaderUniform* CreateUniform(std::string uniform_name, int* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::INT, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, float* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::FLOAT, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::vec2* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::VEC2, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::vec3* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::VEC3, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::vec4* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::VEC4, value); }
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::mat4* value){ return CreateUniform(uniform_name, MR::ShaderUniformTypes::MAT4, value); }

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
        inline GLenum Get(){return this->_program;}

        /** Links sub shaders together (in OpenGL program)
         *  sub_shaders - Array of SubShader objects
         *  num - num of elements in array
         */
        void Link(SubShader** sub_shaders, unsigned int num);

        /** Use this shader
         */
        inline void Use(){
            glUseProgramObjectARB(this->_program);
            for(auto it = shaderUniforms.begin(); it != shaderUniforms.end(); ++it){
                if((*it)->GetValuePtr() == nullptr) continue;
                if((*it)->GetType() == ShaderUniformTypes::INT) glUniform1i((*it)->GetOpenGLLocation(), ((int*)(*it)->GetValuePtr())[0]);
                if((*it)->GetType() == ShaderUniformTypes::FLOAT) glUniform1f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0]);
                if((*it)->GetType() == ShaderUniformTypes::VEC2) glUniform2f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0], ((float*)(*it)->GetValuePtr())[1]);
                if((*it)->GetType() == ShaderUniformTypes::VEC3) glUniform3f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0], ((float*)(*it)->GetValuePtr())[1], ((float*)(*it)->GetValuePtr())[2]);
                if((*it)->GetType() == ShaderUniformTypes::VEC4) glUniform4f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0], ((float*)(*it)->GetValuePtr())[1], ((float*)(*it)->GetValuePtr())[2], ((float*)(*it)->GetValuePtr())[3]);
                if((*it)->GetType() == ShaderUniformTypes::MAT4) {
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
        Shader(ResourceManager* manager, const std::string& name, const std::string& source);

        /** Deletes OpenGL shader object */
        virtual ~Shader();
    protected:
        unsigned int _program; //OpenGL shader program
        std::vector<ShaderUniform*> shaderUniforms;
        std::vector<ShaderUniformBlock*> shaderUniformBlocks;
    };

    class ShaderManager : public virtual MR::ResourceManager{
    public:
        virtual Resource* Create(const std::string& name, const std::string& source);
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
