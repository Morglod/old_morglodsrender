#include "Texture.hpp"

using namespace MR;

void MR::TextureSettings::SetLodBias(const float& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_LOD_BIAS, v);
    _GL_TEXTURE_LOD_BIAS = v;
}

void MR::TextureSettings::SetBorderColor(float* rgba) {
    glSamplerParameterfv(_sampler, GL_TEXTURE_BORDER_COLOR, rgba);
    _GL_TEXTURE_BORDER_COLOR[0] = rgba[0];
    _GL_TEXTURE_BORDER_COLOR[1] = rgba[1];
    _GL_TEXTURE_BORDER_COLOR[2] = rgba[2];
    _GL_TEXTURE_BORDER_COLOR[3] = rgba[3];
}

void MR::TextureSettings::SetBorderColor(const float& r, const float& g, const float& b, const float& a) {
    _GL_TEXTURE_BORDER_COLOR[0] = r;
    _GL_TEXTURE_BORDER_COLOR[1] = g;
    _GL_TEXTURE_BORDER_COLOR[2] = b;
    _GL_TEXTURE_BORDER_COLOR[3] = a;
    glSamplerParameterfv(_sampler, GL_TEXTURE_BORDER_COLOR, _GL_TEXTURE_BORDER_COLOR);
}

void MR::TextureSettings::SetMagFilter(const int& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_MAG_FILTER, v);
    _GL_TEXTURE_MAG_FILTER = v;
}

void MR::TextureSettings::SetMinFilter(const int& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_MIN_FILTER, v);
    _GL_TEXTURE_MIN_FILTER = v;
}

void MR::TextureSettings::SetMinLod(const int& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_MIN_LOD, v);
    _GL_TEXTURE_MIN_LOD = v;
}

void MR::TextureSettings::SetMaxLod(const int& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_MAX_LOD, v);
    _GL_TEXTURE_MAX_LOD = v;
}

void MR::TextureSettings::SetWrapS(const int& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_S, v);
    _GL_TEXTURE_WRAP_S = v;
}

void MR::TextureSettings::SetWrapR(const int& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_R, v);
    _GL_TEXTURE_WRAP_R = v;
}

void MR::TextureSettings::SetWrapT(const int& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_WRAP_T, v);
    _GL_TEXTURE_WRAP_T = v;
}

void MR::TextureSettings::SetCompareMode(const int& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_COMPARE_MODE, v);
    _GL_TEXTURE_COMPARE_MODE = v;
}

void MR::TextureSettings::SetCompareFunc(const int& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_COMPARE_FUNC, v);
    _GL_TEXTURE_COMPARE_FUNC = v;
}

MR::TextureSettings* MR::TextureSettings::Copy(){
    TextureSettings* ts = new TextureSettings();
    ts->SetLodBias(_GL_TEXTURE_LOD_BIAS);
    ts->SetBorderColor(&_GL_TEXTURE_BORDER_COLOR[0]);
    ts->SetMagFilter(_GL_TEXTURE_MAG_FILTER);
    ts->SetMinFilter(_GL_TEXTURE_MIN_FILTER);
    ts->SetMinLod(_GL_TEXTURE_MIN_LOD);
    ts->SetMaxLod(_GL_TEXTURE_MAX_LOD);
    ts->SetWrapS(_GL_TEXTURE_WRAP_S);
    ts->SetWrapT(_GL_TEXTURE_WRAP_T);
    ts->SetWrapR(_GL_TEXTURE_WRAP_R);
    ts->SetCompareMode(_GL_TEXTURE_COMPARE_MODE);
    ts->SetCompareFunc(_GL_TEXTURE_COMPARE_FUNC);
    return ts;
}

MR::TextureSettings::TextureSettings() :
    _sampler(0), _GL_TEXTURE_LOD_BIAS(MR_DEFAULT_TEXTURE_SETTINGS_LOD_BIAS),
    _GL_TEXTURE_BORDER_COLOR{MR_DEFAULT_TEXTURE_SETTINGS_BORDER_COLOR},
    _GL_TEXTURE_MAG_FILTER(MR_DEFAULT_TEXTURE_SETTINGS_MAG_FILTER), _GL_TEXTURE_MIN_FILTER(MR_DEFAULT_TEXTURE_SETTINGS_MIN_FILTER),
    _GL_TEXTURE_MIN_LOD(MR_DEFAULT_TEXTURE_SETTINGS_MIN_LOD), _GL_TEXTURE_MAX_LOD(MR_DEFAULT_TEXTURE_SETTINGS_MAX_LOD),
    _GL_TEXTURE_WRAP_S(MR_DEFAULT_TEXTURE_SETTINGS_WRAP_S), _GL_TEXTURE_WRAP_T(MR_DEFAULT_TEXTURE_SETTINGS_WRAP_T),
    _GL_TEXTURE_WRAP_R(MR_DEFAULT_TEXTURE_SETTINGS_WRAP_R), _GL_TEXTURE_COMPARE_MODE(MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_MODE), _GL_TEXTURE_COMPARE_FUNC(MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_FUNC) {
    glGenSamplers(1, &_sampler);
}

