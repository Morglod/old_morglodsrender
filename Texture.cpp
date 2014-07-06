#include "Texture.hpp"
#include "RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "RenderTarget.hpp"
#include "GL/Context.hpp"
#include "Utils/Singleton.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#   include <SOIL.h>
#endif

using namespace MR;

unsigned int MR::LoadGLTexture(const std::string& file, const unsigned int & t, const GLTextureLoadFormat& format, const GLTextureLoadFlags& flags){
    //MR::GL::GetCurrent()->Lock();
    unsigned int i = SOIL_load_OGL_texture(file.c_str(),(int)format, t,(unsigned int)flags);
    //MR::GL::GetCurrent()->Lock();
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLTexture(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLTexture(\""+file+"\"). " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int MR::LoadGLCubemap(const std::string& x_pos_file, const std::string& x_neg_file, const std::string& y_pos_file, const std::string& y_neg_file, const std::string& z_pos_file, const std::string& z_neg_file,
                               const unsigned int & t, const GLTextureLoadFormat& format, const GLTextureLoadFlags& flags){
    //MR::GL::GetCurrent()->Lock();
    unsigned int i = SOIL_load_OGL_cubemap(x_pos_file.c_str(), x_neg_file.c_str(), y_pos_file.c_str(), y_neg_file.c_str(), z_pos_file.c_str(), z_neg_file.c_str(), (int)format, t,(unsigned int)flags);
    //MR::GL::GetCurrent()->Lock();
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLCubemap. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLCubemap. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int MR::LoadGLCubemap(const std::string& file, const char faceOrder[6], const unsigned int & t, const GLTextureLoadFormat& format, const GLTextureLoadFlags& flags){
    //MR::GL::GetCurrent()->Lock();
    unsigned int i = SOIL_load_OGL_single_cubemap(file.c_str(), faceOrder, (int)format, t, (unsigned int)flags);
    //MR::GL::GetCurrent()->Lock();
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLCubemap(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLCubemap(\""+file+"\"). " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int MR::LoadGLHDR(const std::string& file, const int& rescale_to_max, const unsigned int & t, const GLTextureHDRFake& hdrFake, const GLTextureLoadFlags& flags){
    //MR::GL::GetCurrent()->Lock();
    unsigned int i = SOIL_load_OGL_HDR_texture(file.c_str(), (int)hdrFake, rescale_to_max, t, (unsigned int)flags);
    //MR::GL::GetCurrent()->Lock();
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLHDR(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLHDR(\""+file+"\"). " + lastResult);
        }
    }
    return i;
}

unsigned int MR::LoadGLTexture(const unsigned char *const buffer, const unsigned int& buffer_length, const unsigned int & t, const GLTextureLoadFormat& format, const GLTextureLoadFlags& flags){
    //MR::GL::GetCurrent()->Lock();
    unsigned int i = SOIL_load_OGL_texture_from_memory(buffer, buffer_length, (int)format, t,(unsigned int)flags);
    //MR::GL::GetCurrent()->Lock();
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLTexture. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLTexture. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int MR::LoadGLCubemap(const unsigned char *const x_pos_buffer, const unsigned int& x_pos_buffer_length,
                               const unsigned char *const x_neg_buffer, const unsigned int& x_neg_buffer_length,
                               const unsigned char *const y_pos_buffer, const unsigned int& y_pos_buffer_length,
                               const unsigned char *const y_neg_buffer, const unsigned int& y_neg_buffer_length,
                               const unsigned char *const z_pos_buffer, const unsigned int& z_pos_buffer_length,
                               const unsigned char *const z_neg_buffer, const unsigned int& z_neg_buffer_length,
                               const unsigned int & t,
                               const GLTextureLoadFormat& format, const GLTextureLoadFlags& flags){
    //MR::GL::GetCurrent()->Lock();
    unsigned int i = SOIL_load_OGL_cubemap_from_memory(x_pos_buffer, x_pos_buffer_length,
                                                       x_neg_buffer, x_neg_buffer_length,
                                                       y_pos_buffer, y_pos_buffer_length,
                                                       y_neg_buffer, y_neg_buffer_length,
                                                       z_pos_buffer, z_pos_buffer_length,
                                                       z_neg_buffer, z_neg_buffer_length, (int)format, t,(unsigned int)flags);
    //MR::GL::GetCurrent()->Lock();
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLCubemap. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLCubemap. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int MR::LoadGLCubemap(const unsigned char *const buffer, const unsigned int& buffer_length, const char faceOrder[6], const unsigned int & t, const GLTextureLoadFormat& format, const GLTextureLoadFlags& flags){
    //MR::GL::GetCurrent()->Lock();
    unsigned int i = SOIL_load_OGL_single_cubemap_from_memory(buffer, buffer_length, faceOrder, (int)format, t, (unsigned int)flags);
    //MR::GL::GetCurrent()->Lock();
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLCubemap. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLCubemap. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

bool MR::SaveScreenshot(const std::string& file, const GLImageSaveType& image_type, const int& x, const int& y, const int& width, const int& height){
    //MR::GL::GetCurrent()->Lock();
    bool state = SOIL_save_screenshot(file.c_str(), (int)image_type, x, y, width, height);
    //MR::GL::GetCurrent()->Lock();
    if(state == false){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed SaveScreenshot(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed SaveScreenshot(\""+file+"\"). " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return state;
}

MR::Mutex __LOAD_IMAGE_MUTEX;

unsigned char* MR::LoadImage(const std::string& file, int* width, int* height, GLTextureLoadFormat* format, const GLTextureLoadFormat& force_format){
    __LOAD_IMAGE_MUTEX.Lock();
    unsigned char* ptr = SOIL_load_image(file.c_str(), width, height, (int*)format, (int)force_format);
    __LOAD_IMAGE_MUTEX.UnLock();
    if(ptr == NULL){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadImage(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadImage(\""+file+"\"). " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return ptr;
}

/** CLASS IMPLEMENTION **/

void MR::TextureSettings::SetLodBias(const float& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameterf(_sampler, GL_TEXTURE_LOD_BIAS, v);
    //MR::GL::GetCurrent()->Lock();
    _lod_bias = v;
    OnLodBiasChanged(this, v);
}

void MR::TextureSettings::SetBorderColor(float* rgba) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameterfv(_sampler, GL_TEXTURE_BORDER_COLOR, rgba);
    //MR::GL::GetCurrent()->Lock();
    _border_color[0] = rgba[0];
    _border_color[1] = rgba[1];
    _border_color[2] = rgba[2];
    _border_color[3] = rgba[3];
    OnBorderColorChanged(this, _border_color);
}

void MR::TextureSettings::SetBorderColor(const float& r, const float& g, const float& b, const float& a) {
    _border_color[0] = r;
    _border_color[1] = g;
    _border_color[2] = b;
    _border_color[3] = a;
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameterfv(_sampler, GL_TEXTURE_BORDER_COLOR, _border_color);
    //MR::GL::GetCurrent()->Lock();
    OnBorderColorChanged(this, _border_color);
}

void MR::TextureSettings::SetMagFilter(const MR::TextureSettings::MagFilter& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, (int)v);
    //MR::GL::GetCurrent()->Lock();
    _mag_filter = v;
    OnMagFilterChanged(this, v);
}

void MR::TextureSettings::SetMinFilter(const MR::TextureSettings::MinFilter& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, (int)v);
    //MR::GL::GetCurrent()->Lock();
    _min_filter = v;
    OnMinFilterChanged(this, v);
}

void MR::TextureSettings::SetMinLod(const int& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameteri(_sampler, GL_TEXTURE_MIN_LOD, v);
    //MR::GL::GetCurrent()->Lock();
    _min_lod = v;
    OnMinLodChanged(this, v);
}

void MR::TextureSettings::SetMaxLod(const int& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameteri(_sampler, GL_TEXTURE_MAX_LOD, v);
    //MR::GL::GetCurrent()->Lock();
    _max_lod = v;
    OnMaxLodChanged(this, v);
}

void MR::TextureSettings::SetWrapS(const Wrap& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, (int)v);
    //MR::GL::GetCurrent()->Lock();
    _wrap_s = v;
    OnWrapSChanged(this, v);
}

void MR::TextureSettings::SetWrapR(const Wrap& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, (int)v);
    //MR::GL::GetCurrent()->Lock();
    _wrap_r = v;
    OnWrapRChanged(this, v);
}

void MR::TextureSettings::SetWrapT(const Wrap& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, (int)v);
    //MR::GL::GetCurrent()->Lock();
    _wrap_t = v;
    OnWrapTChanged(this, v);
}

void MR::TextureSettings::SetCompareMode(const CompareMode& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameteri(_sampler, GL_TEXTURE_COMPARE_MODE, (int)v);
    //MR::GL::GetCurrent()->Lock();
    _compare_mode = v;
    OnCompareModeChanged(this, v);
}

void MR::TextureSettings::SetCompareFunc(const CompareFunc& v) {
    //MR::GL::GetCurrent()->Lock();
    glSamplerParameterf(_sampler, GL_TEXTURE_COMPARE_FUNC, (int)v);
    //MR::GL::GetCurrent()->Lock();
    _compare_func = v;
    OnCompareFuncChanged(this, v);
}

ITextureSettings* MR::TextureSettings::Copy(){
    TextureSettings* ts = new TextureSettings();
    ts->SetLodBias(_lod_bias);
    ts->SetBorderColor(&_border_color[0]);
    ts->SetMagFilter(_mag_filter);
    ts->SetMinFilter(_min_filter);
    ts->SetMinLod(_min_lod);
    ts->SetMaxLod(_max_lod);
    ts->SetWrapS(_wrap_s);
    ts->SetWrapT(_wrap_t);
    ts->SetWrapR(_wrap_r);
    ts->SetCompareMode(_compare_mode);
    ts->SetCompareFunc(_compare_func);
    return ts;
}

MR::TextureSettings::TextureSettings() :
    _sampler(0), _lod_bias(MR_DEFAULT_TEXTURE_SETTINGS_LOD_BIAS),
    _border_color{MR_DEFAULT_TEXTURE_SETTINGS_BORDER_COLOR},
    _min_filter((MinFilter)MR_DEFAULT_TEXTURE_SETTINGS_MIN_FILTER),
    _mag_filter((MagFilter)MR_DEFAULT_TEXTURE_SETTINGS_MAG_FILTER),
    _min_lod(MR_DEFAULT_TEXTURE_SETTINGS_MIN_LOD),
    _max_lod(MR_DEFAULT_TEXTURE_SETTINGS_MAX_LOD),
    _wrap_s((Wrap)MR_DEFAULT_TEXTURE_SETTINGS_WRAP_S),
    _wrap_r((Wrap)MR_DEFAULT_TEXTURE_SETTINGS_WRAP_R),
    _wrap_t((Wrap)MR_DEFAULT_TEXTURE_SETTINGS_WRAP_T),
     _compare_mode((CompareMode)MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_MODE),
     _compare_func((CompareFunc)MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_FUNC) {

    //MR::GL::GetCurrent()->Lock();
    glGenSamplers(1, &_sampler);
    glSamplerParameterf(_sampler, GL_TEXTURE_LOD_BIAS, _lod_bias);
    glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, (int)_mag_filter);
    glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, (int)_min_filter);
    glSamplerParameteri(_sampler, GL_TEXTURE_MIN_LOD, _min_lod);
    glSamplerParameteri(_sampler, GL_TEXTURE_MAX_LOD, _max_lod);
    glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, (int)_wrap_s);
    glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, (int)_wrap_r);
    glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, (int)_wrap_t);
    glSamplerParameteri(_sampler, GL_TEXTURE_COMPARE_MODE, (int)_compare_mode);
    glSamplerParameterf(_sampler, GL_TEXTURE_COMPARE_FUNC, (int)_compare_func);
    //MR::GL::GetCurrent()->Lock();
}

MR::TextureSettings::~TextureSettings() {
    if(MR::AnyRenderSystemAlive()) {
        //MR::GL::GetCurrent()->Lock();
        glDeleteSamplers(1, &_sampler);
        //MR::GL::GetCurrent()->Lock();
    }
}

void MR::Texture::GetData(const int& mipMapLevel, const MR::Texture::Format& format, const MR::Texture::Type& type, void* dstBuffer) {
    //MR::GL::GetCurrent()->Lock();
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glGetTexImage(GL_TEXTURE_2D, mipMapLevel, (int)format, (int)type, dstBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
    //MR::GL::GetCurrent()->Lock();
}

void MR::Texture::SetData(const int& mipMapLevel, const InternalFormat& internalFormat, const int& width, const int& height, const Format& format, const Type& type, void* data){
    //MR::GL::GetCurrent()->Lock();
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glTexImage2D(GL_TEXTURE_2D,
        mipMapLevel,
        (int)internalFormat,
        width,
        height,
        0,
        (int)format,
        (int)type,
        data);
    glBindTexture(GL_TEXTURE_2D, 0);
    //MR::GL::GetCurrent()->Lock();
    OnDataChanged(this, mipMapLevel, internalFormat, width, height, format, type, data);
}

void MR::Texture::UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const Format& format, const Type& type, void* data) {
    //MR::GL::GetCurrent()->Lock();
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glTexSubImage2D(GL_TEXTURE_2D,
        mipMapLevel,
        xOffset,
        yOffset,
        width,
        height,
        (int)format,
        (int)type,
        data);
    glBindTexture(GL_TEXTURE_2D, 0);
    //MR::GL::GetCurrent()->Lock();
    OnDataUpdated(this, mipMapLevel, xOffset, yOffset, width, height, format, type, data);
}

void MR::Texture::ResetInfo() {
    //MR::GL::GetCurrent()->Lock();
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
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, (int*)(&gl_compressed));
    gl_mem_image_size = ((gl_red_bits_num+gl_green_bits_num+gl_blue_bits_num+gl_depth_bits_num+gl_alpha_bits_num)/8)*gl_width*gl_height;
    glBindTexture(GL_TEXTURE_2D, 0);
    //MR::GL::GetCurrent()->Lock();
    OnInfoReset(this, NULL);
}

void MR::Texture::CreateOpenGLTexture(GLuint* TexDst, const InternalFormat& internalFormat, const int& Width, const int& Height, const Format& format, const Type& type){
    //MR::GL::GetCurrent()->Lock();
    glGenTextures(1, TexDst);
    glBindTexture(GL_TEXTURE_2D, *TexDst);
    glTexImage2D(GL_TEXTURE_2D, 0, (int)internalFormat, Width, Height, 0, (int)format, (int)type, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    //MR::GL::GetCurrent()->Lock();
}

void MR::Texture::CreateOpenGLTexture(const unsigned char *const data, const int& width, const int& height, const GLTextureLoadFormat& format, unsigned int * TexDst, const GLTextureLoadFlags& flags){
    unsigned int t = 0;
    if((*TexDst) != 0) t = *TexDst;
    //MR::GL::GetCurrent()->Lock();
    *TexDst = SOIL_create_OGL_texture(data, width, height, (int)format, t, (unsigned int)flags);
    //MR::GL::GetCurrent()->Lock();
    if((*TexDst) == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed CreateOpenGLTexture. " + lastResult);
        } else {
            MR::Log::LogString("Failed CreateOpenGLTexture. " + lastResult);
        }
    }
}

void MR::Texture::CreateOpenGLTexture(const unsigned char *const data, const char face_order[6], const int& width, const int& height, const GLTextureLoadFormat& format, unsigned int * TexDst, const GLTextureLoadFlags& flags){
    unsigned int t = 0;
    if((*TexDst) != 0) t = *TexDst;
    //MR::GL::GetCurrent()->Lock();
    *TexDst = SOIL_create_OGL_single_cubemap(data, width, height, (int)format, face_order, t, (unsigned int)flags);
    //MR::GL::GetCurrent()->Lock();
    if((*TexDst) == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed CreateOpenGLTexture. " + lastResult);
        } else {
            MR::Log::LogString("Failed CreateOpenGLTexture. " + lastResult);
        }
    }
}

MR::Texture::Texture(MR::ResourceManager* m, const std::string& name, const std::string& source) : Resource(m, name, source), _settings(nullptr) {
}

MR::Texture::Texture(MR::ResourceManager* m, const std::string& name, const std::string& source, const unsigned int & glTexture) : Resource(m, name, source), gl_texture(glTexture) {
}

MR::Texture::~Texture(){
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") deleting", MR_LOG_LEVEL_INFO);
    UnLoad();
}

bool MR::Texture::_Loading() {
    //if(_async_loading_handle.NoErrors()) if(!_async_loading_handle.End()) return false;

    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") cpu loading", MR_LOG_LEVEL_INFO);
    if(this->_source == "") {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
        return false;
    }

    if(_inTextureArray) {
        MR::GLTextureLoadFormat f;
        _async_data = MR::LoadImage(this->_source, (int*)&gl_width, (int*)&gl_height, &f, MR::GLTextureLoadFormat::Auto);
        if((gl_width == 0)||(gl_height == 0)) {
            MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") loading failed. BAD SIZES", MR_LOG_LEVEL_ERROR);

            return false;
        }

        _async_iform = 0;
        _async_form = 0;

        switch( (int)f ) {
        case (int)GLTextureLoadFormat::Luminous:
            _async_iform = GL_R8;
            _async_form = (int)Format::LUMINANCE;
            break;
        case (int)GLTextureLoadFormat::LuminousAplha:
            _async_iform = GL_RG8;
            _async_form = (int)Format::LUMINANCE_ALPHA;
            break;
        case (int)GLTextureLoadFormat::RGB:
            _async_iform = (int)InternalFormat::RGB8;
            _async_form = (int)Format::RGB;
            break;
        case (int)GLTextureLoadFormat::RGBA:
            _async_iform = (int)InternalFormat::RGBA8;
            _async_form = (int)Format::RGBA;
            break;
        default:
            _async_iform = (int)InternalFormat::RGB8;
            _async_form = (int)Format::RGB;
            break;
        }
    } else {
        MR::GLTextureLoadFormat f;
        _async_data = MR::LoadImage(this->_source, (int*)&gl_width, (int*)&gl_height, &f, MR::GLTextureLoadFormat::Auto);
        if((gl_width == 0)||(gl_height == 0)) {
            MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") loading failed. BAD SIZES", MR_LOG_LEVEL_ERROR);

            return false;
        }

        _async_iform = 0;
        _async_form = 0;
        ITexture::CompressionMode cmode = dynamic_cast<TextureManager*>(this->_resource_manager)->GetCompressionMode();

        switch( (int)f ) {
        case (int)GLTextureLoadFormat::Luminous:
            _async_iform = GL_R8;
            _async_form = (int)Format::LUMINANCE;
            if(cmode != ITexture::CompressionMode::NoCompression) _async_iform = GL_COMPRESSED_LUMINANCE;

            break;
        case (int)GLTextureLoadFormat::LuminousAplha:
            _async_iform = GL_RG8;
            _async_form = (int)Format::LUMINANCE_ALPHA;
            if(cmode != ITexture::CompressionMode::NoCompression) _async_iform = GL_COMPRESSED_LUMINANCE_ALPHA;

            break;
        case (int)GLTextureLoadFormat::RGB:
            _async_iform = (int)InternalFormat::RGB8;
            _async_form = (int)Format::RGB;
            if(cmode == ITexture::CompressionMode::Default) _async_iform = GL_COMPRESSED_RGB;
            if(cmode == ITexture::CompressionMode::S3TC) _async_iform = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            if(cmode == ITexture::CompressionMode::ETC2) _async_iform = GL_COMPRESSED_RGB8_ETC2;

            break;
        case (int)GLTextureLoadFormat::RGBA:
            _async_iform = (int)InternalFormat::RGBA8;
            _async_form = (int)Format::RGBA;
            if(cmode == ITexture::CompressionMode::Default) _async_iform = GL_COMPRESSED_RGBA;
            if(cmode == ITexture::CompressionMode::S3TC) _async_iform = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            if(cmode == ITexture::CompressionMode::ETC2) _async_iform = GL_COMPRESSED_RGBA8_ETC2_EAC;

            break;
        default:
            _async_iform = (int)InternalFormat::RGB8;
            _async_form = (int)Format::RGB;
            if(cmode == ITexture::CompressionMode::Default) _async_iform = GL_COMPRESSED_RGB;
            if(cmode == ITexture::CompressionMode::S3TC) _async_iform = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            if(cmode == ITexture::CompressionMode::ETC2) _async_iform = GL_COMPRESSED_RGB8_ETC2;

            break;
        }
    }

    if(_inTextureArray) {
        //MR::GL::GetCurrent()->Lock();
        _texArray = dynamic_cast<TextureManager*>(_resource_manager)->_StoreTexture(&_textureArrayIndex, _async_data, (InternalFormat)_async_iform, (Format)_async_form, Type::UNSIGNED_BYTE, gl_width, gl_height);
        //MR::GL::GetCurrent()->Lock();

        gl_texture = 0;
        return true;
    } else {
        //MR::GL::GetCurrent()->Lock();
        glGenTextures(1, &gl_texture);
        glBindTexture(GL_TEXTURE_2D, gl_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, _async_iform, gl_width, gl_height, 0, _async_form, (unsigned int) Type::UNSIGNED_BYTE, _async_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        //MR::GL::GetCurrent()->Lock();

        //_texArray = dynamic_cast<TextureManager*>(_resource_manager)->_StoreTexture(&_textureArrayIndex, _async_data, (InternalFormat)_async_iform, (Format)_async_form, Type::UNSIGNED_BYTE, gl_width, gl_height);

        return true;
    }

    if(_async_data) delete [] _async_data;

    ResetInfo();

    return true;
}

