#include "Material.hpp"
#include "../Utils/Log.hpp"
//#include "../RenderManager.hpp"
#include "../Shaders/ShaderObjects.hpp"
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
    _program = mr::ShaderProgram::Default();
    _shaderParams.colorAmbient = _descr.colorAmbient;
    _shaderParams.colorDiffuse = _descr.colorDiffuse;

    if((!_descr.texColor.empty()) && _descr.texColor != "") {
        auto tex = mr::Texture::FromFile(_descr.texColor);

        if(tex) {
            mr::ITextureSettings* texSettings = new mr::TextureSettings();
            texSettings->Create();
            tex->SetSettings(texSettings);
            StateCache* stateCache = StateCache::GetDefault();
            if(!stateCache->BindTexture(tex, 0)) {
                mr::Log::LogString("Bind texture failed in DefaultMaterial::Create.", MR_LOG_LEVEL_ERROR);
                return;
            }
        }

        _shaderParams.texColor.handle = tex->GetGPUHandle();
    }
}

bool DefaultMaterial::Use() {
    if(_program) _program->Use();

    return true;
}

void DefaultMaterial::OnMaterialFlagChanged(MaterialFlag const& newFlag) {
    this->Use();
}

DefaultMaterial::DefaultMaterial() : _program(nullptr) {
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
