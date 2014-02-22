#pragma once

#ifndef _MR_SHADER_H_
#define _MR_SHADER_H_

#include "Config.hpp"
#include "ResourceManager.hpp"
#include "Events.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR{
    class SubShader;
    class Shader;
    class ShaderManager;

    enum class ShaderUniformTypes : unsigned char {
        FLOAT = 0,
        VEC2 = 1, //glm::vec2 float[2]
        VEC3 = 2, //glm::vec3 float[3]
        VEC4 = 3, //glm::vec4 float[4]
        MAT4 = 4, //glm::mat4 float[4][4]
        INT = 5 //int or sampler2D etc; uniform1i
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
        MR::Event<const unsigned int &, const int&> OnMapped; //shader program, new uniform location

        inline const char* GetName();
        inline ShaderUniformTypes GetType();
        inline int GetOpenGLLocation();

        inline void  SetValuePtr(void* p);
        inline void* GetValuePtr();

        //Call after shader linked
        void MapUniform(const unsigned int & shader_program);

        /* Name - shader uniform name in shader
           Type - type of shader uniform
           Value - pointer to value of uniform
           shader_program - OpenGL shader program object */
        ShaderUniform(const char* Name, const ShaderUniformTypes& Type, void* Value, const unsigned int& shader_program);
    protected:
        const char* _name;
        ShaderUniformTypes _type;
        void* _value;
        int _uniform_location;
    };

    class ShaderUniformBlock {
    public:
        inline int GetOpenGLIndex();
        inline unsigned char* GetData();
        inline const char* GetName();
        inline int GetBlockSize();
        inline int GetNumUniforms();
        inline const char** GetUniformNames();
        inline unsigned int* GetUniformIndecies();
        inline int* GetUniformOffsets();

        //Call it before BufferData method (one time)
        void MapBlock(const unsigned int& shader_program);

        //Generate new ubo and buffer this data
        void BufferData(unsigned char* Data);

        //Firstly change smth in data and then tell, what changed (offset from zero and size)
        void ChangeBufferedData(const int& ChangedPos, const int& ChangedSize);

        ShaderUniformBlock(const char* Name, const int& NumUniforms, const char** UniformNames, const unsigned int & shader_program);
        ~ShaderUniformBlock();
    protected:
        const char* _name;
        unsigned char* _data;
        int _uniform_block_index;
        int _block_size;

        int _num_uniforms;
        const char** _uniform_names; //array size of num_uniforms
        unsigned int * _uniform_indecies; //array size of num_uniforms
        int * _uniform_offsets;

        unsigned int _ubo;
    };

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

        inline unsigned int Get();
        inline SubShader::Type GetType();

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

        /* Create new shader uniform
         *  uniform_name - name of new shader's uniform in shader
         *  type - type of shader uniform
         *  value - pointer to uniform's value */
        inline ShaderUniform* CreateUniform(std::string uniform_name, MR::ShaderUniformTypes type, void* value);
        inline ShaderUniform* CreateUniform(std::string uniform_name, int* value);
        inline ShaderUniform* CreateUniform(std::string uniform_name, float* value);
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::vec2* value);
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::vec3* value);
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::vec4* value);
        inline ShaderUniform* CreateUniform(std::string uniform_name, glm::mat4* value);

        inline ShaderUniformBlock* CreateUniformBlock(const char* Name, const int& NumUniforms, const char** UniformNames);

        inline void DeleteUniform(ShaderUniform* su);

        inline ShaderUniform* FindShaderUniform(std::string uniform_name);

        inline unsigned int GetOpenGLProgram();

        /*  Links sub shaders together (in OpenGL program)
         *  sub_shaders - Array of SubShader objects
         *  num - num of elements in array */
        ///    !! Doesn't attach or save this sub_shaders!!
        void Link(SubShader** sub_shaders, const unsigned int& num);

        //Link attached subs
        void Link();

        inline void AttachSubShader(SubShader* sub);
        inline void DetachSubShader(SubShader* sub);
        inline void DetachAllSubShaders();

        void Use();

        virtual bool Load();
        virtual void UnLoad();

        /*  Links sub shaders together (in OpenGL program)
         *  sub_shaders - Array of SubShader objects
         *  num - num of elements in array */
        Shader(ResourceManager* manager, const std::string& name, const std::string& source);

        /* Deletes OpenGL shader object */
        virtual ~Shader();
    protected:
        unsigned int _program; //OpenGL shader program
        std::vector<SubShader*> _sub_shaders;
        std::vector<ShaderUniform*> _shaderUniforms;
        std::vector<ShaderUniformBlock*> _shaderUniformBlocks;
    };

    class ShaderManager : public virtual MR::ResourceManager{
    public:
        virtual Resource* Create(const std::string& name, const std::string& source);
        inline Shader* NeedShader(const std::string& source);

        ShaderManager() : ResourceManager() {}
        virtual ~ShaderManager() {}

        static ShaderManager* Instance(){
            static ShaderManager* m = new ShaderManager();
            return m;
        }
    };
}

