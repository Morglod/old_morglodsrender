#pragma once

#include "ImageInterfaces.hpp"

namespace mr {

/**
    upper left pixel is 0,0
    bottom right pixel is imagesize-1
**/
class Image : public IImage {
public:
    bool Create(glm::ivec2 const& size, int channels, glm::ivec4 const& color = glm::ivec4(255, 0, 0, 255)) override;
    bool Create(glm::ivec2 const& size, int channels, unsigned char* data) override;

    void DrawLine(glm::ivec2 const& from, glm::ivec2 const& to, glm::ivec4 const& color) override;
    void DrawRect(glm::ivec2 const& upperLeft, glm::ivec2 bottomRight, glm::ivec4 const& color, bool fill) override;

    void Mix(glm::ivec2 const& from, IImage* with, MixType const& type) override;
    void Mix(glm::ivec2 const& from, IImage* with, MixCustomFunc func) override;
    void Operation(glm::ivec2 const& from, OperationType const& type) override;
    void Operation(glm::ivec2 const& from, OperationCustomFunc func) override;
    void Invert() override;
    IImagePtr Resize(glm::ivec2 const& dstSize, ResizeAlgo const& algo) override;
    void Lerp(glm::ivec2 const& from, IImage* with, float factor) override;

    inline mu::ArrayHandle<unsigned char> GetData() const override { return mu::ArrayHandle<unsigned char>(_data.GetArray(), _data.GetNum(), true, true); }
    inline mu::ArrayRef<unsigned char> GetDataRef() const override { return _data.ToRef(); }
    inline int GetWidth() const override { return _size.x; }
    inline int GetHeight() const override { return _size.y; }
    inline glm::ivec2 GetSize() const override { return _size; }
    inline int GetChannelsNum() const override { return _channels; }
    inline unsigned char GetValue(glm::ivec2 const& pos, int const& channel) const override { return _data.GetArray()[pos.y * _channels * _size.x + _channels * pos.x + channel]; }
    inline void SetValue(glm::ivec2 const& pos, int const& channel, unsigned char value) override { _data.GetArray()[pos.y * _channels * _size.x + _channels * pos.x + channel] = value; }

    inline glm::ivec4 GetValue(glm::ivec2 const& pos) const override {
        glm::ivec4 out;
        for(int i = 0; i < GetChannelsNum(); ++i) {
            out[i] = (int)GetValue(pos, i);
        }
        return out;
    }

    inline void SetValue(glm::ivec2 const& pos, glm::ivec4 const& color) override {
        for(int i = 0; i < GetChannelsNum(); ++i) {
            SetValue(pos, i, color[i]);
        }
    }

    Image();
    virtual ~Image();
protected:
    mu::ArrayHandle<unsigned char> _data;
    glm::ivec2 _size;
    int _channels;

    typedef void (*MixFunc)(glm::ivec2 const& pos,
                            glm::ivec2 const& mixImgPos,
                            Image* to,
                            IImage* mixWith,
                            glm::ivec4& outColor);
    static MixFunc mixFuncs[MixTypesNum];

    typedef IImagePtr (*ResizeFunc)(Image* from, glm::ivec2 const& dstSize);
    static ResizeFunc resizeFuncs[ResizeAlgosNum];

    typedef void (*OperationFunc)(glm::ivec4 const& color, glm::ivec4& outColor);
    static OperationFunc operationFuncs[OperationsNum];
};

}
