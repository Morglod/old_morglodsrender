#include "Material.hpp"
#include "../Utils/Log.hpp"
//#include "../RenderManager.hpp"
#include "../Shaders/ShaderObjects.hpp"
#include "MaterialsConfig.hpp"
#include "../Utils/Debug.hpp"
#include "../Textures/TextureObjects.hpp"
#include "../Textures/TextureSettings.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

mr::MaterialFlag __MR_MATERIAL_FLAG = 0;//MR::MaterialFlag::Default();
mr::IMaterial* __MR_MATERIAL = nullptr;

namespace mr {

void DefaultMaterial::Create(MaterialDescr const& descr) {
    _descr = descr;
    //_program = mr::ShaderProgram::Default();
    _shader_descr.colorAmbient = _descr.colorAmbient;
    _shader_descr.colorDiffuse = _descr.colorDiffuse;

    if((!_descr.texColor.empty()) && _descr.texColor != "") {
        auto tex = mr::Texture::FromFile(_descr.texColor);

        if(tex) {
            mr::ITextureSettings* texSettings = new mr::TextureSettings();
            texSettings->Create();
            tex->SetSettings(texSettings);
            tex->Bind(0);
        }

        _shader_descr.texColor.handle = tex->GetGPUHandle();
    }
}

bool DefaultMaterial::Use() {
    if(_program) _program->Use();

    return true;
}

void DefaultMaterial::OnMaterialFlagChanged(MaterialFlag const& newFlag) {
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
