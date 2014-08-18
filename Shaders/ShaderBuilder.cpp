#include "ShaderBuilder.hpp"
#include "ShaderCompiler.hpp"
#include "ShaderObjects.hpp"
#include "ShaderConfig.hpp"
#include "../Utils/Log.hpp"
#include "../Utils/Containers.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

bool MR::ShaderBuilder::Params_Texture::operator == (const ShaderBuilder::Params_Texture& p) const {
    return (name == p.name) && (d == p.d);
}

bool MR::ShaderBuilder::Params_Uniform::operator == (const ShaderBuilder::Params_Uniform& p) const {
    return (name == p.name) && (type == p.type);
}

bool MR::ShaderBuilder::Params::operator == (const MR::ShaderBuilder::Params& p) const {
    return  (features == p.features) && (textures == p.textures) &&
            (uniforms == p.uniforms) && (customVertexCode == p.customVertexCode) &&
            (customVertexFuncName == p.customVertexFuncName) && (customFragmentCode == p.customFragmentCode) &&
            (customFragmentFuncName == p.customFragmentFuncName);
}

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

std::vector<std::pair<MR::ShaderBuilder::Params, MR::IShaderProgram*>> _SHADER_BUILDER__defaultShaders;

class __ShaderBuilderCacheKey {
public:
    MR::ShaderBuilder::Params f;
    MR::IShader::Type t;

    bool operator == (__ShaderBuilderCacheKey & key){
        return (key.f == f && key.t == t);
    }

    __ShaderBuilderCacheKey() {}
    __ShaderBuilderCacheKey(const MR::ShaderBuilder::Params& req, const MR::IShader::Type& type) : f(req), t(type) {}
};

MR::PriorityCache<__ShaderBuilderCacheKey, std::string> __SHADER_BUILDER_generated_code(MR_SHADER_BUILDER_CODE_CACHE_SIZE);

