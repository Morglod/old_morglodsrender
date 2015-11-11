#version 400

// Replaced by ShaderMaterialGenerator
#define MR_VERTEX_SHADER {_MR_IS_VERTEX_SHADER_HERE}
#define MR_FRAGMENT_SHADER {_MR_IS_FRAGMENT_SHADER_HERE}
#define MR_CUSTOM_FUNC {_MR_IS_CUSTOM_FUNC_HERE}

struct MR_SysUniformData {
	float time;
	mat4 viewMat;
	mat4 projMat;
};

struct MR_DrawUniformData {
    mat4 modelMat;
};

uniform MR_SysUniform {
	MR_SysUniformData mr_sys;
};

uniform MR_DrawUniform {
    MR_DrawUniformData mr_draw;
};

// Replaced by ShaderMaterialGenerator
{_MR_MATERIAL_HERE}

#if MR_VERTEX_SHADER == 1

// Replaced by ShaderMaterialGenerator
/*
mr_attrib_*, where * is attribute index
Example:
in vec3 mr_attrib_0;
in vec2 mr_attrib_1;
*/
{_MR_VERTEX_IN_HERE}

#if MR_CUSTOM_FUNC == 0
vec4 CustomMain() {
    return (mr_sys.projMat * mr_sys.viewMat * mr_draw.modelMat) * vec4(mr_attrib_0, 1.0);
}
#else
{_MR_CUSTOM_FUNC_HERE}
#endif

void main() {
    vec4 pos;
    pos = CustomMain(pos);
    gl_Position = pos;
}

#elif MR_FRAGMENT_SHADER == 1

#if MR_CUSTOM_FUNC == 0
vec4 CustomMain() {
    return vec4((sin(mr_sys.time) + 1.0) * 0.5,0,0,1);
}
#else
{_MR_CUSTOM_FUNC_HERE}
#endif

void main() {
    vec4 color;
    color = CustomMain();
    gl_FragColor = color;
}

#endif
