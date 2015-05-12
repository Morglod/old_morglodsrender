#include "Material.hpp"
#include "../Utils/Log.hpp"
//#include "../RenderManager.hpp"
#include "../Shaders/ShaderManager.hpp"
#include "MaterialsConfig.hpp"
#include "../Utils/Debug.hpp"
#include "../Textures/TextureManager.hpp"
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
        texColorPtr = static_cast<mr::Texture*>(mr::TextureManager::GetInstance().LoadTexture2DFromFile(_descr.texColor));
        //TODO
        /*if(texColorPtr) {
            mr::TextureSettings* texSettings = mr::TextureManager::GetInstance().CreateSettings();
            mr::TextureSettings::Desc texSettingsDesc;
            texSettingsDesc.wrap_r = texSettingsDesc.wrap_s = texSettingsDesc.wrap_t = descr.texColorWrapMode;
            texSettings->Set(texSettingsDesc);
            texColorPtr->SetSettings(texSettings);
            _shaderParams.texColor.handle = texColorPtr->GetGPUHandle();
        }*/
    }
}

bool DefaultMaterial::Use() {
    StateCache* stateCache = StateCache::GetDefault();
    if(_program) {
        stateCache->SetShaderProgram(_program);
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
