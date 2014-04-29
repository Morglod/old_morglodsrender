#include "Light.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <glm/gtx/transform.hpp>

const char * depth_vert =
"#version 120"
"void main(void){"
"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"}";

const char * depth_frag =
"#version 120"
"void main(void){"
"gl_FragColor = vec4(gl_FragCoord.z);"
"}";

const char * shadow_vert =
"#version 120"
"uniform mat4 lightMatrix;"
"uniform vec3 lightPos;"
"uniform vec3 lightDir;"
"varying vec4 lpos;"
"varying vec3 normal;"
"varying vec3 light_vec;"
"varying vec3 light_dir;"
"void main(void){"
"	vec4 vpos = gl_ModelViewMatrix * gl_Vertex;"
"	lpos = lightMatrix * vpos;"
"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"	light_vec = vpos.xyz - lightPos;"
"	light_dir = gl_NormalMatrix * lightDir;"
"	normal = gl_NormalMatrix * gl_Normal;"
"	gl_FrontColor = gl_Color;"
"}";

const char * shadow_color_frag =
"#version 120"
"uniform sampler2D shadowMap;"
"varying vec4 lpos;"
"varying vec3 normal;"
"varying vec3 light_vec;"
"varying vec3 light_dir;"
"const float inner_angle = 0.809017;"
"const float outer_angle = 0.707107;"
"void main (void){"
"	vec3 smcoord = lpos.xyz / lpos.w;"
"	float shadow = float(smcoord.z <= texture2D(shadowMap, smcoord.xy).x);"
"	vec3 lvec = normalize(light_vec);"
"	float diffuse = max(dot(-lvec, normalize(normal)), 0.0);"
"	float angle = dot(lvec, normalize(light_dir));"
"	float spot = clamp((angle - outer_angle) / (inner_angle - outer_angle), 0.0, 1.0);"
"	gl_FragColor = vec4(gl_Color.xyz * diffuse * shadow * spot, 1.0);"
"}";

const char * shadow_depth_frag =
"#version 120"
"uniform sampler2DShadow shadowMap;"
"varying vec4 lpos;"
"varying vec3 normal;"
"varying vec3 light_vec;"
"varying vec3 light_dir;"
"const float inner_angle = 0.809017;"
"const float outer_angle = 0.707107;"
"void main (void){"
"	vec3 smcoord = lpos.xyz / lpos.w;"
"	float shadow = shadow2D(shadowMap, smcoord).x;"
"	vec3 lvec = normalize(light_vec);"
"	float diffuse = max(dot(-lvec, normalize(normal)), 0.0);"
"	float angle = dot(lvec, normalize(light_dir));"
"	float spot = clamp((angle - outer_angle) / (inner_angle - outer_angle), 0.0, 1.0);"
"	gl_FragColor = vec4(gl_Color.xyz * diffuse * shadow * spot, 1.0);"
"}";

namespace MR{

/*ShadowMap::ShadowMap(LightSource* light, const int& width, const int& height, const TextureBits& tbits) :
    _parentLight(light), _gl_texture(0), _gl_framebuffer(0), _w(width), _h(height) {
    glGenTextures(1, &_gl_texture);
    glBindTexture(GL_TEXTURE_2D, _gl_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if(tbits == TextureBits::tb16) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    else if(tbits == TextureBits::tb24) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    else if(tbits == TextureBits::tb32) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &_gl_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _gl_framebuffer);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _gl_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}*/

void ShadowMapDepth::BeginCapture(IRenderSystem* context, ILightSource* light){
    _captured_from = light;

    glViewport(0, 0, _w, _h);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _gl_framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0, 500.0);

