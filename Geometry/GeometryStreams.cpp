#include "GeometryStreams.hpp"
#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"
#include "../Config.hpp"

#include <vector>
#include <GL/glew.h>

std::vector<MR::GeometryStream> __MR_GEOMETRY_STREAMS_LIST_;

namespace MR {

void GeometryStream::BindVertexFormat(IVertexFormat* format) {
    if(!format) {
        MR::Log::LogString("Failed GeometryStream::BindVertexFormat(null). format is null.", MR_LOG_LEVEL_ERROR);
        return;
    }
    StaticArray<IVertexAttribute*> attr = format->_Attributes();

    for(size_t i = 0; i < attr.GetNum(); ++i) {
        if(!attr.At(i)) MR::Log::LogString("Failed GeometryStream::BindVertexFormat(...). format->Attribute["+std::to_string(i)+"] is null.", MR_LOG_LEVEL_ERROR);
        glVertexAttribBinding(attr.At(i)->ShaderIndex(), _id);
    }
}

GeometryStream* GeometryStream::Get(const unsigned int& id) {
    if(id >= GeometryStream::GetMax()) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
        MR::Log::LogString("Failed getting ("+std::to_string(id)+") geometry stream. It's out of bounds.");
#endif
        return nullptr;
    }
    //Create new streams if needed
    while(id >= __MR_GEOMETRY_STREAMS_LIST_.size()) {
        __MR_GEOMETRY_STREAMS_LIST_.push_back(GeometryStream());
        __MR_GEOMETRY_STREAMS_LIST_[__MR_GEOMETRY_STREAMS_LIST_.size()-1]._id = (unsigned int) (__MR_GEOMETRY_STREAMS_LIST_.size()-1);
    }
    return &__MR_GEOMETRY_STREAMS_LIST_[id];
}

unsigned int GeometryStream::GetMax() {
    return (unsigned int)MR::MachineInfo::GetGeometryStreamsNum();
}

GeometryStream::GeometryStream() : _id(0), _f(nullptr) {
}

GeometryStream::~GeometryStream() {}

}
