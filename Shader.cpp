#include "Shader.hpp"
#include "Log.hpp"

char *textFileRead(const char *fn, int * count = nullptr) {
    /*std::ifstream file(fn, std::ios::in | std::ios::ate);
    std::ifstream::pos_type fileSize;
    char* fileContents = NULL;
    if(file.is_open()) {
        fileSize = file.tellg();
        fileContents = new char[fileSize+1];
        file.seekg(0, std::ios::beg);
        if(!file.read(fileContents, fileSize)) {
            MR::Log::LogString("Error reading file "+std::string(fn), MR_LOG_LEVEL_ERROR);//error(ERROR_FILE_READING);
        }
        file.close();
        fileContents[fileSize] = '\0';
        if(count != nullptr) *count = (fileSize+1);
    } else {
        MR::Log::LogString("Error openning file "+std::string(fn), MR_LOG_LEVEL_ERROR);//error(ERROR_FILE_NOT_OPENED);
    }

    std::cout << "\n\nFILECONTENT |" << fileContents << "||\n\n";

    return fileContents;*/
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

MR::Shader::Shader(SubShader** sub_shaders, unsigned int num) : _res_free_state(true) {
    this->Link(sub_shaders, num);
}

MR::Shader::~Shader() {
    glDeleteObjectARB(this->gl_PROGRAM);
    if(_res_free_state){
        for(std::vector<ShaderUniform*>::iterator it = shaderUniforms.begin(); it != shaderUniforms.end(); ++it){
            delete (*it);
        }
        shaderUniforms.clear();
    }
}
