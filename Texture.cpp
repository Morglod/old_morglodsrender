#include "Texture.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#   include <SOIL.h>
#endif

using namespace MR;

unsigned int MR::LoadGLTexture(const std::string& file, const unsigned int & t, const GLTextureLoadFormat& format, const GLTextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_texture(file.c_str(),(int)format, t,(unsigned int)flags);
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

unsigned int MR::LoadGLCubemap(const std::string& x_pos_file, const std::string& x_neg_file, const std::string& y_pos_file, const std::string& y_neg_file, const std::string& z_pos_file, const std::string& z_neg_file,
                               const unsigned int & t, const GLTextureLoadFormat& format, const GLTextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_cubemap(x_pos_file.c_str(), x_neg_file.c_str(), y_pos_file.c_str(), y_neg_file.c_str(), z_pos_file.c_str(), z_neg_file.c_str(), (int)format, t,(unsigned int)flags);
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
    unsigned int i = SOIL_load_OGL_single_cubemap(file.c_str(), faceOrder, (int)format, t, (unsigned int)flags);
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

unsigned int MR::LoadGLHDR(const std::string& file, const int& rescale_to_max, const unsigned int & t, const GLTextureHDRFake& hdrFake, const GLTextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_HDR_texture(file.c_str(), (int)hdrFake, rescale_to_max, t, (unsigned int)flags);
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLHDR. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLHDR. " + lastResult);
        }
    }
    return i;
}

unsigned int MR::LoadGLTexture(const unsigned char *const buffer, const unsigned int& buffer_length, const unsigned int & t, const GLTextureLoadFormat& format, const GLTextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_texture_from_memory(buffer, buffer_length, (int)format, t,(unsigned int)flags);
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
    unsigned int i = SOIL_load_OGL_cubemap_from_memory(x_pos_buffer, x_pos_buffer_length,
                                                       x_neg_buffer, x_neg_buffer_length,
                                                       y_pos_buffer, y_pos_buffer_length,
                                                       y_neg_buffer, y_neg_buffer_length,
                                                       z_pos_buffer, z_pos_buffer_length,
                                                       z_neg_buffer, z_neg_buffer_length, (int)format, t,(unsigned int)flags);
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
    unsigned int i = SOIL_load_OGL_single_cubemap_from_memory(buffer, buffer_length, faceOrder, (int)format, t, (unsigned int)flags);
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
    bool state = SOIL_save_screenshot(file.c_str(), (int)image_type, x, y, width, height);
    if(state == false){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed SaveScreenshot. " + lastResult);
        } else {
            MR::Log::LogString("Failed SaveScreenshot. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return state;
}

unsigned char* MR::LoadImage(const std::string& file, int* width, int* height, GLTextureLoadFormat* format, const GLTextureLoadFormat& force_format){
    unsigned char* ptr = SOIL_load_image(file.c_str(), width, height, (int*)format, (int)force_format);
    if(ptr == NULL){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadImage. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadImage. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return ptr;
}

/** CLASS IMPLEMENTION **/

void MR::TextureSettings::SetLodBias(const float& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_LOD_BIAS, v);
    _lod_bias = v;
    OnLodBiasChanged(this, v);
}

void MR::TextureSettings::SetBorderColor(float* rgba) {
    glSamplerParameterfv(_sampler, GL_TEXTURE_BORDER_COLOR, rgba);
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
    glSamplerParameterfv(_sampler, GL_TEXTURE_BORDER_COLOR, _border_color);
    OnBorderColorChanged(this, _border_color);
}

void MR::TextureSettings::SetMagFilter(const MR::TextureSettings::MagFilter& v) {
    glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, (int)v);
    _mag_filter = v;
    OnMagFilterChanged(this, v);
}

void MR::TextureSettings::SetMinFilter(const MR::TextureSettings::MinFilter& v) {
    glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, (int)v);
    _min_filter = v;
    OnMinFilterChanged(this, v);
}

void MR::TextureSettings::SetMinLod(const int& v) {
    glSamplerParameteri(_sampler, GL_TEXTURE_MIN_LOD, v);
    _min_lod = v;
    OnMinLodChanged(this, v);
}

void MR::TextureSettings::SetMaxLod(const int& v) {
    glSamplerParameteri(_sampler, GL_TEXTURE_MAX_LOD, v);
    _max_lod = v;
    OnMaxLodChanged(this, v);
}

