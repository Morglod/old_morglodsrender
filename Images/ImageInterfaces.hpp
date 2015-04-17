#pragma once

#include <Containers.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <functional>

namespace mr {

class IImage;
typedef std::shared_ptr<IImage> IImagePtr;

class IImage {
public:
    enum MixType : char {
        MixAdd = 0,
        MixSub = 1,
        MixMultiply,
        MixDevide,
        MixMean, //среднее
        MixPow,
        MixMod,
        MixOverlap,
        MixTypesNum
    };

    typedef std::function<glm::ivec4 (glm::ivec4 const& color1, glm::ivec4 const& color2)> MixCustomFunc;

    enum ResizeAlgo : char {
        NearestNeighborScale = 0,
        ResizeAlgosNum
    };

    enum OperationType : char {
        OperationSin = 0,
        OperationCos = 1,
        OperationSqrt,
        OperationsNum
    };
    typedef std::function<glm::ivec4 (glm::ivec4 const& color)> OperationCustomFunc;

    virtual bool Create(glm::ivec2 const& size, int channels, glm::ivec4 const& color = glm::ivec4(255, 0, 0, 255)) = 0;
    virtual bool Create(glm::ivec2 const& size, int channels, unsigned char* data) = 0;

    virtual mu::ArrayHandle<unsigned char> GetData() const = 0;
    virtual mu::ArrayRef<unsigned char> GetDataRef() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual glm::ivec2 GetSize() const = 0;
    virtual int GetChannelsNum() const = 0;
    virtual unsigned char GetValue(glm::ivec2 const& pos, int const& channel) const = 0;
    virtual glm::ivec4 GetValue(glm::ivec2 const& pos) const = 0;
    virtual void SetValue(glm::ivec2 const& pos, int const& channel, unsigned char value) = 0;
    virtual void SetValue(glm::ivec2 const& pos, glm::ivec4 const& color) = 0;

    virtual void DrawLine(glm::ivec2 const& from, glm::ivec2 const& to, glm::ivec4 const& color) = 0;
    virtual void DrawRect(glm::ivec2 const& upperLeft, glm::ivec2 bottomRight, glm::ivec4 const& color, bool fill) = 0;

    virtual void Mix(glm::ivec2 const& from, IImage* with, MixType const& type) = 0;
    virtual void Mix(glm::ivec2 const& from, IImage* with, MixCustomFunc func) = 0;
    virtual void Operation(glm::ivec2 const& from, OperationType const& type) = 0;
    virtual void Operation(glm::ivec2 const& from, OperationCustomFunc func) = 0;
    virtual void Invert() = 0;
    virtual IImagePtr Resize(glm::ivec2 const& dstSize, ResizeAlgo const& algo) = 0;
    virtual void Lerp(glm::ivec2 const& from, IImage* with, float factor) = 0;

    virtual ~IImage() {}
};

class IImageLoader {
public:
    struct Options {
        bool greyscale = false; //false - rgb / true - grey
        bool alpha = true; //false - rgb or grey / true - rgbA or greyA
    };

    //TODO: Load cubemaps
    virtual IImagePtr Load(std::string const& path, Options const& options) = 0;
    virtual IImagePtr Load(unsigned char* memory, size_t const& memSize, Options const& options) = 0;
    virtual ~IImageLoader() {}
};

class IImageSaver {
public:
    struct Options {
        enum Type : int {
            TGA = 0,
            BMP = 1,
            DDS = 2
        };
        Type fileType = TGA;
    };

    virtual bool Save(std::string const& path, Options const& options, const IImage* const image) = 0;
    virtual ~IImageSaver() {}
};

//”казатель на эти интерфейсы передать в TextureManager

}
