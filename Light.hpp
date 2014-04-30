#pragma once

#ifndef _MR_LIGHT_H_
#define _MR_LIGHT_H_

#include "Config.hpp"
#include "Types.hpp"
#include "Shader.hpp"

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
        Point = 0
    };

    virtual glm::vec3 GetPos() = 0;
    virtual float GetRadius() = 0;
    virtual LightManager* GetManager() = 0;
};

class LightSource : public Object, public ILightSource {
public:
    inline glm::vec3 GetPos() override { return _pos; }
    inline float GetRadius() override { return _radius; }
    inline LightManager* GetManager() override {return _manager;}

    ///TODO
    static ILightSource* CreatePointLight(const glm::vec3& pos, const glm::vec4& color, const float& radius);

    virtual ~LightSource();
private:
    LightSource() : Object() {}

protected:
    LightManager* _manager;
    ILightSource::Type _type;
    glm::vec3 _pos;
    glm::vec4 _color; //r,g,b,multiply
    float _radius; //max effect radius/distance
};

class LightsList {
public:
    inline ILightSource** GetLights() { return _lights.data(); }

    /**
        Find which lights affects on some area

        point - sphere center
        r - sphere radius
        listOfPtrs - array of pointers to lights
        listSize - num of elements in listOfPtrs
    **/
    static LightsList MakeList(const glm::vec3& point, const float& r, ILightSource** listOfPtrs, const size_t& listSize);
private:
    LightsList(const std::vector<ILightSource*>& l);
protected:
    std::vector<ILightSource*> _lights;
};

}

#endif // _MR_LIGHT_H_