void MR::Texture::_UnLoading() {
    //if(_async_unloading_handle.NoErrors()) _async_unloading_handle.End();

    if(_res_free_state) {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") -> ResFreeState is on, deleting data", MR_LOG_LEVEL_INFO);
        if(MR::AnyRenderSystemAlive()) {
            //MR::GL::GetCurrent()->Lock();
            glDeleteTextures(1, &this->gl_texture);
            //MR::GL::GetCurrent()->Lock();
        }
    } else{
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") -> ResFreeState is off", MR_LOG_LEVEL_INFO);
    }
    this->gl_texture = 0;
}

MR::Texture* MR::Texture::FromFile(MR::ResourceManager* m, const std::string& file, std::string name){
    if(name == "Auto") name += "("+file+")";
    unsigned int glt = MR::LoadGLTexture(file);
    return (new MR::Texture(m, name, file, glt));
}

void MR::CubeMap::SetCapturePoint(const glm::vec3& p){
    if(p != _world_point_pos){
        _world_point_pos = p;
        OnCapturePointChanged(this, _world_point_pos);
    }
}

void MR::CubeMap::Capture(IRenderSystem* rs){
    rs->BindRenderTarget(_rtarget);
    _cam->SetPosition(_world_point_pos);

    //forward
    _cam->SetRotation(glm::vec3(0,0,0));
}