namespace MR {

std::string ShaderBuilder::GenerateCode(const ShaderBuilder::Params& req, const MR::IShader::Type& type) {
    std::string code = "";
    if(__SHADER_BUILDER_generated_code.Get( __ShaderBuilderCacheKey(req, type), code )){
        return code;
    }

    /*if(type == MR::IShader::ST_Geometry){
        code =
        "#version 150 core\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "layout (triangles) in;\n"
        "layout (triangle_strip,  max_vertices=3) out;\n"
        ///Inputs
        "layout (location = 0) in vec3 "+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+"[3];\n"
        "layout (location = 1) in vec3 "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+"[3];\n"
        "layout (location = 2) in vec4 "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+"[3];\n"
        "layout (location = 3) in vec2 "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+"[3];\n"
        ///Outputs
        "layout (location = 0) out vec4 gsOutVertexPos;\n"
        "layout (location = 1) out vec3 gsOutVertexNormal;\n"
        "layout (location = 2) out vec4 gsOutVertexColor;\n"
        "layout (location = 3) out vec2 gsOutVertexTexCoord;\n";
        "void main() {\n"
        "   gl_Position = gl_in[0].gl_Position;\n"
        "   gsOutVertexPos = gl_in[0].gl_Position;\n"
        "   gsOutVertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+"[0];\n"
        "   gsOutVertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+"[0];\n"
        "   gsOutVertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+"[0];\n"
        "   EmitVertex();\n"
        "   gl_Position = gl_in[1].gl_Position;\n"
         "   gsOutVertexPos = gl_in[1].gl_Position;\n"
        "   gsOutVertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+"[1];\n"
        "   gsOutVertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+"[1];\n"
        "   gsOutVertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+"[1];\n"
        "   EmitVertex();\n"
        "   gl_Position = gl_in[2].gl_Position;\n"
         "   gsOutVertexPos = gl_in[2].gl_Position;\n"
        "   gsOutVertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+"[2];\n"
        "   gsOutVertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+"[2];\n"
        "   gsOutVertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+"[2];\n"
        "   EmitVertex();\n"
        "   EndPrimitive();\n"
        "}\n";
    }
    else*/ if(type == MR::IShader::ST_Vertex) {
        code =
        "#version 150 core\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize (on)\n"
        "#pragma optionNV(fastmath on)\n"
        "#pragma optionNV(fastprecision on)\n"
        "#pragma optionNV(ifcvt none)\n"
        "#pragma optionNV(inline all)\n"
        "#pragma optionNV(strict on)\n"
        "#pragma optionNV(unroll all)\n"
        "precision mediump float;\n";

        ///Inputs
        code +=
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION)+") in vec3 "+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION)+") in vec3 "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION)+") in vec4 "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION)+") in vec2 "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n";

        if(!req.features.toScreen){
            code +=
            "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
            "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
            "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
            "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n";
        }

        for(size_t it = 0; it < req.uniforms.size(); ++it) {
            code += "uniform " + MR::IShaderUniform::TypeToString(req.uniforms[it].type) + " " + req.uniforms[it].name + ";\n";
        }

        for(size_t it = 0; it < req.textures.size(); ++it) {
            std::string type = "";
            switch(req.textures[it].d) {
            case MR::ShaderBuilder::Params_Texture::D_1D:
                type = "sampler1D";
                break;
            case MR::ShaderBuilder::Params_Texture::D_2D:
                type = "sampler2D";
                break;
            case MR::ShaderBuilder::Params_Texture::D_3D:
                type = "sampler3D";
                break;
            }
            code += "uniform " + type + " " + req.textures[it].name + ";\n";
        }

        ///Outputs
        code +=
        "out vec4 MR_VertexPos;\n"
        "out vec4 MR_LocalVertexPos;\n"
        "out vec3 MR_VertexNormal;\n"
        "out vec4 MR_VertexColor;\n"
        "out vec2 MR_VertexTexCoord;\n";

        ///Functions

        //Add custom code
        if(req.customVertexCode != "" && req.customVertexFuncName != "")
            code += "\n\n" + req.customVertexCode + "\n\n";

        ///Main
        if(req.features.toScreen){
            if(req.customVertexCode == "" || req.customVertexFuncName == "") {
                code +=
                "void main() {"
                "   vec4 pos = vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);"
                "	MR_VertexPos = pos;\n"
                "   MR_LocalVertexPos = pos;\n"
                "	MR_VertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
                "	MR_VertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
                "	MR_VertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"
                "	gl_Position = pos;\n"
                "}";
            } else {
                code +=
                "void main() {"
                "   vec4 pos = vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
                "   "+req.customVertexFuncName+"(pos, pos, "
                                                +std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+", MR_VertexNormal, "
                                                +std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+", MR_VertexColor, "
                                                +std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+", MR_VertexTexCoord);\n"
                "	MR_VertexPos = pos;\n"
                "   MR_LocalVertexPos = pos;\n"
                "	gl_Position = pos;\n"
                "}";
            }
        } else {
            if(req.customVertexCode == "" || req.customVertexFuncName == "") {
                code +=
                "void main() {\n"
                "	vec4 pos = "+std::string(MR_SHADER_MVP_MAT4)+" * vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
                "	MR_VertexPos = pos;\n"
                "   MR_LocalVertexPos = vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
                "   MR_VertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
                "	MR_VertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
                "	MR_VertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"
                "	gl_Position = pos;\n"
                "}";
            } else {
                code +=
                "void main() {\n"
                "	vec4 pos = "+std::string(MR_SHADER_MVP_MAT4)+" * vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
                "   "+req.customVertexFuncName+"(pos, pos, "
                                +std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+", MR_VertexNormal, "
                                +std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+", MR_VertexColor, "
                                +std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+", MR_VertexTexCoord);\n"
                "	MR_VertexPos = pos;\n"
                "   MR_LocalVertexPos = vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
                "	gl_Position = pos;\n"
                "}";
            }
        }
    } else if(type == MR::IShader::ST_Fragment) {
        code =
        "#version 150 core\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize (on)\n"
        "#pragma optionNV(fastmath on)\n"
        "#pragma optionNV(fastprecision on)\n"
        "#pragma optionNV(ifcvt none)\n"
        "#pragma optionNV(inline all)\n"
        "#pragma optionNV(strict on)\n"
        "#pragma optionNV(unroll all)\n"
        "precision mediump float;\n";

        ///Inputs
        code +=
        "in vec4 MR_VertexPos;\n"
        "in vec4 MR_LocalVertexPos;\n"
        "smooth in vec3 MR_VertexNormal;\n"
        "in vec4 MR_VertexColor;\n"
        "in vec2 MR_VertexTexCoord;\n"
        "\n"
        "uniform vec3 "+std::string(MR_SHADER_CAM_POS)+";\n"
        "uniform vec3 "+std::string(MR_SHADER_CAM_DIR)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n";

        if(req.features.toScreen && req.features.defferedRendering) {
            code +=
            "uniform sampler2D "+std::string(MR_SHADER_UNIFORM_GBUFFER_1)+";\n"
            "uniform sampler2D "+std::string(MR_SHADER_UNIFORM_GBUFFER_2)+";\n"
            "uniform sampler2D "+std::string(MR_SHADER_UNIFORM_GBUFFER_3)+";\n";
        }

        if(req.features.ambient) code += "uniform sampler2D "+std::string(MR_SHADER_AMBIENT_TEX)+";\n";
        if(req.features.diffuse) code += "uniform sampler2D "+std::string(MR_SHADER_DIFFUSE_TEX)+";\n";
        if(req.features.opacity) code += "uniform sampler2D "+std::string(MR_SHADER_OPACITY_TEX)+";\n";
        if(req.features.colorFilter) code += "uniform vec4 "+std::string(MR_SHADER_COLOR_V4)+";\n";

        if(req.features.fog) { code +=
            "uniform float "+std::string(MR_SHADER_FOG_MAX_DIST)+";\n"
            "uniform float "+std::string(MR_SHADER_FOG_MIN_DIST)+";\n"
            "uniform vec4 "+std::string(MR_SHADER_FOG_COLOR)+";\n";
        }

        if(req.features.light){
            code +=
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
            "\n";
        }

        for(size_t it = 0; it < req.uniforms.size(); ++it) {
            code += "uniform " + MR::IShaderUniform::TypeToString(req.uniforms[it].type) + " " + req.uniforms[it].name + ";\n";
        }

        for(size_t it = 0; it < req.textures.size(); ++it) {
            std::string type = "";
            switch(req.textures[it].d) {
            case MR::ShaderBuilder::Params_Texture::D_1D:
                type = "sampler1D";
                break;
            case MR::ShaderBuilder::Params_Texture::D_2D:
                type = "sampler2D";
                break;
            case MR::ShaderBuilder::Params_Texture::D_3D:
                type = "sampler3D";
                break;
            }
            code += "uniform " + type + " " + req.textures[it].name + ";\n";
        }

        ///Outputs
        code +=
        "out vec4 "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+";\n";

        if(req.features.toRenderTarget) {
            code +=
            "out vec4 "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_2)+";\n"
            "out vec4 "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_3)+";\n";
        }

        ///Functions
        if(req.features.toScreen && req.features.defferedRendering) {
            code +=
            //Initializated in main()
            "vec4 "+std::string(MR_SHADER_UNIFORM_GBUFFER_1)+"_value;" //scene color + light model
            "vec4 "+std::string(MR_SHADER_UNIFORM_GBUFFER_2)+"_value;" //normal + nothing
            "vec4 "+std::string(MR_SHADER_UNIFORM_GBUFFER_3)+"_value;" //pos + depth
            "vec4 GetVertexPos() { return vec4("+std::string(MR_SHADER_UNIFORM_GBUFFER_3)+"_value.xyz, 1.0); }\n"
            "vec3 GetVertexNormal() { return "+std::string(MR_SHADER_UNIFORM_GBUFFER_2)+"_value.xyz; }\n"
            "vec4 GetVertexColor() { return MR_VertexColor; }\n"
            "vec2 GetVertexTexCoord() { return MR_VertexTexCoord; }\n";
        } else {
            code +=
            "vec4 GetVertexPos() { return MR_VertexPos; }\n"
            "vec3 GetVertexNormal() { return MR_VertexNormal; }\n"
            "vec4 GetVertexColor() { return MR_VertexColor; }\n"
            "vec2 GetVertexTexCoord() { return MR_VertexTexCoord; }\n";
        }

        code += "vec4 GetLocalPos() { return MR_LocalVertexPos; }\n";

        code +=
        "vec3 PackVertexNormal() { return normalize(vec4(MR_VertexNormal, 0.0) * "+ std::string(MR_SHADER_MODEL_MAT4) +").xyz + vec3(1.0,1.0,1.0); }";

        if(req.features.light) {
            code +=
            "vec3 light(){\n"
            "   vec3 fragLight = vec3(0.0, 0.0, 0.0);\n"
            "   for(int i = 0; i < "+std::string(MR_SHADER_POINT_LIGHTS_NUM)+"; i++){\n"
            "       vec3 light_pos_mvp = "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].pos;\n"
            "       vec3 surfN = GetVertexNormal();\n"
            "       float NdotL = dot(surfN, normalize(light_pos_mvp - ("+std::string(MR_SHADER_MODEL_MAT4)+" * GetVertexPos()).xyz));\n"
            "       float diff = (NdotL * 0.5) + 0.5;\n"
            "       float Ld = distance(light_pos_mvp, ("+std::string(MR_SHADER_MODEL_MAT4)+" * GetVertexPos()).xyz);\n"
            //"       float Ld = distance("+std::string(MR_SHADER_POINT_LIGHTS)+"[i].pos, ("+std::string(MR_SHADER_MODEL_MAT4)+" * GetVertexPos()).xyz);\n"
            "       float att = 1.0 / (1.0 + "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].attenuation * pow(Ld, "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].power));\n"
            "       float Lmult = ("+std::string(MR_SHADER_POINT_LIGHTS)+"[i].radius - Ld) / "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].radius;\n"
            "       Lmult = max(0.0, Lmult);\n" //if(Lmult < 0) Lmult = 0.0;\n"
            "       fragLight += diff * (att * "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].emission) + (Lmult * "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].ambient);\n"
            //"       fragLight += (att * "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].emission) + (Lmult * "+std::string(MR_SHADER_POINT_LIGHTS)+"[i].ambient);\n"
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
            code +=
            "vec3 fog(){\n"
            "   float fogFactor = gl_FragCoord.z - "+std::string(MR_SHADER_FOG_MIN_DIST)+";\n"
            //"   float fogFactor = ( "+std::string(MR_SHADER_FOG_MAX_DIST)+" - dist ) / ( "+std::string(MR_SHADER_FOG_MAX_DIST)+" - "+std::string(MR_SHADER_FOG_MIN_DIST)+" );\n"
            "   fogFactor = clamp(fogFactor, 0.0, 1.0);\n"
            "   return vec3(fogFactor, fogFactor, fogFactor); //(fogFactor * "+std::string(MR_SHADER_FOG_COLOR)+".xyz * "+std::string(MR_SHADER_FOG_COLOR)+".w);\n"
            "}\n";
        }*/

        //Add custom code
        if(req.customFragmentCode != "" && req.customFragmentFuncName != "") {
            code += "\n\n" + req.customFragmentCode + "\n\n";
        }

        ///Main
        code +=
        "\n"
        "void main() {\n";

        bool noDiscardBreak = true;
        if(req.features.opacity && req.features.opacityDiscardOnAlpha){
            if(req.features.opacityDiscardValue == 1.0f){
                code += "   discard;\n";
                noDiscardBreak = false;
            } else {
                code +=
                    "   if( texture("+std::string(MR_SHADER_OPACITY_TEX)+", GetVertexTexCoord()).x < "+std::to_string(req.features.opacityDiscardValue)+" ) {\n"
                    "       discard;\n"
                    "   }\n";
            }
        }

        if(noDiscardBreak) {
            if(req.features.toScreen && req.features.defferedRendering) {
                if(req.customFragmentCode != "" && req.customFragmentFuncName != "") {
                    code +=
                    "   "+std::string(MR_SHADER_UNIFORM_GBUFFER_1)+"_value = texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_1)+", InVertexTexCoord);\n"
                    "   "+std::string(MR_SHADER_UNIFORM_GBUFFER_2)+"_value = texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_2)+", InVertexTexCoord);\n"
                    "   "+std::string(MR_SHADER_UNIFORM_GBUFFER_3)+"_value = texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_3)+", InVertexTexCoord);\n"
                    "   "+std::string(MR_SHADER_UNIFORM_GBUFFER_2)+"_value.xyz -= vec3(1.0, 1.0, 1.0);\n";
                    code += "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+" = " + req.customFragmentFuncName + "( vec4("+std::string(MR_SHADER_UNIFORM_GBUFFER_1)+"_value.xyz, 1.0) );\n";
                    if(req.features.light) code +=
                    "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+" *= vec4(light(), 1.0);\n";
                } else {
                    code +=
                    "   "+std::string(MR_SHADER_UNIFORM_GBUFFER_1)+"_value = texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_1)+", InVertexTexCoord);\n"
                    "   "+std::string(MR_SHADER_UNIFORM_GBUFFER_2)+"_value = texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_2)+", InVertexTexCoord);\n"
                    "   "+std::string(MR_SHADER_UNIFORM_GBUFFER_3)+"_value = texture("+std::string(MR_SHADER_UNIFORM_GBUFFER_3)+", InVertexTexCoord);\n"
                    "   "+std::string(MR_SHADER_UNIFORM_GBUFFER_2)+"_value.xyz -= vec3(1.0, 1.0, 1.0);\n";
                    code += "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+" = vec4("+std::string(MR_SHADER_UNIFORM_GBUFFER_1)+"_value.xyz, 1.0);\n";
                    if(req.features.light) code +=
                    "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+" *= vec4(light(), 1.0);\n";
                }
            }
            else {
                code += "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+" = ";

                if(req.customFragmentCode != "" && req.customFragmentFuncName != "") {
                    code += req.customFragmentFuncName + "( ";
                }

                bool bVoidFrag = true;
                bool bAnyColor = false;
                if(req.features.ambient){
                    code +=
                        " vec4(texture2D("+std::string(MR_SHADER_AMBIENT_TEX)+", GetVertexTexCoord()).xyz, 1.0) ";
                    bAnyColor = true; bVoidFrag = false;
                }
                if(req.features.diffuse){
                    if(bAnyColor) code += " + ";
                    code +=
                        "vec4((texture("+std::string(MR_SHADER_DIFFUSE_TEX)+", GetVertexTexCoord()) ";
                    if(req.features.light && !req.features.toScreen){
                        code += "* vec4(light(), 1.0)";
                    }
                    code += ").xyz, 1.0)";

                    bAnyColor = true; bVoidFrag = false;
                }

                if(req.features.colorFilter){
                    if(bAnyColor) code += " * ";
                    code +=
                        std::string(MR_SHADER_COLOR_V4)+" ";
                    bVoidFrag = false;
                }

                /*if(req.fog){
                    if(bAnyColor) code += " + ";
                    code += " vec4(fog(), 0.0) ";
                    bVoidFrag = false;
                }*/

                if(bVoidFrag) code += " vec4(1.0, 0.0, 0.0, 1.0)";
                if(req.customFragmentCode != "" && req.customFragmentFuncName != "") {
                    code += ")";
                }
                code += ";\n";

                if(req.features.toRenderTarget) {
                    code += //"   vec4 DEPTHnearColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                                //"   vec4 DEPTHfarColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
                                //"   float DEPTHnear = 0.0;\n"
                                //"   float DEPTHfar = 500.0;\n"
                                "   " + std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_2) + " = vec4(PackVertexNormal(), 1.0);\n"
                                "   " + std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_3) + " = GetVertexPos();\n";
                }
            }
        }

        code +=
        "}\n"; //end main
    }

    __SHADER_BUILDER_generated_code.Store( __ShaderBuilderCacheKey(req, type), code );

    //MR::Log::LogString("CODE |"+code+"|ENDCODE");

    return code;
}

