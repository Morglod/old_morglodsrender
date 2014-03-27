#pragma once

#ifndef _MR_SHADER_H_
#define _MR_SHADER_H_

#include "Config.hpp"
#include "ResourceManager.hpp"
#include "Events.hpp"
#include "Types.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR{
    class IShader;
    class Shader;

    class ISubShader;
    class SubShader;

    class ShaderManager;
    class RenderContext;

    class IShaderUniform {
    public:
        enum class Types : unsigned char {
            Float = 0,
            Vec2 = 1, //glm::vec2 float[2]
            Vec3 = 2, //glm::vec3 float[3]
            Vec4 = 3, //glm::vec4 float[4]
            Mat4 = 4, //glm::mat4 float[4][4]
            Int = 5 //int or sampler2D etc; uniform1i
        };

        MR::Event<void*> OnNewValuePtr;
        MR::Event<IShader*, const int&> OnMapped; //as args (shader program, new uniform location)

        virtual std::string GetName() = 0;
        virtual Types GetType() = 0;
        virtual void SetValue(void* ptr) = 0;
        virtual void* GetValue() = 0;
        virtual bool Map(IShader* shader) = 0;
        virtual int GetGPULocation() = 0;

        virtual ~IShaderUniform() {}
    };

    /** Named uniform of shader
     *  After shader recompilation, call MapUniform method
     */
    class ShaderUniform : public Super, public IShaderUniform {
        friend class SubShader;
        friend class Shader;
        friend class ShaderManager;
    public:
        inline std::string GetName();
        inline Types GetType();

        inline void  SetValue(void* p) override;
        inline void* GetValue() override;

        //Call after shader linked
        bool Map(IShader* shader) override;

        inline int GetGPULocation() override;

        std::string ToString() override;

        /* Name - shader uniform name in shader
           Type - type of shader uniform
           Value - pointer to value of uniform
           shader_program - OpenGL shader program object */
        ShaderUniform(const char* Name, const ShaderUniform::Types& Type, void* Value, IShader* shader);
        virtual ~ShaderUniform();
    protected:
        std::string _name;
        Types _type;
        void* _value;
        int _uniform_location;
    };

    class IShaderUniformBlock {
    public:
        virtual std::string GetName() = 0;
        virtual unsigned char* GetData() = 0;
        virtual int GetBlockSize() = 0;
        virtual int GetNumUniforms() = 0;
        virtual std::string* GetUniformNames() = 0;
        virtual unsigned int * GetUniformGPUIndecies() = 0;
        virtual int* GetUniformGPUOffsets() = 0;
        virtual bool Map(IShader* shader) = 0;

        /*  If it is first call of this method for this instance, data will be set from (data+offset) ptr
            Else size and offset sets, where data was changed
        */
        virtual bool BufferData(unsigned char* data, const size_t& size, const size_t& offset) = 0;

        virtual ~IShaderUniformBlock(){}
    };

    class ShaderUniformBlock : public Super, public IShaderUniformBlock {
    public:
        inline unsigned char* GetData() override;
        inline std::string GetName() override;
        inline int GetBlockSize() override;
        inline int GetNumUniforms() override;
        inline std::string* GetUniformNames() override;
        inline unsigned int* GetUniformGPUIndecies() override;
        inline int* GetUniformGPUOffsets() override;

        //Call it before BufferData method (one time)
        bool Map(IShader* shader) override;

        bool BufferData(unsigned char* data, const size_t& size, const size_t& offset) override;

        std::string ToString() override;

        ShaderUniformBlock(const std::string& Name, const int& NumUniforms, std::string* UniformNames, IShader* shader);
        virtual ~ShaderUniformBlock();
    protected:
        std::string _name;
        unsigned char* _data;
        int _uniform_block_index;
        int _block_size;

        int _num_uniforms;
        std::string* _uniform_names; //array size of num_uniforms
        unsigned int * _uniform_indecies; //array size of num_uniforms
        int * _uniform_offsets;

        unsigned int _ubo;
    };

    class ISubShader {
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
        MR::Event<const std::string&, const ISubShader::Type&> OnCompiled;

        /** Compiles or Recompiles OpenGL shader
         *  code - OpenGL shader code */
        virtual bool Compile(const std::string& code, const ISubShader::Type& type) = 0;

        virtual unsigned int GetGPUId() = 0;
        virtual ISubShader::Type GetType() = 0;

        virtual ~ISubShader(){}
    };

    class SubShader : public Super, public ISubShader {
    public:

        /** Compiles or Recompiles OpenGL shader
         *  code - OpenGL shader code */
        bool Compile(const std::string& code, const ISubShader::Type& type) override;

        inline unsigned int GetGPUId() override;
        inline ISubShader::Type GetType() override;

        std::string ToString() override;

        /** Compiles OpenGL shader */
        SubShader(const std::string& code, const ISubShader::Type& type);

        /** Deletes OpenGL shader */
        virtual ~SubShader();

        /** Load sub shader code from text file and create it.
         *  file - path to file */
        static SubShader* FromFile(const std::string& file, const ISubShader::Type& shader_type);

        static SubShader* DefaultVert();
        static SubShader* DefaultRTTVert();
        static SubShader* DefaultRTTDiscardVert();
        static SubShader* DefaultScreenVert();

        static SubShader* DefaultFrag();
        static SubShader* DefaultRTTFrag();
        static SubShader* DefaultRTTDiscardFrag();
        static SubShader* DefaultScreenFrag();
    protected:
        ISubShader::Type _type;
        unsigned int _shader;
    };

    class IShader {
    public:
        virtual IShaderUniform* CreateUniform(const std::string& name, const MR::IShaderUniform::Types& type, void* value) = 0;
        virtual IShaderUniformBlock* CreateUniformBlock(const std::string& name, const int& numUniforms, std::string* uniformNames) = 0;
        virtual void DeleteUniform(IShaderUniform* su) = 0;
        virtual void DeleteUniformBlock(IShaderUniformBlock* sub) = 0;
        virtual IShaderUniform* FindShaderUniform(const std::string& name) = 0;
        virtual IShaderUniformBlock* FindShaderUniformBlock(const std::string& name) = 0;
        virtual unsigned int GetGPUProgramId() = 0;
        virtual bool Link(ISubShader** subs, const unsigned int& subsNum) = 0;
        virtual bool Link() = 0;
        virtual void AttachSubShader(ISubShader* sub) = 0;
        virtual void DetachSubShader(ISubShader* sub) = 0;
        virtual void DetachAllSubShaders() = 0;

        virtual bool Use(RenderContext* context) = 0;

        virtual ~IShader(){}
    };

    class Shader : public Super, public IShader, public virtual Resource {
    public:

        /* Create new shader uniform
         *  uniform_name - name of new shader's uniform in shader
         *  type - type of shader uniform
         *  value - pointer to uniform's value */
        IShaderUniform* CreateUniform(const std::string& name, const MR::IShaderUniform::Types& type, void* value) override;
        inline IShaderUniform* CreateUniform(const std::string& name, int* value);
        inline IShaderUniform* CreateUniform(const std::string& name, float* value);
        inline IShaderUniform* CreateUniform(const std::string& name, glm::vec2* value);
        inline IShaderUniform* CreateUniform(const std::string& name, glm::vec3* value);
        inline IShaderUniform* CreateUniform(const std::string& name, glm::vec4* value);
        inline IShaderUniform* CreateUniform(const std::string& name, glm::mat4* value);

        IShaderUniformBlock* CreateUniformBlock(const std::string& name, const int& mumUniforms, std::string* uniformNames) override;

        void DeleteUniform(IShaderUniform* su) override;
        void DeleteUniformBlock(IShaderUniformBlock* sub) override;

        IShaderUniform* FindShaderUniform(const std::string& name) override;
        IShaderUniformBlock* FindShaderUniformBlock(const std::string& name) override;

        inline unsigned int GetGPUProgramId() override;

        std::string ToString() override;

        /*  Links sub shaders together (in OpenGL program)
         *  sub_shaders - Array of SubShader objects
         *  num - num of elements in array */
        ///    !! Doesn't attach or save this sub_shaders!!
        bool Link(ISubShader** sub_shaders, const unsigned int& num) override;

        //Link attached subs
        bool Link() override;

        bool Use(RenderContext* context) override;

        void AttachSubShader(ISubShader* sub) override;
        void DetachSubShader(ISubShader* sub) override;
        void DetachAllSubShaders() override;

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
        std::vector<ISubShader*> _sub_shaders;
        std::vector<IShaderUniform*> _shaderUniforms;
        std::vector<IShaderUniformBlock*> _shaderUniformBlocks;
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

std::string MR::ShaderUniform::GetName(){
    return _name;
}

MR::ShaderUniform::Types MR::ShaderUniform::GetType(){
    return _type;
}

void MR::ShaderUniform::SetValue(void* p){
    _value = p;
    OnNewValuePtr(this, p);
}

void* MR::ShaderUniform::GetValue(){
    return _value;
}

int MR::ShaderUniform::GetGPULocation() {
    return _uniform_location;
}

unsigned char* MR::ShaderUniformBlock::GetData() {
    return _data;
}

std::string MR::ShaderUniformBlock::GetName() {
    return _name;
}

int MR::ShaderUniformBlock::GetBlockSize() {
    return _block_size;
}

int MR::ShaderUniformBlock::GetNumUniforms() {
    return _num_uniforms;
}

std::string* MR::ShaderUniformBlock::GetUniformNames() {
    return _uniform_names;
}

unsigned int* MR::ShaderUniformBlock::GetUniformGPUIndecies() {
    return _uniform_indecies;
}

int* MR::ShaderUniformBlock::GetUniformGPUOffsets() {
    return _uniform_offsets;
}

unsigned int MR::SubShader::GetGPUId(){
    return this->_shader;
}

MR::SubShader::Type MR::SubShader::GetType(){
    return this->_type;
}

MR::IShaderUniform* MR::Shader::CreateUniform(const std::string& uniform_name, int* value){
    return CreateUniform(uniform_name, MR::ShaderUniform::Types::Int, value);
}

MR::IShaderUniform* MR::Shader::CreateUniform(const std::string& uniform_name, float* value){
    return CreateUniform(uniform_name, MR::ShaderUniform::Types::Float, value);
}

MR::IShaderUniform* MR::Shader::CreateUniform(const std::string& uniform_name, glm::vec2* value){
    return CreateUniform(uniform_name, MR::ShaderUniform::Types::Vec2, value);
}

MR::IShaderUniform* MR::Shader::CreateUniform(const std::string& uniform_name, glm::vec3* value){
    return CreateUniform(uniform_name, MR::ShaderUniform::Types::Vec3, value);
}

MR::IShaderUniform* MR::Shader::CreateUniform(const std::string& uniform_name, glm::vec4* value){
    return CreateUniform(uniform_name, MR::ShaderUniform::Types::Vec4, value);
}

MR::IShaderUniform* MR::Shader::CreateUniform(const std::string& uniform_name, glm::mat4* value){
    return CreateUniform(uniform_name, MR::ShaderUniform::Types::Mat4, value);
}

unsigned int MR::Shader::GetGPUProgramId(){
    return this->_program;
}

MR::Shader* MR::ShaderManager::NeedShader(const std::string& source){
    return dynamic_cast<Shader*>(Need(source));
}

#endif
