#include "Texture.h"
#include "Log.h"

using namespace MR;

bool MR::Texture::Load(){
    if(this->_manager->debugMessages) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") loading");
    if(this->_source != "") this->gl_texture = MR::LoadGLTexture(this->_source);
    else if(this->_manager->debugMessages){
        MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") load failed. Source is null");
        this->_loaded = false;
        return false;
    }
    if(this->gl_texture == 0){
        MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") loading failed. GL_TEXTURE is null");
        this->_loaded = false;
        return false;
    }

    this->_loaded = true;
    return true;
}

void MR::Texture::UnLoad(){
    if(this->_manager->debugMessages) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") unloading");
    glDeleteTextures(1, &this->gl_texture);
    this->gl_texture = 0;
}

bool MR::Texture::ReLoad(){
    if(this->_manager->debugMessages) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") reloading");
    this->UnLoad();
    return this->Load();
}

MR::Texture::Texture(MR::TextureManager* manager, std::string name, std::string source){
    this->_manager = manager;
    this->_source = source;
    this->_name = name;
}

MR::Texture::~Texture(){
    if(this->_manager->debugMessages) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") deleting");
    this->UnLoad();
}

Resource* TextureManager::Create(std::string name, std::string source){
    if(this->debugMessages) MR::Log::LogString("TextureManager "+name+" ("+source+") creating");
    Texture * t = new Texture(this, name, source);
    this->_resources.push_back(t);
    return t;
}

Resource* TextureManager::Create(std::string name, unsigned int gl_texture){
    if(this->debugMessages) MR::Log::LogString("TextureManager "+name+" (gl_texture) creating");
    Texture * t = new Texture(this, name, "");
    t->gl_texture = gl_texture;
    this->_resources.push_back(t);
    return t;
}

Resource* TextureManager::CreateAndLoad(std::string name, std::string source){
    if(this->debugMessages) MR::Log::LogString("TextureManager "+name+" ("+source+") creating and loading");
    Resource* t = this->Create(name, source);
    t->Load();
    return t;
}

Resource* TextureManager::Get(std::string source){
    if(this->debugMessages) MR::Log::LogString("TextureManager ("+source+") getting");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        if( (*it)->GetSource() == source ) return *it;
    }
    return nullptr;
}

Resource* TextureManager::Find(std::string name){
    if(this->debugMessages) MR::Log::LogString("TextureManager "+name+" searching");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        if( (*it)->GetName() == name ) return *it;
    }
    return nullptr;
}

Resource* TextureManager::Need(std::string source){
    if(this->debugMessages) MR::Log::LogString("TextureManager ("+source+") needed");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        if( (*it)->GetSource() == source ) return *it;
    }
    return this->CreateAndLoad("AutoLoaded_"+source, source);
}

Resource** TextureManager::Need(std::string* sources, const unsigned int num){
    Resource** finded_list = new Resource*[num];
    if(this->debugMessages) MR::Log::LogString("TextureManager List of resources needed");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
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

void TextureManager::Remove(std::string name, std::string source){
    if(this->debugMessages) MR::Log::LogString("TextureManager "+name+" ("+source+") removing");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        if( (*it)->GetName() == name && (*it)->GetSource() == source){
            delete *it;
            this->_resources.erase(it);
        }
    }
}

void TextureManager::RemoveAll(){
    if(this->debugMessages) MR::Log::LogString("TextureManager removing all");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        delete *it;
        this->_resources.erase(it);
        it--;
    }
}
