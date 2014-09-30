#include "TextureInterfaces.hpp"

namespace MR {

TextureSizeInfo::TextureSizeInfo() : width(0), height(0), depth(0) {}
TextureSizeInfo::TextureSizeInfo(unsigned short const& w, unsigned short const& h, unsigned short const& d)
    : width(w), height(h), depth(d) {}
TextureSizeInfo::TextureSizeInfo(glm::ivec3 const& s)
    : width(s.x), height(s.y), depth(s.z) {}

TextureBitsInfo::TextureBitsInfo() : r(0), g(0), b(0), d(0), a(0) {}
TextureBitsInfo::TextureBitsInfo(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char d_, unsigned char a_)
    : r(r_), g(g_), b(b_), d(d_), a(a_) {}

}