/** INLINES **/

const char* MR::ShaderUniform::GetName(){
    return _name;
}

MR::ShaderUniformTypes MR::ShaderUniform::GetType(){
    return _type;
}

int MR::ShaderUniform::GetOpenGLLocation(){
    return _uniform_location;
}

void MR::ShaderUniform::SetValuePtr(void* p){
    _value = p;
    OnNewValuePtr(this, p);
}

void* MR::ShaderUniform::GetValuePtr(){
    return _value;
}

int MR::ShaderUniformBlock::GetOpenGLIndex() {
    return _uniform_block_index;
}

unsigned char* MR::ShaderUniformBlock::GetData() {
    return _data;
}

const char* MR::ShaderUniformBlock::GetName() {
    return _name;
}

int MR::ShaderUniformBlock::GetBlockSize() {
    return _block_size;
}

int MR::ShaderUniformBlock::GetNumUniforms() {
    return _num_uniforms;
}

const char** MR::ShaderUniformBlock::GetUniformNames() {
    return _uniform_names;
}

unsigned int* MR::ShaderUniformBlock::GetUniformIndecies() {
    return _uniform_indecies;
}

int* MR::ShaderUniformBlock::GetUniformOffsets() {
    return _uniform_offsets;
}

unsigned int MR::SubShader::Get(){
    return this->_shader;
}

MR::SubShader::Type MR::SubShader::GetType(){
    return this->_type;
}

MR::ShaderUniform* MR::Shader::CreateUniform(std::string uniform_name, MR::ShaderUniformTypes type, void* value){
    MR::ShaderUniform* p = new MR::ShaderUniform(uniform_name.c_str(), type, value, _program);
    _shaderUniforms.push_back(p);
    return p;
}

MR::ShaderUniform* MR::Shader::CreateUniform(std::string uniform_name, int* value){
    return CreateUniform(uniform_name, MR::ShaderUniformTypes::INT, value);
}

MR::ShaderUniform* MR::Shader::CreateUniform(std::string uniform_name, float* value){
    return CreateUniform(uniform_name, MR::ShaderUniformTypes::FLOAT, value);
}

MR::ShaderUniform* MR::Shader::CreateUniform(std::string uniform_name, glm::vec2* value){
    return CreateUniform(uniform_name, MR::ShaderUniformTypes::VEC2, value);
}

MR::ShaderUniform* MR::Shader::CreateUniform(std::string uniform_name, glm::vec3* value){
    return CreateUniform(uniform_name, MR::ShaderUniformTypes::VEC3, value);
}

MR::ShaderUniform* MR::Shader::CreateUniform(std::string uniform_name, glm::vec4* value){
    return CreateUniform(uniform_name, MR::ShaderUniformTypes::VEC4, value);
}

MR::ShaderUniform* MR::Shader::CreateUniform(std::string uniform_name, glm::mat4* value){
    return CreateUniform(uniform_name, MR::ShaderUniformTypes::MAT4, value);
}

MR::ShaderUniformBlock* MR::Shader::CreateUniformBlock(const char* Name, const int& NumUniforms, const char** UniformNames) {
    MR::ShaderUniformBlock* p = new MR::ShaderUniformBlock(Name, NumUniforms,UniformNames,_program);
    _shaderUniformBlocks.push_back(p);
    return p;
}

void MR::Shader::DeleteUniform(MR::ShaderUniform* su){
    std::vector<ShaderUniform*>::iterator it = std::find(_shaderUniforms.begin(), _shaderUniforms.end(), su);
    if(it == _shaderUniforms.end()) return;
    delete (*it);
    _shaderUniforms.erase(it);
}

MR::ShaderUniform* MR::Shader::FindShaderUniform(std::string uniform_name){
    for(std::vector<ShaderUniform*>::iterator it = _shaderUniforms.begin(); it != _shaderUniforms.end(); ++it){
        if((*it)->GetName() == uniform_name) return (*it);
    }
    return nullptr;
}

unsigned int MR::Shader::GetOpenGLProgram(){
    return this->_program;
}

MR::Shader* MR::ShaderManager::NeedShader(const std::string& source){
    return dynamic_cast<Shader*>(Need(source));
}

void MR::Shader::AttachSubShader(SubShader* sub){
    _sub_shaders.push_back(sub);
}

void MR::Shader::DetachSubShader(SubShader* sub){
    std::vector<SubShader*>::iterator it = std::find(_sub_shaders.begin(), _sub_shaders.end(), sub);
    if(it == _sub_shaders.end()) return;
    _sub_shaders.erase(it);
}

void MR::Shader::DetachAllSubShaders(){
    _sub_shaders.clear();
}

#endif