MR::TextureSettings::~TextureSettings() {
    glDeleteSamplers(1, &_sampler);
}

void MR::Texture::GetData(const int& mipMapLevel, const GLenum& format, const GLenum& type, void* dstBuffer) {
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glGetTexImage(GL_TEXTURE_2D, mipMapLevel, format, type, dstBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MR::Texture::SetData(const int& mipMapLevel, const int& internalFormat, const int& width, const int& height, const GLenum& format, const GLenum& type, void* data){
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glTexImage2D(GL_TEXTURE_2D,
        mipMapLevel,
        internalFormat,
        width,
        height,
        0,
        format,
        type,
        data);
    glBindTexture(GL_TEXTURE_2D, 0);
    OnDataChanged(this, mipMapLevel, internalFormat, width, height, format, type, data);
}

void MR::Texture::UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const GLenum& format, const GLenum& type, void* data) {
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glTexSubImage2D(GL_TEXTURE_2D,
        mipMapLevel,
        xOffset,
        yOffset,
        width,
        height,
        format,
        type,
        data);
    glBindTexture(GL_TEXTURE_2D, 0);
    OnDataUpdated(this, mipMapLevel, xOffset, yOffset, width, height, format, type, data);
}

void MR::Texture::ResetInfo() {
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (int*)(&gl_width));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (int*)(&gl_height));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, (int*)(&gl_depth));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, (int*)(&gl_internal_format));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, (int*)(&gl_red_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, (int*)(&gl_green_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, (int*)(&gl_blue_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH_SIZE, (int*)(&gl_depth_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, (int*)(&gl_alpha_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, (int*)(&gl_mem_compressed_img_size));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, (int*)(&gl_compressed));
    gl_mem_image_size = ((gl_red_bits_num+gl_green_bits_num+gl_blue_bits_num+gl_depth_bits_num+gl_alpha_bits_num)/8)*gl_width*gl_height;
    glBindTexture(GL_TEXTURE_2D, 0);
    OnInfoReset(this, NULL);
}

bool MR::Texture::Load(){
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") loading", MR_LOG_LEVEL_INFO);
    if(this->_source != "") this->gl_texture = MR::LoadGLTexture(this->_source);
    else if(this->_resource_manager->GetDebugMessagesState()){
        MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
        this->_loaded = false;
        return false;
    }
    if(this->gl_texture == 0){
        MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") loading failed. GL_TEXTURE is null", MR_LOG_LEVEL_ERROR);
        this->_loaded = false;
        return false;
    }

    this->_loaded = true;

    ResetInfo();

    return true;
}

void MR::Texture::UnLoad(){
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") unloading", MR_LOG_LEVEL_INFO);
    if(_res_free_state) {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") -> ResFreeState is on, deleting data", MR_LOG_LEVEL_INFO);
        glDeleteTextures(1, &this->gl_texture);
    } else{
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") -> ResFreeState is off", MR_LOG_LEVEL_INFO);
    }
    this->gl_texture = 0;
}

void MR::Texture::CreateOpenGLTexture(GLuint* tex_dst, GLint internal_format, GLsizei Width, GLsizei Height, GLint format, GLenum type){
    glGenTextures(1, tex_dst);
    glBindTexture(GL_TEXTURE_2D, *tex_dst);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, Width, Height, 0, format, type, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

MR::Texture::Texture(MR::ResourceManager* manager, std::string name, std::string source) : Resource(manager, name, source), _settings(nullptr) {
}

MR::Texture::~Texture(){
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") deleting", MR_LOG_LEVEL_INFO);
    UnLoad();
}

Resource* TextureManager::Create(std::string name, std::string source){
    if(this->_debugMessages) MR::Log::LogString("TextureManager "+name+" ("+source+") creating", MR_LOG_LEVEL_INFO);
    Texture * t = new Texture(this, name, source);
    this->_resources.push_back(t);
    return t;
}

Resource* TextureManager::Create(std::string name, unsigned int gl_texture){
    if(this->_debugMessages) MR::Log::LogString("TextureManager "+name+" (gl_texture) creating", MR_LOG_LEVEL_INFO);
    Texture * t = new Texture(this, name, "");
    t->SetGLTexture(gl_texture);
    this->_resources.push_back(t);
    return t;
}

/*
Resource** TextureManager::Need(std::string* sources, unsigned int num){
    Resource** finded_list = new Resource*[num];
    if(this->debugMessages) MR::Log::LogString("TextureManager List of resources needed", MR_LOG_LEVEL_INFO);
    for(std::vector<Texture*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        for(unsigned int i = 0; i < num; ++i){
            if( !(finded_list[i]) ){
                if( (*it)->GetSource() == sources[i] ){
                    if( !(*it)->IsLoaded() ) (*it)->Load();
                    finded_list[i] = (*it);
                }
                else {
                    finded_list[i] = nullptr;
                }
            }
        }
    }

    for(unsigned int i = 0; i < num; ++i){
        if( !(finded_list[i]) ){
            finded_list[i] = this->CreateAndLoad("AutoLoaded_"+sources[i], sources[i]);
        }
    }

    return &finded_list[0];
}
*/
