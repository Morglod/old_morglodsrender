#include "Material.hpp"
#include "../Utils/Log.hpp"
//#include "../RenderManager.hpp"
#include "../Shaders/ShaderManager.hpp"
#include "MaterialsConfig.hpp"
#include "../Utils/Debug.hpp"
#include "../Textures/TextureObjects.hpp"
#include "../Textures/TextureSettings.hpp"
#include "../StateCache.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

mr::MaterialFlag __MR_MATERIAL_FLAG = 0;//MR::MaterialFlag::Default();
mr::IMaterial* __MR_MATERIAL = nullptr;

namespace mr {

void DefaultMaterial::Create(MaterialDescr const& descr) {
    _descr = descr;
    _shaderParams.colorAmbient = _descr.colorAmbient;
    _shaderParams.colorDiffuse = _descr.colorDiffuse;

    ShaderManager* shaderManager = mr::ShaderManager::GetInstance();
    _program = shaderManager->DefaultShaderProgram();

    if((!_descr.texColor.empty()) && _descr.texColor != "") {
        texColorPtr = mr::Texture::FromFile(_descr.texColor);

        if(texColorPtr) {
            mr::ITextureSettings* texSettings = new mr::TextureSettings();
            texSettings->Create();
            texSettings->SetWrapT(descr.texColorWrapMode);
            texSettings->SetWrapS(descr.texColorWrapMode);
            texSettings->SetWrapR(descr.texColorWrapMode);
            texColorPtr->SetSettings(texSettings);
            /*StateCache* stateCache = StateCache::GetDefault();
            if(!stateCache->BindTexture(texColorPtr, 0)) {
                mr::Log::LogString("Bind texture failed in DefaultMaterial::Create.", MR_LOG_LEVEL_ERROR);
                return;
            }*/
            _shaderParams.texColor.handle = texColorPtr->GetGPUHandle();
        }

    }
}

bool DefaultMaterial::Use() {
    StateCache* stateCache = StateCache::GetDefault();
    if(_program) {
        stateCache->BindShaderProgram(_program);
    }
    if(texColorPtr != nullptr) stateCache->BindTexture(texColorPtr, 0);
    return true;
}

void DefaultMaterial::OnMaterialFlagChanged(MaterialFlag const& newFlag) {
    this->Use();
}

DefaultMaterial::DefaultMaterial() : _program(nullptr), texColorPtr(nullptr) {
}

DefaultMaterial::~DefaultMaterial() {
    if(__MR_MATERIAL == dynamic_cast<mr::IMaterial*>(this)) __MR_MATERIAL = nullptr;
}

MaterialFlag MaterialFlagGetUsed() {
    return __MR_MATERIAL_FLAG;
}

void MaterialFlagUse(MaterialFlag const& flag) {
    __MR_MATERIAL_FLAG = flag;
    if(__MR_MATERIAL != nullptr) __MR_MATERIAL->OnMaterialFlagChanged(flag);
}

IMaterial* MaterialGetUsed() {
    return __MR_MATERIAL;
}

void MaterialUse(IMaterial* mat) {
    if(__MR_MATERIAL == mat) return;
    __MR_MATERIAL = mat;
    if(mat) mat->Use();
}

}