IShaderProgram* ShaderBuilder::Need(const MR::ShaderBuilder::Params& req){
    for(size_t _i = 0; _i < _SHADER_BUILDER__defaultShaders.size(); ++_i){
        if(_SHADER_BUILDER__defaultShaders[_i].first == req) {
            return _SHADER_BUILDER__defaultShaders[_i].second;
        }
    }

    std::string vertCode = GenerateCode(req, MR::IShader::ST_Vertex), fragCode = GenerateCode(req, MR::IShader::ST_Fragment)/*, geomCode = GenerateCode(req, MR::IShader::ST_Geometry)*/;

    MR::ShaderCompiler compiler;

    ShaderProgram* shader = MR::ShaderProgram::Create(); //dynamic_cast<MR::Shader*>(MR::ShaderManager::Instance()->Create("Auto", "FromSubs"));
    Shader* vertSub = MR::Shader::Create(IShader::ST_Vertex);//new MR::Shader(vertCode, ISubShader::Type::Vertex);
    Shader* fragSub = MR::Shader::Create(IShader::ST_Fragment);//new MR::Shader(fragCode, ISubShader::Type::Fragment);
    //Shader* geomSub = MR::Shader::Create(IShader::ST_Geometry);

    if(!compiler.Compile(vertCode, IShaderCompiler::ST_Vertex, vertSub->GetGPUHandle()).Good()) {
        MR::Log::LogString("Failed vertex shader compilation");
        return nullptr;
    }
    if(!compiler.Compile(fragCode, IShaderCompiler::ST_Fragment, fragSub->GetGPUHandle()).Good()) {
        MR::Log::LogString("Failed fragment shader compilation");
        return nullptr;
    }
    /*if(!compiler.Compile(geomCode, IShaderCompiler::ST_Geometry, geomSub->GetGPUHandle()).Good()) {
        MR::Log::LogString("Failed geometry shader compilation");
        return nullptr;
    }*/

    if( !vertSub->Attach(shader) || !fragSub->Attach(shader) /*|| !geomSub->Attach(shader)*/ ) {
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
    MR::Log::LogString("ShaderBuilder::FromFile is not yet implemented. Sorry.", MR_LOG_LEVEL_WARNING);
    return nullptr;
}

IShaderProgram* ShaderBuilder::SimpleMake(const std::string& vertex_shader, const std::string& fragment_shader) {
    MR::ShaderCompiler sc;
    MR::IShader* vshader = new MR::Shader(glCreateShader(GL_VERTEX_SHADER), (MR::IShader::Type)GL_VERTEX_SHADER);
    MR::ShaderCompilationOutput shader_out = sc.Compile(std::string(vertex_shader), MR::ShaderCompiler::ST_Vertex, vshader->GetGPUHandle());
    for(size_t i = 0; i < shader_out.GetMessagesNum(); ++i){
        MR::Log::LogString(shader_out.GetMessage(i).GetText(), MR_LOG_LEVEL_INFO);
    }
    if(shader_out.Good() == false) {
        delete vshader;
        MR::Log::LogString("Failed ShaderBuilder::SimpleMake(..., ...). Vertex shader compilation failed.", MR_LOG_LEVEL_WARNING);
        return nullptr;
    }

    MR::IShader* fshader = new MR::Shader(glCreateShader(GL_FRAGMENT_SHADER), (MR::IShader::Type)GL_FRAGMENT_SHADER);
    shader_out = sc.Compile(std::string(fragment_shader), MR::ShaderCompiler::ST_Fragment, fshader->GetGPUHandle());
    for(size_t i = 0; i < shader_out.GetMessagesNum(); ++i){
        MR::Log::LogString(shader_out.GetMessage(i).GetText(), MR_LOG_LEVEL_INFO);
    }
    if(shader_out.Good() == false) {
        delete vshader;
        delete fshader;
        MR::Log::LogString("Failed ShaderBuilder::SimpleMake(..., ...). Fragment shader compilation failed.", MR_LOG_LEVEL_WARNING);
        return nullptr;
    }

    unsigned int handles[2] {vshader->GetGPUHandle(), fshader->GetGPUHandle()};
    IShaderProgram* prog = new MR::ShaderProgram(glCreateProgram(), MR::IShaderProgram::Features());
    shader_out = sc.Link(MR::StaticArray<unsigned int>(&handles[0], 2, false), prog->GetGPUHandle());
    for(size_t i = 0; i < shader_out.GetMessagesNum(); ++i){
        MR::Log::LogString(shader_out.GetMessage(i).GetText(), MR_LOG_LEVEL_INFO);
    }
    if(shader_out.Good() == false) {
        delete vshader;
        delete fshader;
        delete prog;
        MR::Log::LogString("Failed ShaderBuilder::SimpleMake(..., ...). Shader program link failed.", MR_LOG_LEVEL_WARNING);
        return nullptr;
    }

    return prog;
}

}
