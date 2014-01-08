#include "Texture.hpp"

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

    return true;
}

void MR::Texture::UnLoad(){
    if(this->_manager->debugMessages) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") unloading");
    if(_res_free_state) {
        if(this->_manager->debugMessages) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") -> ResFreeState is on, deleting data");
        glDeleteTextures(1, &this->gl_texture);
    } else{
        if(this->_manager->debugMessages) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") -> ResFreeState is off");
    }
    this->gl_texture = 0;
}

bool MR::Texture::ReLoad(){
    if(this->_manager->debugMessages) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") reloading");
    this->UnLoad();
    return this->Load();
}

MR::Texture::Texture(MR::ResourceManager* manager, std::string name, std::string source) : Resource(manager, name, source), _name(name), _source(source) {
    this->_manager = dynamic_cast<TextureManager*>(manager);
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
    for(std::vector<Texture*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetSource() == source ) return *it;
    }
    return nullptr;
}

Resource* TextureManager::Find(std::string name){
    if(this->debugMessages) MR::Log::LogString("TextureManager "+name+" searching");
    for(std::vector<Texture*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetName() == name ) return *it;
    }
    return nullptr;
}

Resource* TextureManager::Need(std::string source){
    if(this->debugMessages) MR::Log::LogString("TextureManager ("+source+") needed");
    for(std::vector<Texture*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetSource() == source ) return *it;
    }
    return this->CreateAndLoad("AutoLoaded_"+source, source);
}

Resource** TextureManager::Need(std::string* sources, const unsigned int num){
    Resource** finded_list = new Resource*[num];
    if(this->debugMessages) MR::Log::LogString("TextureManager List of resources needed");
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

void TextureManager::Remove(std::string name, std::string source){
    if(this->debugMessages) MR::Log::LogString("TextureManager "+name+" ("+source+") removing");
    for(std::vector<Texture*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetName() == name && (*it)->GetSource() == source){
            delete *it;
            this->_resources.erase(it);
        }
    }
}

void TextureManager::RemoveAll(){
    if(this->debugMessages) MR::Log::LogString("TextureManager removing all");
    for(std::vector<Texture*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        delete *it;
        this->_resources.erase(it);
        it--;
    }
}