void MR::TextureSettings::SetWrapS(const Wrap& v) {
    glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, (int)v);
    _wrap_s = v;
    OnWrapSChanged(this, v);
}

void MR::TextureSettings::SetWrapR(const Wrap& v) {
    glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, (int)v);
    _wrap_r = v;
    OnWrapRChanged(this, v);
}

void MR::TextureSettings::SetWrapT(const Wrap& v) {
    glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, (int)v);
    _wrap_t = v;
    OnWrapTChanged(this, v);
}

void MR::TextureSettings::SetCompareMode(const CompareMode& v) {
    glSamplerParameteri(_sampler, GL_TEXTURE_COMPARE_MODE, (int)v);
    _compare_mode = v;
    OnCompareModeChanged(this, v);
}

void MR::TextureSettings::SetCompareFunc(const CompareFunc& v) {
    glSamplerParameterf(_sampler, GL_TEXTURE_COMPARE_FUNC, (int)v);
    _compare_func = v;
    OnCompareFuncChanged(this, v);
}

MR::TextureSettings::Ptr MR::TextureSettings::Copy(){
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
    return MR::TextureSettings::Ptr(ts);
}

MR::TextureSettings::TextureSettings() :
    _sampler(0), _lod_bias(MR_DEFAULT_TEXTURE_SETTINGS_LOD_BIAS),
    _border_color{MR_DEFAULT_TEXTURE_SETTINGS_BORDER_COLOR},
    _min_filter(MR_DEFAULT_TEXTURE_SETTINGS_MIN_FILTER),
    _mag_filter(MR_DEFAULT_TEXTURE_SETTINGS_MAG_FILTER),
    _min_lod(MR_DEFAULT_TEXTURE_SETTINGS_MIN_LOD),
    _max_lod(MR_DEFAULT_TEXTURE_SETTINGS_MAX_LOD),
    _wrap_s(MR_DEFAULT_TEXTURE_SETTINGS_WRAP_S),
    _wrap_r(MR_DEFAULT_TEXTURE_SETTINGS_WRAP_R),
    _wrap_t(MR_DEFAULT_TEXTURE_SETTINGS_WRAP_T),
     _compare_mode(MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_MODE),
     _compare_func(MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_FUNC) {

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
}

MR::TextureSettings::~TextureSettings() {
    glDeleteSamplers(1, &_sampler);
}

