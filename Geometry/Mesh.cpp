#include "Mesh.hpp"
#include "../Utils/Log.hpp"
#include "../Materials/MaterialInterfaces.hpp"
#include "GeometryInterfaces.hpp"
#include "../Utils/Debug.hpp"
#include "../Shaders/ShaderInterfaces.hpp"
#include <Macro.hpp>
#include "../StateCache.hpp"

void mr::Mesh::Draw(glm::mat4* modelMat) {
    if(_mat) {
        _mat->Use();
    }
    mr::StateCache* stateCache = mr::StateCache::GetDefault();
    mr::IShaderProgram* usedShaderProgram = stateCache->GetBindedShaderProgram();

    if(usedShaderProgram) {
        IShaderUniform* uni = nullptr;
        if((uni = usedShaderProgram->FindShaderUniform(MR_SHADER_MODEL_MAT4)))
            uni->SetPtr(modelMat);
        else
            usedShaderProgram->CreateUniform(MR_SHADER_MODEL_MAT4, IShaderUniform::Mat4, modelMat);
    }
    for(size_t i = 0; i < _geom.GetNum(); ++i){
        _geom.GetRaw()[i]->Draw();
    }
}

mr::Mesh::Mesh(TStaticArray<IGeometry*> geom, IMaterial* mat)
 : _geom(geom), _mat(mat) {
}

mr::Mesh::~Mesh() {
}

mr::IMesh* mr::Mesh::Create(TStaticArray<IGeometry*> geom, IMaterial* mat) {
    return dynamic_cast<mr::IMesh*>(new mr::Mesh(geom, mat));
}
