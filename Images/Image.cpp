#include "Image.hpp"

#include "../Utils/Log.hpp"
#include <glm/gtx/compatibility.hpp>

namespace mr {

namespace {

void MixAddFunc(glm::ivec2 const& pos,
            glm::ivec2 const& mixImgPos,
            Image* to,
            IImage* mixWith,
            glm::ivec4& outColor)
{
    outColor = to->GetValue(pos) + mixWith->GetValue(mixImgPos);
}

void MixSubFunc(glm::ivec2 const& pos,
            glm::ivec2 const& mixImgPos,
            Image* to,
            IImage* mixWith,
            glm::ivec4& outColor)
{
    outColor = to->GetValue(pos) - mixWith->GetValue(mixImgPos);
}

void MixMulFunc(glm::ivec2 const& pos,
            glm::ivec2 const& mixImgPos,
            Image* to,
            IImage* mixWith,
            glm::ivec4& outColor)
{
    outColor = to->GetValue(pos) * mixWith->GetValue(mixImgPos);
}

void MixDivFunc(glm::ivec2 const& pos,
            glm::ivec2 const& mixImgPos,
            Image* to,
            IImage* mixWith,
            glm::ivec4& outColor)
{
    outColor = to->GetValue(pos) / mixWith->GetValue(mixImgPos);
}

void MixMeanFunc(glm::ivec2 const& pos,
            glm::ivec2 const& mixImgPos,
            Image* to,
            IImage* mixWith,
            glm::ivec4& outColor)
{
    outColor = (to->GetValue(pos) + mixWith->GetValue(mixImgPos)) / 2;
}

void MixPowFunc(glm::ivec2 const& pos,
            glm::ivec2 const& mixImgPos,
            Image* to,
            IImage* mixWith,
            glm::ivec4& outColor)
{
    outColor = glm::pow(to->GetValue(pos), mixWith->GetValue(mixImgPos));
}

void MixModFunc(glm::ivec2 const& pos,
            glm::ivec2 const& mixImgPos,
            Image* to,
            IImage* mixWith,
            glm::ivec4& outColor)
{
    outColor = glm::mod(to->GetValue(pos), mixWith->GetValue(mixImgPos));
}

void MixOverlapFunc(glm::ivec2 const& pos,
            glm::ivec2 const& mixImgPos,
            Image* to,
            IImage* mixWith,
            glm::ivec4& outColor)
{
    outColor = mixWith->GetValue(mixImgPos);
}

IImagePtr NearestNeighborScaleFunc(Image* from, glm::ivec2 const& dstSize) {
///http://tech-algorithm.com/articles/nearest-neighbor-image-scaling/
    Image* img = new Image();
    img->Create(dstSize, from->GetChannelsNum());

    int newXSize = dstSize.x;
    int newYSize = dstSize.y;

    int w1 = from->GetWidth();
    int h1 = from->GetHeight();

    int x_ratio = (int)((w1 << 16) / newXSize) + 1;
    int y_ratio = (int)((h1 << 16) / newYSize) + 1;

    int x2, y2;
    for(int i = 0; i < newYSize; ++i) {
        for(int j = 0; j < newXSize; ++j) {
            x2 = ((j * x_ratio) >> 16);
            y2 = ((i * y_ratio) >> 16);
            img->SetValue(glm::ivec2(j,i), from->GetValue(glm::ivec2(x2, y2)));
        }
    }
    return IImagePtr(static_cast<IImage*>(img));
}


void OperationSinFunc(glm::ivec4 const& color, glm::ivec4& outColor) {
    outColor = glm::ivec4(glm::sin((float)color.x),
                          glm::sin((float)color.y),
                          glm::sin((float)color.z),
                          glm::sin((float)color.w));
}

void OperationCosFunc(glm::ivec4 const& color, glm::ivec4& outColor) {
    outColor = glm::ivec4(glm::cos((float)color.x),
                          glm::cos((float)color.y),
                          glm::cos((float)color.z),
                          glm::cos((float)color.w));
}

void OperationSqrtFunc(glm::ivec4 const& color, glm::ivec4& outColor) {
    outColor = glm::ivec4(glm::sqrt((float)color.x),
                          glm::sqrt((float)color.y),
                          glm::sqrt((float)color.z),
                          glm::sqrt((float)color.w));
}

}

Image::MixFunc Image::mixFuncs[IImage::MixTypesNum] {
    MixAddFunc,
    MixSubFunc,
    MixMulFunc,
    MixDivFunc,
    MixMeanFunc,
    MixPowFunc,
    MixModFunc,
    MixOverlapFunc
};

Image::ResizeFunc Image::resizeFuncs[IImage::ResizeAlgosNum] {
    NearestNeighborScaleFunc
};

Image::OperationFunc Image::operationFuncs[OperationsNum] {
    OperationSinFunc,
    OperationCosFunc,
    OperationSqrtFunc
};

bool Image::Create(glm::ivec2 const& size, int channels, glm::ivec4 const& color) {
    if(size.x <= 0 || size.y <= 0 || channels <= 0) {
        mr::Log::LogString("Failed Image::Create. (size.x <= 0 || size.y <= 0 || channels <= 0).", MR_LOG_LEVEL_ERROR);
        return false;
    }
    size_t byteSize = size.x * size.y * channels;
    unsigned char* data = new unsigned char[byteSize];
    _data = mu::ArrayHandle<unsigned char>(data, byteSize, true, false);
    _size = size;
    _channels = channels;

    for(int ix = 0; ix < size.x; ++ix) {
        for(int iy = 0; iy < size.y; ++iy) {
            for(int i = 0; i < channels; ++i) {
                SetValue(glm::ivec2(ix, iy), i, color[i]);
            }
        }
    }

    return true;
}

bool Image::Create(glm::ivec2 const& size, int channels, unsigned char* data) {
    if(size.x <= 0 || size.y <= 0 || channels <= 0 || !data) {
        mr::Log::LogString("Failed Image::Create. (size.x <= 0 || size.y <= 0 || channels <= 0 || !data).", MR_LOG_LEVEL_ERROR);
        return false;
    }

    size_t byteSize = size.x * size.y * channels;
    _data = mu::ArrayHandle<unsigned char>(data, byteSize, true, false);
    _size = size;
    _channels = channels;

    return true;
}

void Image::DrawLine(glm::ivec2 const& from, glm::ivec2 const& to, glm::ivec4 const& color) {
///http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    glm::ivec2 d = to - from;

    int D = 2 * d.y - d.x;
    SetValue(from, color);
    int y = from.y;

    for(int x = from.x+1; x <= to.x; ++x) {
        if(D > 0) {
            ++y;
            SetValue(glm::ivec2(x,y), color);
            D += 2 * d.y - 2 * d.x;
        } else {
            SetValue(glm::ivec2(x,y), color);
            D += 2 * d.y;
        }
    }
}

void Image::DrawRect(glm::ivec2 const& upperLeft, glm::ivec2 bottomRight, glm::ivec4 const& color, bool fill) {
    if(fill) {
        for(int ix = upperLeft.x; ix <= bottomRight.x; ++ix) {
            for(int iy = upperLeft.y; iy <= bottomRight.y; ++iy) {
                SetValue(glm::ivec2(ix, iy), color);
            }
        }
    } else {
        for(int x = upperLeft.x; x <= bottomRight.x; ++x)
            SetValue(glm::ivec2(x, upperLeft.y), color);
        for(int x = upperLeft.x; x <= bottomRight.x; ++x)
            SetValue(glm::ivec2(x, bottomRight.y), color);
        for(int y = upperLeft.y; y <= bottomRight.y; ++y)
            SetValue(glm::ivec2(upperLeft.x, y), color);
        for(int y = upperLeft.y; y <= bottomRight.y; ++y)
            SetValue(glm::ivec2(bottomRight.x, y), color);
    }
}

void Image::Mix(glm::ivec2 const& from, IImage* what, MixType const& type) {
    for(int ix = from.x; ix < from.x + what->GetWidth(); ++ix) {
        for(int iy = from.y; iy < from.y + what->GetHeight(); ++iy) {
            glm::ivec4 color;
            const glm::ivec2 pos(ix,iy);
            mixFuncs[(int)type](pos, pos-from, this, what, color);
            SetValue(pos, color);
        }
    }
}

void Image::Mix(glm::ivec2 const& from, IImage* with, MixCustomFunc func) {
    for(int ix = from.x; ix < from.x + GetWidth(); ++ix) {
        for(int iy = from.y; iy < from.y + GetHeight(); ++iy) {
            const glm::ivec2 pos(ix,iy);
            SetValue(pos, func(GetValue(pos), with->GetValue(pos-from)));
        }
    }
}

void Image::Operation(glm::ivec2 const& from, OperationType const& type) {
    for(int ix = from.x; ix < GetWidth(); ++ix) {
        for(int iy = from.y; iy < GetHeight(); ++iy) {
            const glm::ivec2 pos(ix,iy);
            glm::ivec4 color;
            operationFuncs[(int)type](GetValue(pos), color);
            SetValue(pos, color);
        }
    }
}

void Image::Operation(glm::ivec2 const& from, OperationCustomFunc func) {
    for(int ix = from.x; ix < GetWidth(); ++ix) {
        for(int iy = from.y; iy < GetHeight(); ++iy) {
            const glm::ivec2 pos(ix,iy);
            SetValue(pos, func(GetValue(pos)));
        }
    }
}

void Image::Invert() {
    for(int ix = 0; ix < _size.x; ++ix) {
        for(int iy = 0; iy < _size.y; ++iy) {
            const glm::ivec2 pos(ix,iy);
            const glm::vec4 col = (((glm::vec4)GetValue(pos)) / 255.0f);
            SetValue(pos, (glm::ivec4)((1.0f - col) * 255.0f));
        }
    }
}

void Image::Lerp(glm::ivec2 const& from, IImage* with, float factor) {
    for(int ix = from.x; ix < from.x+with->GetWidth(); ++ix) {
        for(int iy = from.y; iy < from.x+with->GetHeight(); ++iy) {
            const glm::ivec2 pos(ix,iy);
            SetValue(pos, (glm::ivec4)glm::lerp((glm::vec4)GetValue(pos), (glm::vec4)(with->GetValue(pos-from)), factor));
        }
    }
}

IImagePtr Image::Resize(glm::ivec2 const& dstSize, ResizeAlgo const& algo) {
    return resizeFuncs[(int)algo](this, dstSize);
}

Image::Image() : _data(), _size(0,0), _channels(0) {
}

Image::~Image() {
}

}