MR::RenderTarget* MR::CubeMap::GetRenderTarget(){
    return _rtarget;
}

MR::CubeMap::CubeMap(MR::ResourceManager* m, const std::string& name, const std::string& source, const unsigned short & Width, const unsigned short & Height)
 :
 MR::Resource(m, name, source),
 MR::Texture(m, name, source),
 _world_point_pos(0.0f, 0.0f, 0.0f),
 _cam(new MR::Camera(_world_point_pos, glm::vec3(0.0f, 0.0f, 0.0f), 90.0f, 0.1f, 1000.0f, ((float)Width / (float)Height))),
 _rtarget(new MR::RenderTarget("CubeMapCaptureTarget", 1, Width, Height)) {

    _target = Texture::Target::CubeMap;
    //_rtarget.CreateTargetTexture(0, MR::Texture::InternalFormat::RGB, MR::Texture::Format::RGB, MR::Texture::Type::UNSIGNED_BYTE);
    //_rtarget->CreateCubeMapTargetTexture(0, MR::Texture::InternalFormat::RGB, MR::Texture::Format::RGB, MR::Texture::Type::UNSIGNED_BYTE);
    //gl_texture = _rtarget->GetTargetTexture(0);

    //ResetInfo();
}

MR::CubeMap::~CubeMap(){
    delete _cam;
    delete _rtarget;
}

