#include "Shader.hpp"
#include "Log.hpp"

char *textFileRead(const char *fn, int * count = nullptr) {
    FILE *fp;
    char *content = NULL;

    int icount = 0;

    if (fn != NULL) {
        fp = fopen(fn,"rt");

        if (fp != NULL) {

            fseek(fp, 0, SEEK_END);
            icount = ftell(fp);
            rewind(fp);

            if (icount > 0) {
                content = (char *)malloc(sizeof(char) * (icount+1));
                icount = fread(content,sizeof(char),icount,fp);
                content[icount] = '\0';
            }
            fclose(fp);
        }
    }
    if(count != nullptr) *count = icount;

    return content;
}

MR::ShaderUniform::ShaderUniform(const char* Name, ShaderUniformTypes Type, void* Value, GLenum shader_program) : name(Name), type(Type), value(Value) {
    MapUniform(shader_program);
}

//SUB SHADER
void MR::SubShader::Compile(const char* code, GLenum shader_type) {
    if(this->gl_SHADER != 0) glDeleteObjectARB(this->gl_SHADER);
    this->gl_SHADER_TYPE = shader_type;
    this->gl_SHADER = glCreateShaderObjectARB(shader_type);
    glShaderSourceARB(this->gl_SHADER, 1, &code, NULL);
    glCompileShaderARB(this->gl_SHADER);

    int bufflen = 0;
    glGetShaderiv(this->gl_SHADER, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        GLchar* logString = new GLchar[bufflen + 1];
        glGetShaderInfoLog(this->gl_SHADER, bufflen, 0, logString);
        MR::Log::LogString("Sub shader output: "+std::string(logString), MR_LOG_LEVEL_WARNING);

        delete [] logString;
        logString = 0;
    }

    OnCompiled(this, code, shader_type);
}

MR::SubShader::SubShader(const char* code, GLenum shader_type) {
    this->Compile(code, shader_type);
}

MR::SubShader::~SubShader() {
    glDeleteObjectARB(this->gl_SHADER);
}

MR::SubShader* MR::SubShader::FromFile(std::string file, GLenum shader_type) {
    return new MR::SubShader(textFileRead(file.c_str()), shader_type);
}

//SHADER
void MR::Shader::Link(SubShader** sub_shaders, unsigned int num) {
    if(this->gl_PROGRAM != 0) glDeleteObjectARB(this->gl_PROGRAM);
    this->gl_PROGRAM = glCreateProgramObjectARB();
    for(unsigned int i = 0; i < num; ++i) {
        glAttachObjectARB(this->gl_PROGRAM, sub_shaders[i]->Get());
    }
    glLinkProgramARB(this->gl_PROGRAM);

    int bufflen = 0;
    glGetProgramiv(this->gl_PROGRAM, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        GLchar* logString = new GLchar[bufflen + 1];
        glGetProgramInfoLog(this->gl_PROGRAM, bufflen, 0, logString);
        MR::Log::LogString("Shader output: "+std::string(logString), MR_LOG_LEVEL_WARNING);

        delete [] logString;
        logString = 0;
    }
}

bool MR::Shader::Load(){
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") loading", MR_LOG_LEVEL_INFO);
    if(this->_source != "") {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Importing shader from ("+this->_source+")", MR_LOG_LEVEL_INFO);
        std::ifstream ffile(this->_source, std::ios::in | std::ios::binary);
        if(!ffile.is_open()) return false;

        int sub_shaders_num = 0;
        ffile.read( reinterpret_cast<char*>(&sub_shaders_num), sizeof(int));
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Sub shaders num "+std::to_string(sub_shaders_num));

        MR::SubShader** subs = new MR::SubShader*[sub_shaders_num];

        for(int i = 0; i < sub_shaders_num; ++i){
            unsigned char type = 0;
            ffile.read( reinterpret_cast<char*>(&type), sizeof(unsigned char));
            if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Sub shader type "+std::to_string((int)type));

            int chars_num = 0;
            ffile.read( reinterpret_cast<char*>(&chars_num), sizeof(int));
            if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Sub shader code chars num "+std::to_string(chars_num));

            void* p_code = new unsigned char[chars_num];
            ffile.read( reinterpret_cast<char*>(&((unsigned char*)p_code)[0]), sizeof(unsigned char)*chars_num);

            for (int ci = 0; ci < chars_num; ++ci) {
                ((char*)p_code)[ci] = (char)(( (int) ((unsigned char*)p_code)  [ci])-127);
            }

            //if(this->_manager->debugMessages) MR::Log::LogString("Sub shader code "+std::string(code));

            GLenum sh_type = GL_VERTEX_SHADER;
            switch(type){
            case 0:
                sh_type = GL_COMPUTE_SHADER;
                break;
            case 1:
                sh_type = GL_VERTEX_SHADER;
                break;
            case 2:
                sh_type = GL_TESS_CONTROL_SHADER;
                break;
            case 3:
                sh_type = GL_TESS_EVALUATION_SHADER;
                break;
            case 4:
                sh_type = GL_GEOMETRY_SHADER;
                break;
            case 5:
                sh_type = GL_FRAGMENT_SHADER;
                break;
            }

            subs[i] = new MR::SubShader((char*)p_code, sh_type);
        }

        Link(subs, sub_shaders_num);
    }
    else if(this->_resource_manager->GetDebugMessagesState()){
        MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
        this->_loaded = false;
        return false;
    }
    if(this->gl_PROGRAM == 0){
        MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") loading failed. GL_PROGRAM is null", MR_LOG_LEVEL_ERROR);
        this->_loaded = false;
        return false;
    }

    this->_loaded = true;
    return true;
}
void MR::Shader::UnLoad(){
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") unloading", MR_LOG_LEVEL_INFO);
    glDeleteObjectARB(this->gl_PROGRAM);
}

//--------

MR::Shader::Shader(ResourceManager* manager, std::string name, std::string source) : Resource(manager, name, source) {
    //this->Link(sub_shaders, num);
}

MR::Shader::~Shader() {
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") deleting", MR_LOG_LEVEL_INFO);
    UnLoad();
    if(_res_free_state){
        for(std::vector<ShaderUniform*>::iterator it = shaderUniforms.begin(); it != shaderUniforms.end(); ++it){
            delete (*it);
        }
        shaderUniforms.clear();
    }
}

//SHADER MANAGER

MR::Resource* MR::ShaderManager::Create(std::string name, std::string source){
    if(this->_debugMessages) MR::Log::LogString("ShaderManager "+name+" ("+source+") creating", MR_LOG_LEVEL_INFO);
    Shader * s = new Shader(this, name, source);
    this->_resources.push_back(s);
    return s;
}
