#include "Light.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <glm/gtx/transform.hpp>

namespace MR{

LightSource::~LightSource(){
}

LightsList LightsList::MakeList(MR::ICollidable* bound, ILightSource** listOfPtrs, const size_t& listSize){
    std::vector<ILightSource*> l;
    if(bound){
        for(size_t i = 0; i < listSize; ++i){
            if( bound->TestSphere(listOfPtrs[i]->GetPos(), listOfPtrs[i]->GetRadius()) || (listOfPtrs[i]->GetType() == MR::ILightSource::Type::Dir) ) l.push_back(listOfPtrs[i]);
        }
    }
    return LightsList(l);
}

LightsList::LightsList(const std::vector<ILightSource*>& l) : _lights(l) {
}

ILightSource* LightSource::CreatePointLight(const glm::vec3& pos, const glm::vec3& emission, const glm::vec3& ambient, const float& attenuation, const float& power, const float& radius) {
    LightSource* lsrc = new LightSource();
    lsrc->_pos = pos;
    lsrc->_em = emission;
    lsrc->_ambient = ambient;
    lsrc->_att = attenuation;
    lsrc->_power = power;
    lsrc->_radius = radius;
    lsrc->_type = Type::Point;
    return lsrc;
}

ILightSource* LightSource::CreateDirLight(const glm::vec3& dir, const glm::vec3& emission, const glm::vec3& ambient){
    LightSource* lsrc = new LightSource();
    lsrc->_dir = dir;
    lsrc->_ambient = ambient;
    lsrc->_em = emission;
    lsrc->_type = Type::Dir;
    return lsrc;
}

}
