#include "Texture.hpp"
#include "RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "RenderTarget.hpp"
#include "Context.hpp"
#include "Utils/Singleton.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

/** CLASS IMPLEMENTION **/
/*
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

MR::TextureArray::TextureArray(const Texture::InternalFormat& iformat, const Texture::Format& format, const Texture::Type& type, const unsigned short& width, const unsigned short& height, const unsigned int& maxTextures) : _maxTextures(maxTextures), _gl_texture(0), _internal_format(iformat), _format(format), _type(type), _width(width), _height(height) {

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
*/


