#include "mr/vformat.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/log.hpp"
#include "mr/gl/types.hpp"
#include "src/mp.hpp"
#include "mr/pre/glew.hpp"
#include "src/statecache.hpp"
#include "mr/alloc.hpp"

#include <unordered_map>

namespace mr {

VertexDecl::Changer& VertexDecl::Changer::Pos(PosDataType const& type, uint8_t bindpoint) {
    MP_BeginSample(VertexDecl::Changer::Pos);

    Push(bindpoint, (uint32_t)type, 3, false, false, ShaderLocation_Pos);

    MP_EndSample();

    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Color(ColorDataType const& type, uint8_t bindpoint) {
    MP_BeginSample(VertexDecl::Changer::Color);

    Push(bindpoint, (uint32_t)type, 4, true, false, ShaderLocation_Color);

    MP_EndSample();

    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Normal(PosDataType const& type, uint8_t bindpoint) {
    MP_BeginSample(VertexDecl::Changer::Normal);

    Push(bindpoint, (uint32_t)type, 3, true, false, ShaderLocation_Normal);

    MP_EndSample();

    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::TexCoord(uint8_t bindpoint) {
    MP_BeginSample(VertexDecl::Changer::TexCoord);

    Push(bindpoint, (uint32_t)PosDataType::Float, 2, true, false, ShaderLocation_TexCoord);

    MP_EndSample();

    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Data(uint8_t sz, uint8_t bindpoint) {
    MP_BeginSample(VertexDecl::Changer::Data);

    Push(bindpoint, GL_BYTE, sz, false, true, 0);

    MP_EndSample();

    return *this;
}

VertexDecl::Changer& VertexDecl::Changer::Custom(DataType const& type, uint8_t components_num, uint8_t shaderLocation, uint8_t bindpoint, bool normalized) {
    MP_BeginSample(VertexDecl::Changer::Custom);

    Push(bindpoint, (uint32_t)type, components_num, normalized, false, shaderLocation);

    MP_EndSample();
    return *this;
}

void VertexDecl::Changer::End() {
    MP_BeginSample(VertexDecl::Changer::End);

    int32_t attributesNum = 0;
    for(std::pair<uint8_t, BindPointDesc> const& src_attribs : bindpoints)
        attributesNum += src_attribs.second.attribs.size();

    // Write changes
    // src - VertexDecl::Changer declaration
    // dst - VertexDecl declaration

    decl._size = 0;
    decl._map.Resize(attributesNum);

    int iattributes = 0;

    for(std::pair<uint8_t, BindPointDesc> const& src_attribs : bindpoints) {
        for(uint8_t i = 0, n = src_attribs.second.attribs.size(); i < n; ++i) {
            Attrib src_attrib = src_attribs.second.attribs[i];
            src_attrib.bindpoint = src_attribs.first;
            src_attrib.stride = src_attribs.second.offset;
            decl._map.attribs[iattributes++] = src_attrib;
            decl._size += src_attrib.size;
        }
    }

    MP_EndSample();
}

void VertexDecl::Changer::Push(uint8_t bindpoint_index, uint32_t gl_dt, uint8_t comp_num, bool norm, bool offsetOnly, uint8_t shaderLocation) {
    MP_ScopeSample(VertexDecl::Changer::Push);

    const uint32_t dt_size = (gl_dt != GL_HALF_FLOAT) ? sizeof_gl(gl_dt) : sizeof(float);
    const uint8_t attrib_size = dt_size * comp_num;

    BindPointDesc& bindpoint = bindpoints[bindpoint_index];

    if(offsetOnly) {
        bindpoint.offset += attrib_size;
        MP_EndSample();
        return;
    }

    if(shaderLocation == -1) {
        shaderLocation = bindpoint.attribi;
    }

    Attrib attrib;
    attrib.location = shaderLocation;
    attrib.offset = bindpoint.offset;
    attrib.components_num = comp_num;
    attrib.datatype = gl_dt;
    attrib.normalized = norm;
    attrib.size = attrib_size;

    bindpoint.attribs.push_back(attrib);
    if(bindpoint.attribi <= shaderLocation) (bindpoint.attribi = shaderLocation+1);
    bindpoint.offset += attrib_size;
}

VertexDecl::Changer::Changer(VertexDecl& d) : decl(d) {
}

VertexDecl::Changer VertexDecl::Begin() {
    return VertexDecl::Changer(*this);
}

bool VertexDecl::Bind() {
    if(!StateCache::Get()->SetVertexDecl(this)) return true;

    MP_BeginSample(VertexDecl::Bind);

    // Reset vertex attribs state

    static int attribsNum = 0;
    static int lastBoundAttrib = 0;
    if(attribsNum == 0) {
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attribsNum);
        for(int i = 0; i < attribsNum; ++i)
            glEnableVertexAttribArray(/*attrib.index*/i);
    }

    int maxBoundAttrib = 0;
    for(uint8_t i = 0, n = _map.num; i < n; ++i) {
        Attrib const& attrib = _map.attribs[i];
        if(attrib.location >= maxBoundAttrib)
            glEnableVertexAttribArray(attrib.location);
        if(GLEW_NV_vertex_buffer_unified_memory) {
            glVertexAttribFormatNV(attrib.location, attrib.components_num, attrib.datatype, attrib.normalized, attrib.stride);
        }
        else {
            glVertexAttribFormat(attrib.location, attrib.components_num, attrib.datatype, attrib.normalized, attrib.offset);
            glVertexAttribBinding(attrib.location, attrib.bindpoint);
        }
        if(attrib.location > maxBoundAttrib) maxBoundAttrib = attrib.location;
    }

    for(int i = 0; i < maxBoundAttrib - lastBoundAttrib; ++i)
        glDisableVertexAttribArray(lastBoundAttrib+i);
    lastBoundAttrib = maxBoundAttrib;

    MP_EndSample();

    return true;
}

void VertexDecl::AttribMap::Resize(uint8_t n) {
    MP_BeginSample(VertexDecl::AttribMap::Resize);

    Free();
    attribs = MR_NEW_ARRAY(Attrib, n);
    num = n;

    MP_EndSample();
}

void VertexDecl::AttribMap::Free() {
    MP_ScopeSample(VertexDecl::AttribMap::Free);

    if(attribs != nullptr) {
        delete [] attribs;
        attribs = nullptr;
    }
    num = 0;
}

VertexDecl::AttribMap::~AttribMap() {
    Free();
}

VertexDeclPtr VertexDecl::Create() {
    return MR_NEW_SHARED(VertexDecl);
}

}
