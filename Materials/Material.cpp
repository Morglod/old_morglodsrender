#include "Material.hpp"
#include "../Utils/Log.hpp"
//#include "../RenderManager.hpp"
#include "../Shaders/ShaderManager.hpp"
#include "MaterialsConfig.hpp"
#include "../Utils/Debug.hpp"
#include "../Textures/TextureManager.hpp"
#include "../StateCache.hpp"
#include "../MachineInfo.hpp"

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
    colorTexture = _descr.colorTexture;
    normalTexture = _descr.normalTexture;
    specularTexture = _descr.specularTexture;
}

bool DefaultMaterial::Use() {
    StateCache* stateCache = StateCache::GetDefault();
    if(_program) {
        stateCache->SetShaderProgram(_program);
        ShaderUniformMap* umap = _program->GetMap();
        umap->SetUniformT("MR_TEX_COLOR", colorTexture.index);
        umap->SetUniformT("MR_TEX_NORMAL", normalTexture.index);
        umap->SetUniformT("MR_TEX_SPECULAR", specularTexture.index);

        if(normalTexture.texture == nullptr) umap->SetUniformT("MR_TEX_NORMAL_F", 0.0f);
        else umap->SetUniformT("MR_TEX_NORMAL_F", 1.0f);

        if(specularTexture.texture == nullptr) umap->SetUniformT("MR_TEX_SPECULAR_F", 0.0f);
        else umap->SetUniformT("MR_TEX_SPECULAR_F", 1.0f);

        bool arb;
        if(mr::gl::IsBindlessTextureSupported(arb)) {
            stateCache->BindUniformBuffer(colorTexture.ubo, 1);
        } else {
            stateCache->BindTexture(colorTexture.texture, colorTexture.index);
        }
    }

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
