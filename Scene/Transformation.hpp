#pragma once

#ifndef _MR_TRANSFORMATION_H_
#define _MR_TRANSFORMATION_H_

#include <Events.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace mr{

class ITransformable {
public:
    virtual glm::vec3 GetPos() = 0;
    virtual glm::vec3 GetRot() = 0;
    virtual glm::vec3 GetScale() = 0;

    virtual void SetPos(glm::vec3 const&) = 0;
    virtual void SetRot(glm::vec3 const&) = 0;
    virtual void SetScale(glm::vec3 const&) = 0;

    virtual glm::mat4 GetMat() = 0;
    virtual glm::mat4* GetMatPtr() = 0;

    virtual void CalcMatrix() = 0;
    virtual void SetAutoCalcMatrix(bool const&) = 0;
    virtual bool GetAutoCalcMatrix() = 0;
};

class Transformable : public ITransformable {
public:
    inline glm::vec3 GetPos() override { return _pos; }
    inline glm::vec3 GetRot() override { return _rot; }
    inline glm::vec3 GetScale() override { return _scale; }

    inline void SetPos(glm::vec3 const& v) override { _pos = v; if(_auto_calc) CalcMatrix(); }
    inline void SetRot(glm::vec3 const& v) override { _rot = v; if(_auto_calc) CalcMatrix();  }
    inline void SetScale(glm::vec3 const& v) override { _scale = v; if(_auto_calc) CalcMatrix();  }

    inline void CalcMatrix() override {
        _matrix = ( glm::translate(glm::mat4(1.0f), _pos) * ( glm::rotate(glm::mat4(1.0f), _rot.x, glm::vec3(1,0,0)) * glm::rotate(glm::mat4(1.0f), _rot.y, glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1.0f), _rot.z, glm::vec3(0,0,1)) ) * glm::scale(glm::mat4(1.0f), _scale) );
    }

    void SetAutoCalcMatrix(bool const& s) override { _auto_calc = s; if(s) CalcMatrix(); }
    bool GetAutoCalcMatrix() override { return _auto_calc; }

    Transformable() {}
private:
    glm::vec3 _pos, _rot, _scale;
    glm::mat4 _matrix = glm::mat4(1.0f);
    bool _auto_calc = true;
};

class Transform {
public:
    mu::Event<Transform*, const glm::mat4&> OnChanged;

    void Calc();

    inline bool IsAutoRecalc(){ return _autoReCalc; }
    inline void SetAutoRecalc(const bool& state);

    void SetPos(const glm::vec3& p);
    void SetPos(glm::vec3* p);
    void SetRot(const glm::vec3& r);
    void SetRot(glm::vec3* r);
    void SetScale(const glm::vec3& s);
    void SetScale(glm::vec3* s);

    inline glm::vec3& GetPos(){ return _pos; }
    inline glm::vec3* GetPosP(){ return &_pos; }
    inline glm::vec3& GetRot(){ return _rot; }
    inline glm::vec3* GetRotP(){ return &_rot; }
    inline glm::vec3& GetScale(){ return _scale; }
    inline glm::vec3* GetScaleP(){ return &_scale; }
    inline glm::mat4& GetMat(){ return _mat; }
    inline glm::mat4* GetMatP(){ return &_mat; }

    Transform() : _mat(glm::mat4(1.0f)), _pos(glm::vec3(0.0f)), _rot(glm::vec3(0.0f)), _scale(glm::vec3(1.0f)), _autoReCalc(true) { Calc(); }
    Transform(glm::mat4 m) : _mat(m), _pos(glm::vec3(0.0f)), _rot(glm::vec3(0.0f)), _scale(glm::vec3(1.0f)), _autoReCalc(true) {}

    static glm::vec2 NormalizedDirection(float angle);
    static glm::vec3 NormalizedDirection(float angleX, float angleY, float angleZ);

    inline static glm::vec3 WorldForwardVector() { return glm::vec3(0,0,-1); }
    inline static glm::vec3 WorldBackwardVector() { return glm::vec3(0,0,1); }
    inline static glm::vec3 WorldUpVector() { return glm::vec3(0,1,0); }
    inline static glm::vec3 WorldDownVector() { return glm::vec3(0,-1,0); }
    inline static glm::vec3 WorldLeftVector() { return glm::vec3(-1,0,0); }
    inline static glm::vec3 WorldRightVector() { return glm::vec3(1,0,0); }
    inline static float CalcDist(const glm::vec3& v1, const glm::vec3& v2) { return glm::distance(v1, v2); }

protected:
    glm::mat4 _mat;
    glm::vec3 _pos;
    glm::vec3 _rot;
    glm::vec3 _scale;
    bool _autoReCalc;
};
}

#endif
