#pragma once

#ifndef _MR_LIGHT_H_
#define _MR_LIGHT_H_

#include "Config.hpp"
#include "Types.hpp"
#include "Shader.hpp"
#include "Boxes.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR {

class ILightSource;
class LightSource;
class LightManager;
class IShader;
class ShaderManager;

class ILightSource {
public:
    enum class Type : unsigned char {
        Point = 0, //point { pos, emission, attenuation, radius, power, ambient }
        Dir = 1 //directional { direction, emission, ambient }
    };

    virtual glm::vec3 GetPos() = 0;
    virtual glm::vec3 GetEmission() = 0;
    virtual glm::vec3 GetAmbient() = 0;
    virtual float GetAttenuation() = 0;
    virtual float GetRadius() = 0;
    virtual float GetPower() = 0;

    virtual glm::vec3* GetPosP() = 0;
    virtual glm::vec3* GetEmissionP() = 0;
    virtual glm::vec3* GetAmbientP() = 0;
    virtual float* GetAttenuationP() = 0;
    virtual float* GetRadiusP() = 0;
    virtual float* GetPowerP() = 0;

    virtual glm::vec3 GetDir() = 0;
    virtual glm::vec3* GetDirP() = 0;

    virtual ILightSource::Type GetType() = 0;

    virtual LightManager* GetManager() = 0;
};

class LightSource : public Object, public ILightSource {
public:
    inline glm::vec3 GetPos() override { return _pos; }
    inline glm::vec3 GetEmission() override { return _em; }
    inline glm::vec3 GetAmbient() override { return _ambient; }
    inline float GetAttenuation() override { return _att; }
    inline float GetPower() override { return _power; }
    inline float GetRadius() override { return _radius; }

    inline glm::vec3* GetPosP() override { return &_pos; }
    inline glm::vec3* GetEmissionP() override { return &_em; }
    inline glm::vec3* GetAmbientP() override { return &_ambient; }
    inline float* GetAttenuationP() override { return &_att; }
    inline float* GetPowerP() override { return &_power; }
    inline float* GetRadiusP() override { return &_radius; }

    inline glm::vec3 GetDir() override { return _dir; }
    inline glm::vec3* GetDirP() override { return &_dir; }

    inline ILightSource::Type GetType() override { return _type; }

    inline LightManager* GetManager() override {return _manager;}

    static ILightSource* CreatePointLight(const glm::vec3& pos, const glm::vec3& emission, const glm::vec3& ambient, const float& attenuation, const float& power, const float& radius);
    static ILightSource* CreateDirLight(const glm::vec3& dir, const glm::vec3& emission, const glm::vec3& ambient);

    virtual ~LightSource();
private:
    LightSource() : Object() {}

protected:
    LightManager* _manager;
    ILightSource::Type _type;
    glm::vec3 _pos, _em, _dir, _ambient;
    float _radius; //max effect radius/distance
    float _att;
    float _power;
};

class LightsList {
public:
    inline ILightSource** GetLights() { return _lights.data(); }
    inline size_t GetLightsNum() { return _lights.size(); }

    /**
        Find which lights affects on some area

        point - sphere center
        r - sphere radius
        listOfPtrs - array of pointers to lights
        listSize - num of elements in listOfPtrs
    **/
    static LightsList MakeList(MR::ICollidable* bound, ILightSource** listOfPtrs, const size_t& listSize);
private:
    LightsList(const std::vector<ILightSource*>& l);
protected:
    std::vector<ILightSource*> _lights;
};

}

#endif // _MR_LIGHT_H_