unsigned int MR::TextureArray::Add(void* newData){
    //MR::GL::GetCurrent()->Lock();
    glBindTexture(GL_TEXTURE_2D_ARRAY, _gl_texture);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, _storedTexNum, _width, _height, 1, (int)_format, (int)_type, newData);
    //glGenerateMipmap( GL_TEXTURE_2D_ARRAY );
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    //MR::GL::GetCurrent()->Lock();

    ++_storedTexNum;
    return (_storedTexNum-1);
}

void MR::TextureArray::Update(const unsigned int& textureIndex, void* newData){
    //MR::GL::GetCurrent()->Lock();

    glBindTexture(GL_TEXTURE_2D_ARRAY, _gl_texture);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, textureIndex, _width, _height, 1, (int)_format, (int)_type, newData);
    //glGenerateMipmap( GL_TEXTURE_2D_ARRAY );
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);

    //MR::GL::GetCurrent()->Lock();
}

MR::TextureArray::TextureArray(const Texture::InternalFormat& iformat, const Texture::Format& format, const Texture::Type& type, const unsigned short& width, const unsigned short& height, const unsigned int& maxTextures) : _maxTextures(maxTextures), _gl_texture(0), _internal_format(iformat), _format(format), _type(type), _width(width), _height(height) {
    //MR::GL::GetCurrent()->Lock();

    glGenTextures(1, &_gl_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, _gl_texture);

    //glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SPARSE_ARB, GL_TRUE );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );

    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    //glGetTextureSamplerHandleARB;

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 10, (int)iformat, width, height, _maxTextures);

    glBindTexture(GL_TEXTURE_2D_ARRAY,0);

    //MR::GL::GetCurrent()->Lock();
}

