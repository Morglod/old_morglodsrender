#include "Models.hpp"
#include "../Geometry/MeshInterfaces.hpp"

namespace mr {

void SubModel::Draw(glm::mat4* modelMatrix) const {
    for(size_t i = 0; i < _meshes.GetNum(); ++i) {
        if(_meshes[i].expired()) continue;
        _meshes[i].lock()->Draw(modelMatrix);
    }
}

SubModel::SubModel() {}
SubModel::~SubModel() {}

size_t Model::GetLodIndexAtDistance(float dist) const {
    dist *= _bias;
    if(_lods.GetNum() == 0 || _lods.GetNum() == 1) return 0;
    if(_lods[0]->GetMinDist() > dist) return 0;
    for(size_t i = 0; i < _lods.GetNum(); i++) {
        if(_lods[i]->GetMinDist() > dist) return i-1;
    }
    return _lods.GetNum()-1;
}

bool Model::IsVisibleAtDist(float dist) const {
    if(_lods.GetNum() == 0) return false;
    return (_lods[0]->GetMinDist() >= dist);
}

}
