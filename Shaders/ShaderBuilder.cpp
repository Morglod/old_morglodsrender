#include "ShaderBuilder.hpp"
#include "ShaderCompiler.hpp"
#include "ShaderObjects.hpp"
#include "../Config.hpp"
#include "../Utils/Log.hpp"

char * __SHADER_BUILDER__textFileRead(const char *fn, int * count = nullptr) {
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

std::vector<std::pair<MR::IShaderProgram::Features, MR::IShaderProgram*>> _SHADER_BUILDER__defaultShaders;

namespace MR {

IShaderProgram* ShaderBuilder::Need(const MR::IShaderProgram::Features& req){
    for(size_t _i = 0; _i < _SHADER_BUILDER__defaultShaders.size(); ++_i){
        if(_SHADER_BUILDER__defaultShaders[_i].first == req) {
            return _SHADER_BUILDER__defaultShaders[_i].second;
        }
    }

    std::string fragCode =
        "#version 330\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize(on)\n"
        "layout (location = 0) in vec4 InVertexPos;\n"
        "smooth layout (location = 1) in vec3 InVertexNormal;\n"
        "layout (location = 2) in vec4 InVertexColor;\n"
        "layout (location = 3) in vec2 InVertexTexCoord;\n"
        "out vec4 "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME)+";\n";
    if(req.toRenderTarget) {    fragCode +=
        "out vec4 "+std::string(MR_SHADER_DEFAULT_DEPTH_TEX_COORD_DATA_NAME)+";\n"
        "out vec4 "+std::string(MR_SHADER_DEFAULT_NORMAL_DATA_NAME)+";\n"
        "out vec4 "+std::string(MR_SHADER_DEFAULT_POS_DATA_NAME)+";\n";
    }
    fragCode +=
        "uniform vec3 "+std::string(MR_SHADER_CAM_POS)+";\n"
        "uniform vec3 "+std::string(MR_SHADER_CAM_DIR)+";\n";

    std::string vertCode =
        "#version 330\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize(on)\n"
        "layout (location = 0) in vec3 "+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+";\n"
        "layout (location = 1) in vec3 "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
        "layout (location = 2) in vec4 "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
        "layout (location = 3) in vec2 "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"
        "layout (location = 0) out vec4 OutVertexPos;\n"
        "layout (location = 1) out vec3 OutVertexNormal;\n"
        "layout (location = 2) out vec4 OutVertexColor;\n"
        "layout (location = 3) out vec2 OutVertexTexCoord;\n";

    if(req.toScreen){
        vertCode +=
            "void main() {"
            "	OutVertexPos = vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
            "	OutVertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
            "	OutVertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
            "	OutVertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"
            "	gl_Position = vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
            "}";
    } else {
        vertCode +=
            "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
            "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
            "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
            "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n"
            "void main() {\n"
            "	vec4 vert = "+std::string(MR_SHADER_MVP_MAT4)+" * vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
            "	OutVertexPos = vert;\n"
            "	OutVertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
            "	OutVertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
            "	OutVertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"
            "	gl_Position = vert;\n"
            "}";
    }

    fragCode +=
            "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
            "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
            "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
            "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n";

    if(req.toScreen && req.defferedRendering) {
    fragCode +=
        "uniform sampler2D "+std::string(MR_SHADER_UNIFORM_GBUFFER_ALBEDO)+";\n"
        "uniform sampler2D "+std::string(MR_SHADER_UNIFORM_GBUFFER_DEPT_TEX_COORD)+";\n"
        "uniform sampler2D "+std::string(MR_SHADER_UNIFORM_GBUFFER_NORMAL)+";\n"
        "uniform sampler2D "+std::string(MR_SHADER_UNIFORM_GBUFFER_POS)+";\n"
        "vec4 GetVertexPos() { return texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_POS)+", InVertexTexCoord); }\n"
        "vec3 GetVertexNormal() { return texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_NORMAL)+", InVertexTexCoord).xyz - vec3(1.0, 1.0, 1.0); }\n"
        "vec4 GetVertexColor() { return InVertexColor; }\n"
        "vec2 GetVertexTexCoord() { return texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_DEPT_TEX_COORD)+", InVertexTexCoord).yz; }\n";
    } else {
    fragCode +=
        "vec4 GetVertexPos() { return InVertexPos; }\n"
        "vec3 GetVertexNormal() { return InVertexNormal; }\n"
        "vec4 GetVertexColor() { return InVertexColor; }\n"
        "vec2 GetVertexTexCoord() { return InVertexTexCoord; }\n";
    }

    fragCode +=
        "vec3 PackVertexNormal() { return normalize(vec4(InVertexNormal, 0.0) * "+ std::string(MR_SHADER_MODEL_MAT4) +").xyz + vec3(1.0,1.0,1.0); }";

    //FRAG

    //UNIFORMS
    if(req.ambient){
        fragCode +=
            "uniform sampler2D "+std::string(MR_SHADER_AMBIENT_TEX)+";\n";
    }
    if(req.diffuse){
        fragCode +=
            "uniform sampler2D "+std::string(MR_SHADER_DIFFUSE_TEX)+";\n";
    }
    if(req.opacity){
        fragCode +=
            "uniform sampler2D "+std::string(MR_SHADER_OPACITY_TEX)+";\n";
    }
    if(req.colorFilter){
        fragCode +=
            "uniform vec4 "+std::string(MR_SHADER_COLOR_V4)+";\n";
    }
    if(req.fog){
        fragCode +=
            "uniform float "+std::string(MR_SHADER_FOG_MAX_DIST)+";\n"
            "uniform float "+std::string(MR_SHADER_FOG_MIN_DIST)+";\n"
            "uniform vec4 "+std::string(MR_SHADER_FOG_COLOR)+";\n";
    }

    //MAKE LIGHT
    if( req.light && ((req.toScreen && req.defferedRendering) || !(req.toRenderTarget && req.defferedRendering) )){
        fragCode +=
            "\n"
            "struct PointLight {\n"
            "   vec3 pos;\n"
            "   vec3 emission;\n"
            "   vec3 ambient;\n"
            "   float attenuation;\n"
            "   float power;\n"
            "   float radius;\n"
            "};\n"
            "\n"
            "struct DirLight {\n"
            "   vec3 dir;\n"
            "   vec3 emission;\n"
            "   vec3 ambient;\n"
            "};\n"
            "\n"
            "uniform int "+std::string(MR_SHADER_POINT_LIGHTS_NUM)+";\n"
            "uniform PointLight "+std::string(MR_SHADER_POINT_LIGHTS)+"["+std::to_string(MR_SHADER_MAX_POINT_LIGHTS)+"];\n"
            "\n"
            "uniform int "+std::string(MR_SHADER_DIR_LIGHTS_NUM)+";\n"
            "uniform DirLight "+std::string(MR_SHADER_DIR_LIGHTS)+"["+std::to_string(MR_SHADER_MAX_DIR_LIGHTS)+"];\n"
            "\n"
            "vec3 light(){\n"
            "   vec3 fragLight = vec3(0.0, 0.0, 0.0);\n"
            "   for(int i = 0; i < "+std::string(MR_SHADER_POINT_LIGHTS_NUM)+"; i++){\n"
            "       vec3 light_pos_mvp = (vec4("+std::string(MR_SHADER_POINT_LIGHTS)+"[i].pos, 1.0) * "+std::string(MR_SHADER_MVP_MAT4)+").xyz;\n"
            "       vec3 surfN = GetVertexNormal();\n"
            "       float NdotL = dot(surfN, normalize(light_pos_mvp - GetVertexPos().xyz));\n"
            "       float diff = (NdotL * 0.5) + 0.5;\n"
            "       float Ld = distance(light_pos_mvp, GetVertexPos().xyz);\n"
            "       float att = 1.0 / (1.0 + "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].attenuation * pow(Ld, "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].power));\n"
            "       float Lmult = ("+std::string(MR_SHADER_POINT_LIGHTS)+"[i].radius - Ld) / "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].radius;\n"
            "       Lmult = max(0.0, Lmult);\n" //if(Lmult < 0) Lmult = 0.0;\n"
            "       fragLight += diff * (att * "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].emission) + (Lmult * "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].ambient);\n"
            "   }\n"
            "   for(int di = 0; di < "+std::string(MR_SHADER_DIR_LIGHTS_NUM)+"; di++){\n"
            "       vec3 surfN = GetVertexNormal();\n"
            "       float NdotL = dot(surfN, -"+std::string(MR_SHADER_DIR_LIGHTS)+"[di].dir);\n"
            "       float diff = (NdotL * 0.5) + 0.5;\n"
            "       fragLight += vec3(diff, diff, diff);\n"//max(ld, 0.0) * "+std::string(MR_SHADER_DIR_LIGHTS)+"[di].emission + "+std::string(MR_SHADER_DIR_LIGHTS)+"[di].ambient;\n"
            "   }\n"
            "   return fragLight;\n"
            "}\n";
    }

    /*if(req.fog){
        fragCode +=
            "vec3 fog(){\n"
            "   float fogFactor = gl_FragCoord.z - "+std::string(MR_SHADER_FOG_MIN_DIST)+";\n"
            //"   float fogFactor = ( "+std::string(MR_SHADER_FOG_MAX_DIST)+" - dist ) / ( "+std::string(MR_SHADER_FOG_MAX_DIST)+" - "+std::string(MR_SHADER_FOG_MIN_DIST)+" );\n"
            "   fogFactor = clamp(fogFactor, 0.0, 1.0);\n"
            "   return vec3(fogFactor, fogFactor, fogFactor); //(fogFactor * "+std::string(MR_SHADER_FOG_COLOR)+".xyz * "+std::string(MR_SHADER_FOG_COLOR)+".w);\n"
            "}\n";
    }*/

    //MAKE FRAG MAIN

    fragCode +=
        "\n"
        "void main(){\n";

    if(req.opacity && req.opacityDiscardOnAlpha){
        if(req.opacityDiscardValue == 1.0f){
            fragCode += "   discard;\n";
        } else {
            fragCode +=
                "   if( texture("+std::string(MR_SHADER_OPACITY_TEX)+", GetVertexTexCoord()).x < "+std::to_string(req.opacityDiscardValue)+" ) {\n"
                "       discard;\n"
                "   }\n";
        }
    }

    if(req.toScreen && req.defferedRendering){
        fragCode += "   vec4 albedo = texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_ALBEDO)+", GetVertexTexCoord());\n";
        fragCode += "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME)+" = vec4((albedo.xyz * (-(albedo.w - 1)))";
        if(req.light) fragCode += " + (albedo.w * light()) ";
        fragCode += ", 1.0);\n";
    }
    else {
        fragCode += "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME)+" = ";

        bool bVoidFrag = true;
        bool bAnyColor = false;
        if(req.ambient){
            fragCode +=
                " vec4(texture("+std::string(MR_SHADER_AMBIENT_TEX)+", GetVertexTexCoord()).xyz, 0.0) ";
            bAnyColor = true; bVoidFrag = false;
        }
        if(req.diffuse){
            if(bAnyColor) fragCode += " + ";
            fragCode +=
                "vec4((texture("+std::string(MR_SHADER_DIFFUSE_TEX)+", GetVertexTexCoord()) ";
            if(req.light && !req.toScreen && !(req.toRenderTarget && req.defferedRendering)){
                fragCode += "* vec4(light(), 1.0)";
            }
            fragCode += ").xyz, 1.0)";
            bAnyColor = true; bVoidFrag = false;
        }

        if(req.colorFilter){
            if(bAnyColor) fragCode += " * ";
            fragCode +=
                std::string(MR_SHADER_COLOR_V4)+" ";
            bVoidFrag = false;
        }

        /*if(req.fog){
            if(bAnyColor) fragCode += " + ";
            fragCode += " vec4(fog(), 0.0) ";
            bVoidFrag = false;
        }*/

        if(bVoidFrag) fragCode += " vec4(1.0, 0.0, 0.0, 1.0)";
        fragCode += ";\n";

        if(req.toRenderTarget) {
            fragCode += "   vec4 DEPTHnearColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                        "   vec4 DEPTHfarColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
                        "   float DEPTHnear = 0.0;\n"
                        "   float DEPTHfar = 500.0;\n"
                        "   " + std::string(MR_SHADER_DEFAULT_DEPTH_TEX_COORD_DATA_NAME) + " = vec4(mix(DEPTHnearColor, DEPTHfarColor, smoothstep(DEPTHnear, DEPTHfar, gl_FragCoord.z / gl_FragCoord.w)).x, GetVertexTexCoord().x, GetVertexTexCoord().y, 1.0);\n"
                        "   " + std::string(MR_SHADER_DEFAULT_NORMAL_DATA_NAME) + " = vec4(PackVertexNormal(), 1.0);\n"
                        "   " + std::string(MR_SHADER_DEFAULT_POS_DATA_NAME) + " = GetVertexPos();\n";
        }
    }

    fragCode += "\n}";

    MR::ShaderCompiler compiler;

    ShaderProgram* shader = MR::ShaderProgram::Create(); //dynamic_cast<MR::Shader*>(MR::ShaderManager::Instance()->Create("Auto", "FromSubs"));
    Shader* vertSub = MR::Shader::Create(IShader::ST_Vertex);//new MR::Shader(vertCode, ISubShader::Type::Vertex);
    Shader* fragSub = MR::Shader::Create(IShader::ST_Fragment);//new MR::Shader(fragCode, ISubShader::Type::Fragment);

    if(!compiler.Compile(vertCode, IShaderCompiler::ST_Vertex, vertSub->GetGPUHandle()).Good()) {
        MR::Log::LogString("Failed vertex shader compilation");
        return nullptr;
    }
    if(!compiler.Compile(fragCode, IShaderCompiler::ST_Fragment, fragSub->GetGPUHandle()).Good()) {
        MR::Log::LogString("Failed fragment shader compilation");
        return nullptr;
    }

    if( !vertSub->Attach(shader) || !fragSub->Attach(shader) ) {
        MR::Log::LogString("Failed attaching");
        return nullptr;
    }

    shader->BindDefaultShaderInOut();
    ShaderCompilationOutput out = compiler.Link(StaticArray<unsigned int>(), shader->GetGPUHandle());
    for(size_t i = 0; i < out.GetMessagesNum(); ++i){
        MR::Log::LogString(out.GetMessage(i).GetText());
    }
    if(!out.Good()){
        MR::Log::LogString("Failed linking");
        return nullptr;
    }

    _SHADER_BUILDER__defaultShaders.push_back(std::make_pair(req, shader));
    return shader;
}

IShader* ShaderBuilder::FromFile(const std::string& file, const MR::IShader::Type& type) {
    /*std::string code();

    IShader* shader = new Shader();

    MR::ShaderCompiler compiler;
    complier.Compile(std::string(__SHADER_BUILDER__textFileRead(file.c_str())), type, );*/
    return nullptr;
}

}
