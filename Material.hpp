#pragma once

#ifndef _MR_MATERIAL_H_
#define _MR_MATERIAL_H_

#include "MorglodsRender.hpp"

namespace MR {
class Shader;
class Texture;

inline float* WhiteColor() {
    return new float[4] {1.0f, 1.0f, 1.0f, 1.0f};
}
inline float* BlackColor() {
    return new float[4] {0.0f, 0.0f, 0.0f, 1.0f};
}
inline float* RedColor() {
    return new float[4] {1.0f, 0.0f, 0.0f, 1.0f};
}
inline float* GreenColor() {
    return new float[4] {0.0f, 1.0f, 0.0f, 1.0f};
}
inline float* BlueColor() {
    return new float[4] {0.0f, 0.0f, 1.0f, 1.0f};
}

class MaterialPass {
public:
    float* ambientColor; //rgba or float[4] glColor4f
    Texture* ambientTexture;
    GLenum ambientTextureStage; //GL_TEXTURE0/GL_TEXTURE1/GL_TEXTURE2 etc
    Shader* shader; //pointer to shader

    void Use();

    MaterialPass() : ambientColor(nullptr), ambientTexture(nullptr), ambientTextureStage(GL_TEXTURE0), shader(nullptr) {}
    MaterialPass(float amColor[4], Texture* amTex, GLenum amTexStage, Shader* sh) : ambientColor(amColor), ambientTexture(amTex), ambientTextureStage(amTexStage), shader(sh) {}
};

class Material {
public:
    std::string name = "noname";
    std::vector<MaterialPass*> materialPasses;
};
}

#endif