void MR::Texture::GetData(const int& mipMapLevel, const MR::Texture::Format& format, const MR::Texture::Type& type, void* dstBuffer) {
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    glGetTexImage(GL_TEXTURE_2D, mipMapLevel, (int)format, (int)type, dstBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MR::Texture::SetData(const int& mipMapLevel, const InternalFormat& internalFormat, const int& width, const int& height, const Format& format, const Type& type, void* data){
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
    OnDataChanged(this, mipMapLevel, internalFormat, width, height, format, type, data);
}

void MR::Texture::UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const Format& format, const Type& type, void* data) {
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
    if(this->_source != "") {
        if(_inTextureArray){
            MR::GLTextureLoadFormat f;
            unsigned char* bytes = MR::LoadImage(this->_source, (int*)&gl_width, (int*)&gl_height, &f, MR::GLTextureLoadFormat::Auto);
            if((gl_width == 0)||(gl_height == 0)){
                MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") loading failed. BAD SIZES", MR_LOG_LEVEL_ERROR);
                this->_loaded = false;
                return false;
            }

            int iform = 0;
            int format = 0;

            switch( (int)f ){
            case (int)GLTextureLoadFormat::Luminous:
                iform = GL_R8;
                format = (int)Format::LUMINANCE;
                break;
            case (int)GLTextureLoadFormat::LuminousAplha:
                iform = GL_RG8;
                format = (int)Format::LUMINANCE_ALPHA;
                break;
            case (int)GLTextureLoadFormat::RGB:
                iform = (int)InternalFormat::RGB8;
                format = (int)Format::RGB;
                break;
            case (int)GLTextureLoadFormat::RGBA:
                iform = (int)InternalFormat::RGBA8;
                format = (int)Format::RGBA;
                break;
            default:
                iform = (int)InternalFormat::RGB8;
                format = (int)Format::RGB;
                break;
            }

            _texArray = dynamic_cast<TextureManager*>(_resource_manager)->_StoreTexture(&_textureArrayIndex, bytes, (InternalFormat)iform, (Format)format, Type::UNSIGNED_BYTE, gl_width, gl_height);
            this->_loaded = true;
            gl_texture = 0;
            return true;
        } else {
            this->gl_texture = MR::LoadGLTexture(this->_source);
        }
    }
    else {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
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

void MR::Texture::CreateOpenGLTexture(GLuint* TexDst, const InternalFormat& internalFormat, const int& Width, const int& Height, const Format& format, const Type& type){
    glGenTextures(1, TexDst);
    glBindTexture(GL_TEXTURE_2D, *TexDst);
    glTexImage2D(GL_TEXTURE_2D, 0, (int)internalFormat, Width, Height, 0, (int)format, (int)type, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MR::Texture::CreateOpenGLTexture(const unsigned char *const data, const int& width, const int& height, const GLTextureLoadFormat& format, unsigned int * TexDst, const GLTextureLoadFlags& flags){
    unsigned int t = 0;
    if((*TexDst) != 0) t = *TexDst;
    *TexDst = SOIL_create_OGL_texture(data, width, height, (int)format, t, (unsigned int)flags);
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
    *TexDst = SOIL_create_OGL_single_cubemap(data, width, height, (int)format, face_order, t, (unsigned int)flags);
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

MR::Texture* MR::Texture::FromFile(MR::ResourceManager* m, const std::string& file, std::string name){
    if(name == "Auto") name += "("+file+")";
    unsigned int glt = MR::LoadGLTexture(file);
    return (new MR::Texture(m, name, file, glt));
}

unsigned int MR::TextureArray::Add(void* newData){
    glBindTexture(GL_TEXTURE_2D_ARRAY, _gl_texture);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, _storedTexNum, _width, _height, 1, (int)_format, (int)_type, newData);
    //glGenerateMipmap( GL_TEXTURE_2D_ARRAY );
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    ++_storedTexNum;
    return (_storedTexNum-1);
}

void MR::TextureArray::Update(const unsigned int& textureIndex, void* newData){
    glBindTexture(GL_TEXTURE_2D_ARRAY, _gl_texture);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, textureIndex, _width, _height, 1, (int)_format, (int)_type, newData);
    //glGenerateMipmap( GL_TEXTURE_2D_ARRAY );
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
}

MR::TextureArray::TextureArray(const Texture::InternalFormat& iformat, const Texture::Format& format, const Texture::Type& type, const unsigned short& width, const unsigned short& height, const unsigned int& maxTextures) : _maxTextures(maxTextures), _internal_format(iformat), _format(format), _type(type), _width(width), _height(height), _gl_texture(0) {
    glGenTextures(1, &_gl_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, _gl_texture);

    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_FALSE );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );

    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 10, (int)iformat, width, height, _maxTextures);
    //glTexImage3D(GL_TEXTURE_2D_ARRAY, 10, (int)iformat, width, height, _maxTextures, 0, (int)format, (int)type, NULL);

    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
}

MR::TextureArray::~TextureArray(){
    glDeleteTextures(1, &_gl_texture);
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

    Texture * t = new Texture(this, name, source);
    if(_useTexArrays) t->_inTextureArray = true;
    this->_resources.push_back(t);
    return t;
}

Resource* TextureManager::Create(const std::string& name, const unsigned int & gl_texture){
    if(this->_debugMessages) MR::Log::LogString("TextureManager "+name+" (gl_texture) creating", MR_LOG_LEVEL_INFO);

    Texture * t = new Texture(this, name, "");
    t->gl_texture = gl_texture;
    t->ResetInfo();
    t->_loaded = true;
    this->_resources.push_back(t);
    return t;
}

TextureArray* TextureManager::_StoreTexture(unsigned int* texArrayIndex, void* data, const Texture::InternalFormat& iformat, const Texture::Format& format, const Texture::Type& type, const unsigned short& width, const unsigned short& height){
    for(auto it = _tex_arrays.begin(); it != _tex_arrays.end(); ++it){
        if( (*it)->CanStore(iformat, format, type, width, height) ){
            *texArrayIndex = (*it)->Add(data);
            return (*it);
        }
    }

    TextureArray* ta = new TextureArray(iformat, format, type, width, height, _maxTexturesInArray);
    *texArrayIndex = ta->Add(data);
    _tex_arrays.push_back(ta);
    return ta;
}

TextureManager::~TextureManager(){
    delete [] _tex_arrays.data();
    _tex_arrays.clear();
}
