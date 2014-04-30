#include "Light.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <glm/gtx/transform.hpp>

namespace MR{

LightSource::~LightSource(){
}

LightsList LightsList::MakeList(const glm::vec3& point, const float& r, ILightSource** listOfPtrs, const size_t& listSize){
    std::vector<ILightSource*> l;
    for(size_t i = 0; i < listSize; ++i){
        if( glm::distance(listOfPtrs[i]->GetPos(), point) <= (r+listOfPtrs[i]->GetRadius()) ) l.push_back(listOfPtrs[i]);
    }
    return LightsList(l);
}

LightsList::LightsList(const std::vector<ILightSource*>& l) : _lights(l) {
}

LightManager::LightManager(ShaderManager* sm){  }

LightManager::~LightManager(){  }

ILightSource* LightSource::CreatePointLight(const glm::vec3& pos, const glm::vec4& color, const float& radius) {
    LightSource* lsrc = new LightSource();
    lsrc->_pos = pos;
    lsrc->_color = color;
    lsrc->_radius = radius;
    lsrc->_type = Type::Point;
    return lsrc;
}

}
