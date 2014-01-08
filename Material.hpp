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
    Texture* diffuseTexture;
    GLenum diffuseTextureStage; //GL_TEXTURE0/GL_TEXTURE1/GL_TEXTURE2 etc

    Shader* shader; //pointer to shader

    void Use();

    MaterialPass() : diffuseTexture(nullptr), diffuseTextureStage(GL_TEXTURE0), shader(nullptr) {}
    MaterialPass(Texture* dTex, GLenum dTexStage, Shader* sh) : diffuseTexture(dTex), diffuseTextureStage(dTexStage), shader(sh) {}
};

class Material {
public:
    std::string name = "noname";
    std::vector<MaterialPass*> materialPasses;
};

}

#endif