MR::TextureArray::~TextureArray(){
    //MR::GL::GetCurrent()->Lock();
    glDeleteTextures(1, &_gl_texture);
    //MR::GL::GetCurrent()->Lock();
}

bool MR::TextureArray::Support(const Texture::InternalFormat& iformat){
    switch((int)iformat){
    case GL_R8:
    case GL_R8_SNORM:
    case GL_R16:
    case GL_R16_SNORM:
    case GL_RG8:
    case GL_RG8_SNORM:
    case GL_RG16:
    case GL_RG16_SNORM:
    case GL_RGB565:
    case GL_RGBA8:
    case GL_RGBA8_SNORM:
    case GL_RGB10_A2:
    case GL_RGB10_A2UI:
    case GL_RGBA16:
    case GL_RGBA16_SNORM:
    case GL_R16F:
    case GL_RG16F:
    case GL_RGBA16F:
    case GL_R32F:
    case GL_RG32F:
    case GL_RGBA32F:
    case GL_R11F_G11F_B10F:
    case GL_RGB9_E5:
    case GL_R8I:
    case GL_R8UI:
    case GL_R16I:
    case GL_R16UI:
    case GL_R32I:
    case GL_R32UI:
    case GL_RG8I:
    case GL_RG8UI:
    case GL_RG16I:
    case GL_RG16UI:
    case GL_RG32I:
    case GL_RG32UI:
    case GL_RGBA8I:
    case GL_RGBA8UI:
    case GL_RGBA16I:
    case GL_RGBA16UI:
    case GL_RGBA32I:
    case GL_RGBA32UI:
        return true;
        break;
    }
    return false;
}

