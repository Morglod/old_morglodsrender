#include "Mesh.h"

void MR::Mesh::Draw(){
    //!!NODE DRAW CODE----------------------
    glPushMatrix();

    glTranslatef(this->tX, this->tY, this->tZ);
    glRotatef(this->rX, 1, 0, 0);
    glRotatef(this->rY, 0, 1, 0);
    glRotatef(this->rZ, 0, 0, 1);
    glScalef(this->sX, this->sY, this->sZ);
    //!!END FIRST PART-----------------------

    //!!FOR SPEED
    MR::Log::LogString("db0");
    if(this->materials == NULL){
        for(unsigned int i = 0; i < this->geom_buffers_num; ++i){
            if(this->geom_buffers[i] != nullptr) this->geom_buffers[i]->Draw();
        }
        MR::Log::LogString("db1");
    }
    else if(this->SingleMaterial){
            MR::Log::LogString("db2");
        glColor4f(this->materials[0]->AmbientColor[0], this->materials[0]->AmbientColor[1], this->materials[0]->AmbientColor[2], this->materials[0]->AmbientColor[3]);
        if(this->materials[0]->AmbientTexture != NULL) this->materials[0]->AmbientTexture->Bind(this->materials[0]->AmbientTextureStage);
        if(this->materials[0]->GlobalShader != NULL) this->materials[0]->GlobalShader->Use();
        for(unsigned int i = 0; i < this->geom_buffers_num; ++i){
            if(this->geom_buffers[i] != nullptr) this->geom_buffers[i]->Draw();
        }
    MR::Log::LogString("db3");
    }
    else{
            MR::Log::LogString("db4");
        for(unsigned int i = 0; i < this->geom_buffers_num; ++i){
            glColor4f(this->materials[i]->AmbientColor[0], this->materials[i]->AmbientColor[1], this->materials[i]->AmbientColor[2], this->materials[i]->AmbientColor[3]);
            if(this->materials[i]->AmbientTexture != NULL) this->materials[i]->AmbientTexture->Bind(this->materials[i]->AmbientTextureStage);
            if(this->materials[i]->GlobalShader != NULL) this->materials[i]->GlobalShader->Use();
            this->geom_buffers[i]->Draw();
        }
    MR::Log::LogString("db5");
    }
    MR::Log::LogString("db6");
    //!!NODE DRAW CODE--------------
    for(std::vector<Node*>::iterator it = this->Children.begin(); it != this->Children.end(); it++){
        (*it)->Draw();
    }

    glPopMatrix();
    //!!END NODE DRAWING CODE-------
}

bool MR::Mesh::Load(){
    if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") loading");
    if(this->_source != ""){
        MR::Mesh* m = MR::ImportMesh(this->_source);
        if(m == nullptr){
            MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") importing failed. Mesh is null");
            this->_loaded = false;
            return false;
        }

        this->Children = m->Children;
        this->geom_buffers = m->geom_buffers;
        this->geom_buffers_num = m->geom_buffers_num;
        this->materials = m->materials;
        this->name = m->name;
        this->parent = m->parent;
        this->tX = m->tX;
        this->tY = m->tY;
        this->tZ = m->tZ;
        this->rX = m->rX;
        this->rY = m->rY;
        this->rZ = m->rZ;
        this->sX = m->sX;
        this->sY = m->sY;
        this->sZ = m->sZ;
        this->SingleMaterial = m->SingleMaterial;
        m = nullptr;
    }
    else if(this->_manager->debugMessages){
        MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") load failed. Source is null");
        this->_loaded = false;
        return false;
    }

    this->_loaded = true;
    return true;
}

void MR::Mesh::UnLoad(){
    if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") unloading");

    for(unsigned int i = 0; i < this->geom_buffers_num; ++i){
        this->geom_buffers[i] = nullptr;
    }
    delete [] this->geom_buffers;
    this->geom_buffers = nullptr;
    this->geom_buffers_num = 0;

    if(this->SingleMaterial){
        this->materials[0] = nullptr;
        delete [] this->materials;
    }
    else{
        for(unsigned int i = 0; i < this->geom_buffers_num; ++i){
            this->materials[i] = nullptr;
        }
        delete [] this->materials;
        this->materials = nullptr;
    }
}

bool MR::Mesh::ReLoad(){
    if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") reloading");
    this->UnLoad();
    return this->Load();
}

MR::Mesh::Mesh(MR::MeshManager* manager, std::string name, std::string source){
    this->_manager = manager;
    this->_source = source;
    this->_name = name;
}

MR::Mesh::~Mesh(){
    if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") deleting");
    this->UnLoad();
}

//RESOURCE MANAGER
MR::Resource* MR::MeshManager::Create(std::string name, std::string source){
    if(this->debugMessages) MR::Log::LogString("MeshManager "+name+" ("+source+") creating");
    Mesh * t = new Mesh(this, name, source);
    this->_resources.push_back(t);
    return t;
}
MR::Resource* MR::MeshManager::CreateAndLoad(std::string name, std::string source){
    if(this->debugMessages) MR::Log::LogString("MeshManager "+name+" ("+source+") creating and loading");
    Resource* t = this->Create(name, source);
    t->Load();
    return t;
}
MR::Resource* MR::MeshManager::Get(std::string source){
    if(this->debugMessages) MR::Log::LogString("MeshManager ("+source+") getting");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        if( (*it)->GetSource() == source ) return *it;
    }
    return nullptr;
}
MR::Resource* MR::MeshManager::Find(std::string name){
    if(this->debugMessages) MR::Log::LogString("MeshManager "+name+" searching");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        if( (*it)->GetName() == name ) return *it;
    }
    return nullptr;
}
MR::Resource* MR::MeshManager::Need(std::string source){
    if(this->debugMessages) MR::Log::LogString("MeshManager ("+source+") needed");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        if( (*it)->GetSource() == source ) return *it;
    }
    return this->CreateAndLoad("AutoLoaded_"+source, source);
}
void MR::MeshManager::Remove(std::string name, std::string source){
    if(this->debugMessages) MR::Log::LogString("MeshManager "+name+" ("+source+") removing");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        if( (*it)->GetName() == name && (*it)->GetSource() == source){
            delete *it;
            this->_resources.erase(it);
        }
    }
}
void MR::MeshManager::RemoveAll(){
    if(this->debugMessages) MR::Log::LogString("MeshManager removing all");
    for(auto it = this->_resources.begin(); it != this->_resources.end(); it++){
        delete *it;
        this->_resources.erase(it);
        it--;
    }
}
