/* ---------------

    WIP

---------------- */

#pragma once

#ifndef _MR_TEXTURE_GENERATOR_H_

#include "../Utils/Singleton.hpp"
#include "../Utils/Containers.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace MR {

class ITexture;

class ProcedureTexture;
typedef std::shared_ptr<ProcedureTexture> ProcedureTexturePtr;

class TextureLayer {
public:
    enum Operation {
        O_Add = 0,
        O_Mult = 1,
        O_Sub,
        O_Div,
        O_Mix,
        O_Sin,
        O_Custom //override CustomOperation method
    };

    //Image data will be used as background or as foreground
    enum ImageDataLayering {
        IDL_Over,
        IDL_Under
    };

    //May be null
    inline virtual glm::vec4* GetImageData() { return _imageData; }

    //Should be in size equal to parent ProcedureTexture sizes
    inline virtual void SetImageData(glm::vec4* imgData) { _imageData = imgData; }

    inline virtual glm::vec4 CustomOperation(const glm::vec4& pixel, const glm::vec4& thisLayerPixel, const glm::ivec2& pixelCoords) { return pixel + thisLayerPixel; }
    inline virtual ProcedureTexturePtr GetParent() { return _parent; }

    inline virtual void SetOperation(const Operation& o) { _operation = o; }
    inline virtual Operation GetOperation() { return _operation; }

    inline virtual void SetImageDataOperation(const Operation& o) { _imageData_operation = o; }
    inline virtual Operation GetImageDataOperation() { return _imageData_operation; }

    inline virtual void SetImageDataLayering(const Operation& o) { _imageData_layering = o; }
    inline virtual ImageDataLayering GetImageDataLayering() { return _imageData_layering; }

    inline virtual void SetFillColor(const glm::vec4& color) { _fillColor = color; }
    inline virtual glm::vec4 GetFillColor() { return _fillColor; }

    TextureLayer();
    virtual ~TextureLayer();
protected:
    glm::vec4* _imageData;
    ProcedureTexturePtr _parent;
    Operation _operation, _imageData_operation;
    ImageDataLayering _imageData_layering;
    glm::vec4 _fillColor;
};

typedef std::shared_ptr<TextureLayer> TextureLayerPtr;

class ProcedureTexture {
public:
    inline virtual StaticArray<TextureLayerPtr> GetLayers() { return StaticArray<TextureLayerPtr>(&_layers[0], _layers.size(), false); }
    inline virtual void AddLayer(TextureLayerPtr layer) { _layers.push_back(layer); }
    virtual TextureLayerPtr CreateLayer(const glm::vec4& fillColor);
    virtual void DeleteLayer(TextureLayerPtr layer);
    virtual void DeleteLayer(const size_t& i);

    virtual ITexture* Generate();

    ProcedureTexture();
    virtual ~ProcedureTexture();
protected:
    std::vector<TextureLayerPtr> _layers;
};

class TextureGenerator : public Singleton<TextureGenerator> {
public:
    ProcedureTexturePtr CreateProcedureTexture();

    TextureGenerator();
    ~TextureGenerator();
};

}

#endif // _MR_TEXTURE_GENERATOR_H_