	SetProjection(glm::perspective(90.0f, 1.0f, 30.0f, 300.0f));
	SetView(glm::lookAt(light->GetPos(), light->GetPos()+light->GetDir(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void ShadowMapDepth::EndCapture(IRenderSystem* context, ILightSource* light){
    glDisable(GL_POLYGON_OFFSET_FILL);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void ShadowMapDepth::SetProjection(const glm::mat4& p){
    _projection = p;
}

void ShadowMapDepth::SetView(const glm::mat4& v){
    _view = v;
}

ShadowMapDepth::ShadowMapDepth(ILightSource* light, const int& width, const int& height) : Object(), _captured_from(light), _w(width), _h(height) {
    //depth texture
	glGenTextures(1, &_gl_texture);
	glBindTexture(GL_TEXTURE_2D, _gl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _w, _h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//framebuffer
	glGenFramebuffersEXT(1, &_gl_framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _gl_framebuffer);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, _gl_texture, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

ShadowMapDepth::~ShadowMapDepth(){
}

void ShadowMapColor::BeginCapture(IRenderSystem* context, ILightSource* light){
    _captured_from = light;

    glViewport(0, 0, _w, _h);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _gl_framebuffer);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0, 500.0);

	SetProjection(glm::perspective(90.0f, 1.0f, 30.0f, 300.0f));
	SetView(glm::lookAt(light->GetPos(), light->GetPos()+light->GetDir(), glm::vec3(0.0f, 1.0f, 0.0f)));

    light->GetManager()->GetDepth()->Use(context);
}

void ShadowMapColor::EndCapture(IRenderSystem* context, ILightSource* light){
    glUseProgramObjectARB(0);
    glDisable(GL_POLYGON_OFFSET_FILL);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void ShadowMapColor::SetProjection(const glm::mat4& p){
    _projection = p;
}

void ShadowMapColor::SetView(const glm::mat4& v){
    _view = v;
}

ShadowMapColor::ShadowMapColor(ILightSource* light, const int& width, const int& height) : Object(), _captured_from(light), _w(width), _h(height) {
    //texture
    glGenTextures(1, &_gl_texture);
	glBindTexture(GL_TEXTURE_2D, _gl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, _w, _h, 0, GL_RGBA, GL_UNSIGNED_SHORT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	//render depth buffer
	glGenRenderbuffersEXT(1, &_gl_renderbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _gl_renderbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, _w, _h);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	//framebuffer
	glGenFramebuffersEXT(1, &_gl_framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _gl_framebuffer);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, _gl_texture, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _gl_renderbuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

ShadowMapColor::~ShadowMapColor(){
}

void LightSource::SetShadowMap(IShadowMap* map){
    _captured = map;
}

glm::vec3 LightSource::GetPos(){
    return _pos;
}

glm::vec3 LightSource::GetDir(){
    return _dir;
}

LightSource::~LightSource(){
}

LightsList LightsList::MakeList(const glm::vec3& point, const float& r, ILightSource** listOfPtrs, const size_t& listSize){
    std::vector<ILightSource*> l;
    for(size_t i = 0; i < listSize; ++i){
        if( glm::distance(listOfPtrs[i]->GetPos(), point) <= (r+listOfPtrs[i]->GetRadius()) ) l.push_back(listOfPtrs[i]);
    }
    return LightsList(l);
}

LightsList::LightsList(const std::vector<ILightSource*>& l) : _lights(l) {
}

LightManager::LightManager(ShaderManager* sm){
    _shadow_color_map_unit = new int(0);
    _shadow_depth_map_unit = new int(0);
    _shadow_color_mat_light = new glm::mat4(1.0f);
    _shadow_depth_mat_light = new glm::mat4(1.0f);
    _shadow_color_vec3_light_pos = new glm::vec3(0.0f, 0.0f, 0.0f);
    _shadow_depth_vec3_light_pos = new glm::vec3(0.0f, 0.0f, 0.0f);
    _shadow_color_vec3_light_dir = new glm::vec3(0.0f, 0.0f, 0.0f);
    _shadow_depth_vec3_light_dir = new glm::vec3(0.0f, 0.0f, 0.0f);

    _depth = new MR::Shader(sm, "ShadowDrawDepth", "FromMem");
    _depth->AttachSubShader(new MR::SubShader(depth_frag, MR::ISubShader::Type::Fragment));
    _depth->AttachSubShader(new MR::SubShader(depth_vert, MR::ISubShader::Type::Vertex));
    _depth->Link();

    _shadow_color = new MR::Shader(sm, "ShadowColor", "FromMem");
    _shadow_color->AttachSubShader(new MR::SubShader(shadow_color_frag, MR::ISubShader::Type::Fragment));
    _shadow_color->AttachSubShader(new MR::SubShader(shadow_vert, MR::ISubShader::Type::Vertex));
    _shadow_color->Link();
    _shadow_color->CreateUniform("shadowMap", IShaderUniform::Types::Int, _shadow_color_map_unit);
    _shadow_color->CreateUniform("lightMatrix", IShaderUniform::Types::Mat4, _shadow_color_mat_light);
    _shadow_color->CreateUniform("lightPos", IShaderUniform::Types::Vec3, _shadow_color_vec3_light_pos);
    _shadow_color->CreateUniform("lightDir", IShaderUniform::Types::Vec3, _shadow_color_vec3_light_dir);

    _shadow_depth = new MR::Shader(sm, "ShadowDepth", "FromMem");
    _shadow_depth->AttachSubShader(new MR::SubShader(shadow_depth_frag, MR::ISubShader::Type::Fragment));
    _shadow_depth->AttachSubShader(new MR::SubShader(shadow_vert, MR::ISubShader::Type::Vertex));
    _shadow_depth->Link();
    _shadow_depth->CreateUniform("shadowMap", IShaderUniform::Types::Int, _shadow_depth_map_unit);
    _shadow_depth->CreateUniform("lightMatrix", IShaderUniform::Types::Mat4, _shadow_depth_mat_light);
    _shadow_depth->CreateUniform("lightPos", IShaderUniform::Types::Vec3, _shadow_depth_vec3_light_pos);
    _shadow_depth->CreateUniform("lightDir", IShaderUniform::Types::Vec3, _shadow_depth_vec3_light_dir);
}

LightManager::~LightManager(){
    delete _shadow_color;
    delete _shadow_depth;
    delete _shadow_color_map_unit;
    delete _shadow_depth_map_unit;
    delete _shadow_color_mat_light;
    delete _shadow_depth_mat_light;
    delete _shadow_color_vec3_light_pos;
    delete _shadow_depth_vec3_light_pos;
    delete _shadow_color_vec3_light_dir;
    delete _shadow_depth_vec3_light_dir;
}

ILightSource* LightSource::CreatePointLight(const glm::vec3& pos, const glm::vec4& color, const float& radius) {
    LightSource* lsrc = new LightSource();
    lsrc->_pos = pos;
    lsrc->_color = color;
    lsrc->_radius = radius;
    lsrc->_type = Type::PointLight;
    return lsrc;
}

ILightSource* LightSource::CreateSpotLight(const glm::vec3& pos, const glm::vec4& color, const float& radius) {
    LightSource* lsrc = new LightSource();
    lsrc->_pos = pos;
    lsrc->_color = color;
    lsrc->_radius = radius;
    lsrc->_type = Type::PointLight;
    return lsrc;
}

}