Resource* TextureManager::Create(const std::string& name, const std::string& source){
    if(this->_debugMessages) MR::Log::LogString("TextureManager "+name+" ("+source+") creating", MR_LOG_LEVEL_INFO);

    Texture * t = new Texture(dynamic_cast<MR::ResourceManager*>(this), name, source);
    if(_useTexArrays) t->_inTextureArray = true;
    this->_resources.push_back(t);
    return t;
}

Resource* TextureManager::Create(const std::string& name, const unsigned int & gl_texture){
    if(this->_debugMessages) MR::Log::LogString("TextureManager "+name+" (gl_texture) creating", MR_LOG_LEVEL_INFO);

    Texture * t = new Texture(dynamic_cast<MR::ResourceManager*>(this), name, "");
    t->gl_texture = gl_texture;
    t->ResetInfo();
    t->_loaded = true;
    this->_resources.push_back(t);
    return t;
}

TextureArray* TextureManager::_StoreTexture(unsigned int* texArrayIndex, void* data, const Texture::InternalFormat& iformat, const Texture::Format& format, const Texture::Type& type, const unsigned short& width, const unsigned short& height){
    for(size_t i = 0; i < _tex_arrays.size(); ++i){
        if( (_tex_arrays[i])->CanStore(iformat, format, type, width, height) ){
            *texArrayIndex = (_tex_arrays[i])->Add(data);
            return (_tex_arrays[i]);
        }
    }

    TextureArray* ta = new TextureArray(iformat, format, type, width, height, _maxTexturesInArray);
    *texArrayIndex = ta->Add(data);
    _tex_arrays.push_back(ta);
    return ta;
}

TextureManager::TextureManager() : ResourceManager() {
    _white = new MR::Texture(dynamic_cast<MR::ResourceManager*>(this), "White", "FromMem");
    unsigned char pixel_white_data[4]{255,255,255,255};
    MR::Texture::CreateOpenGLTexture(&pixel_white_data[0], 1, 1, GLTextureLoadFormat::RGBA, &(dynamic_cast<MR::Texture*>(_white)->gl_texture), GLTextureLoadFlags::None);
    Add(dynamic_cast<Resource*>(_white));
}

TextureManager::~TextureManager(){
    delete [] _tex_arrays.data();
    _tex_arrays.clear();
}

SingletonVar(TextureManager, new TextureManager());

TextureManager* TextureManager::Instance(){
    return SingletonVarName(TextureManager).Get();
}

void TextureManager::DestroyInstance() {
    SingletonVarName(TextureManager).Destroy();
}
