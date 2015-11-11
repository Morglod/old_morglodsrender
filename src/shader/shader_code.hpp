#pragma once
#include <string>

/// RUN 'build_shader_resources.py' to update this

namespace mr {

//BEGIN FILE TO CHAR "common_shader.glsl"

const char* common_shader_glsl = "#version 400\n\n// Replaced by ShaderMaterialGenerator\n#define MR_VERTEX_SHADER {_MR_IS_VERTEX_SHADER_HERE}\n#define MR_FRAGMENT_SHADER {_MR_IS_FRAGMENT_SHADER_HERE}\n#define MR_CUSTOM_FUNC {_MR_IS_CUSTOM_FUNC_HERE}\n\nstruct MR_SysUniformData {\n	float time;\n	mat4 viewMat;\n	mat4 projMat;\n};\n\nstruct MR_DrawUniformData {\n    mat4 modelMat;\n};\n\nuniform MR_SysUniform {\n	MR_SysUniformData mr_sys;\n};\n\nuniform MR_DrawUniform {\n    MR_DrawUniformData mr_draw;\n};\n\n// Replaced by ShaderMaterialGenerator\n{_MR_MATERIAL_HERE}\n\n#if MR_VERTEX_SHADER == 1\n\n// Replaced by ShaderMaterialGenerator\n/*\nmr_attrib_*, where * is attribute index\nExample:\nin vec3 mr_attrib_0;\nin vec2 mr_attrib_1;\n*/\n{_MR_VERTEX_IN_HERE}\n\n#if MR_CUSTOM_FUNC == 0\nvec4 CustomMain() {\n    return (mr_sys.projMat * mr_sys.viewMat * mr_draw.modelMat) * vec4(mr_attrib_0, 1.0);\n}\n#else\n{_MR_CUSTOM_FUNC_HERE}\n#endif\n\nvoid main() {\n    vec4 pos;\n    pos = CustomMain(pos);\n    gl_Position = pos;\n}\n\n#elif MR_FRAGMENT_SHADER == 1\n\n#if MR_CUSTOM_FUNC == 0\nvec4 CustomMain() {\n    return vec4((sin(mr_sys.time) + 1.0) * 0.5,0,0,1);\n}\n#else\n{_MR_CUSTOM_FUNC_HERE}\n#endif\n\nvoid main() {\n    vec4 color;\n    color = CustomMain();\n    gl_FragColor = color;\n}\n\n#endif\n";

//END FILE TO CHAR

std::string common_shader_glsl_s = common_shader_glsl;

}
