#include "Material.hpp"
#include "../Utils/Log.hpp"
//#include "../RenderManager.hpp"
#include "../Shaders/ShaderObjects.hpp"
#include "MaterialsConfig.hpp"
#include "../Utils/Debug.hpp"
#include "../Textures/TextureInterfaces.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

mr::MaterialFlag __MR_MATERIAL_FLAG = 0;//MR::MaterialFlag::Default();
mr::IMaterial* __MR_MATERIAL = nullptr;

namespace mr {

void DefaultMaterial::Create(MaterialDescr const& descr) {
    _descr = descr;
    _program = mr::ShaderProgram::DefaultBase();
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
