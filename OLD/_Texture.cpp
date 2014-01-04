#include "Texture.h"
#include "Log.h"

void MR::Texture::Load(){
    std::cout << "TEX[LOAD]";
    this->_current_state = MR::IResource::LoadingState::LoadingState_Loading;
    this->_send_event(0);
    try{
        this->gl_TEXTURE = MR::LoadGLTexture(this->_source);

        //--
        this->_send_event(1);
        this->_current_state = MR::IResource::LoadingState::LoadingState_Loaded;
    }
    catch(std::exception & e){
        this->_send_event(4);
        this->_current_state = MR::IResource::LoadingState::LoadingState_Error;
        MR::Log::LogException(e);
    }
    std::cout << "TEX[LOAD]OK";
}

void MR::Texture::UnLoad(){
    std::cout << "TEX[UNLOAD]";
    this->_current_state = MR::IResource::LoadingState::LoadingState_Unloading;
    this->_send_event(2);
    try{
        glDeleteTextures(1, &this->gl_TEXTURE);

        //--
        this->_current_state = MR::IResource::LoadingState::LoadingState_Loaded;
        this->_send_event(3);
    }
    catch(std::exception & e){
        this->_send_event(4);
        this->_current_state = MR::IResource::LoadingState::LoadingState_Error;
        MR::Log::LogException(e);
    }
    std::cout << "TEX[UNLOAD]OK";
}

MR::Texture::Texture(MR::IResourceManager* manager, std::string source, std::string name){
    this->_manager = manager;
    this->_source = source;
    this->_name = name;
}

MR::Texture::~Texture(){
    this->UnLoad();
}
