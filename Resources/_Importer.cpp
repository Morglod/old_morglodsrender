#include "Importer.hpp"
#include "../Utils/Log.hpp"

namespace MR {

/** INTERFACE **/

ImporterStage::ImporterStage(const char& stage, ImporterStage* next, ImporterStage* prev, Importer* imp) :
    _stage(stage), _next(next), _prev(prev), _importer(imp), _imported(false) {}

ImporterStage::~ImporterStage(){
    _importer->_stages[(int)_stage] = nullptr;
}

/** ANIMS **/

void ImporterStage_Anims::_GoToEnd() {
}

bool ImporterStage_Anims::Import(){
    return true;
}

ImporterStage_Anims::ImporterStage_Anims(ImporterStage* next, ImporterStage* prev, Importer* imp) :
    ImporterStage(ImporterStage::STAGE_ANIM, next, prev, imp)
{  }

ImporterStage_Anims::~ImporterStage_Anims(){
}

/** CAMS **/

void ImporterStage_Cams::_GoToEnd() {
}

bool ImporterStage_Cams::Import(){
    return true;
}

ImporterStage_Cams::ImporterStage_Cams(ImporterStage* next, ImporterStage* prev, Importer* imp) :
    ImporterStage(ImporterStage::STAGE_CAM, next, prev, imp)
{  }

ImporterStage_Cams::~ImporterStage_Cams(){
}

/** LIGHTS **/

void ImporterStage_Lights::_GoToEnd() {
}

bool ImporterStage_Lights::Import(){
    return true;
}

ImporterStage_Lights::ImporterStage_Lights(ImporterStage* next, ImporterStage* prev, Importer* imp) :
    ImporterStage(ImporterStage::STAGE_LIGHT, next, prev, imp)
{  }

ImporterStage_Lights::~ImporterStage_Lights(){
}

/** MATERIALS **/

void ImporterStage_Materials::_GoToEnd() {
}

bool ImporterStage_Materials::Import(){
    return true;
}

ImporterStage_Materials::ImporterStage_Materials(ImporterStage* next, ImporterStage* prev, Importer* imp) :
    ImporterStage(ImporterStage::STAGE_MATERIAL, next, prev, imp)
{  }

ImporterStage_Materials::~ImporterStage_Materials(){
}

/** MESHES **/

void ImporterStage_Meshes::_GoToEnd() {
}

bool ImporterStage_Meshes::Import(){
    return true;
}

ImporterStage_Meshes::ImporterStage_Meshes(ImporterStage* next, ImporterStage* prev, Importer* imp) :
    ImporterStage(ImporterStage::STAGE_MESH, next, prev, imp)
{  }

ImporterStage_Meshes::~ImporterStage_Meshes(){
}

/** TEXTURES **/

void ImporterStage_Textures::_GoToEnd() {
}

bool ImporterStage_Textures::Import(){
    return true;
}

ImporterStage_Textures::ImporterStage_Textures(ImporterStage* next, ImporterStage* prev, Importer* imp) :
    ImporterStage(ImporterStage::STAGE_TEXTURE, next, prev, imp)
{  }

ImporterStage_Textures::~ImporterStage_Textures(){
}

/** IMPORTER **/

Importer* Importer::Open(const std::string& file){
    Importer* imp = new Importer();
    if(!imp->_OpenFile(file)){
        delete imp;
        return nullptr;
    }

    /** MAKE STAGES **/
    imp->_stages[0] = new ImporterStage_Anims(nullptr, nullptr, imp);
    imp->_stages[1] = new ImporterStage_Cams(0, nullptr, imp);
    imp->_stages[2] = new ImporterStage_Lights(0, nullptr, imp);
    imp->_stages[3] = new ImporterStage_Materials(0, nullptr, imp);
    imp->_stages[4] = new ImporterStage_Meshes(0, nullptr, imp);
    imp->_stages[5] = new ImporterStage_Textures(0, nullptr, imp);

    //Make next/prev ptrs
    imp->_stages[0]->_next = imp->_stages[1];
    imp->_stages[5]->_prev = imp->_stages[4];
    for(int i = 1; i < 5; ++i){
        imp->_stages[i]->_next = imp->_stages[i+1];
        imp->_stages[i]->_prev = imp->_stages[i-1];
    }

    imp->_current = imp->_stages[0];

    /** READ RESOURCES NUM **/
    if(imp->_log) MR::Log::LogString("Reading resources num", MR_LOG_LEVEL_INFO);

    imp->_file.read( reinterpret_cast<char*>(&imp->_numAnims), sizeof(int));
    if(imp->_log) MR::Log::LogString("Anims num " + std::to_string(imp->_numAnims), MR_LOG_LEVEL_INFO);

    imp->_file.read( reinterpret_cast<char*>(&imp->_numCams), sizeof(int));
    if(imp->_log) MR::Log::LogString("Cams num " + std::to_string(imp->_numCams), MR_LOG_LEVEL_INFO);

    imp->_file.read( reinterpret_cast<char*>(&imp->_numLights), sizeof(int));
    if(imp->_log) MR::Log::LogString("Lights num " + std::to_string(imp->_numLights), MR_LOG_LEVEL_INFO);

    imp->_file.read( reinterpret_cast<char*>(&imp->_numMaterials), sizeof(int));
    if(imp->_log) MR::Log::LogString("Materials num " + std::to_string(imp->_numMaterials), MR_LOG_LEVEL_INFO);

    imp->_file.read( reinterpret_cast<char*>(&imp->_numMeshes), sizeof(int));
    if(imp->_log) MR::Log::LogString("Meshes num " + std::to_string(imp->_numMeshes), MR_LOG_LEVEL_INFO);

    imp->_file.read( reinterpret_cast<char*>(&imp->_numTextures), sizeof(int));
    if(imp->_log) MR::Log::LogString("Textures num " + std::to_string(imp->_numTextures), MR_LOG_LEVEL_INFO);

    return imp;
}

Importer::Importer() :
    _file(), _current(0), _numAnims(0), _numCams(0), _numLights(0), _numMaterials(0), _numMeshes(0), _numTextures(0), _log(false) {
}

Importer::~Importer() {
    for(int i = 0; i < ((int)ImporterStage::STAGES_NUM); ++i){
        delete _stages[i];
    }
    _file.close();
}

bool Importer::_OpenFile(const std::string& file){
    _file.open(file, std::ios::in | std::ios::binary);
    return _file.is_open();
}

}
