#include "GeometryStreams.hpp"
#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"

#include <vector>
#include <GL/glew.h>

std::vector<MR::GeometryStream> __MR_GEOMETRY_STREAMS_LIST_;

namespace MR {

void GeometryStream::BindVertexFormat(IVertexFormat* format) {
    IVertexAttribute** attr = 0;
    size_t num = format->_Attributes(&attr);

    for(size_t i = 0; i < num; ++i) {
        glVertexAttribBinding(attr[i]->ShaderIndex(), _id);
    }
}

GeometryStream* GeometryStream::Get(const unsigned int& id) {
    if(id >= GeometryStream::GetMax()) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
        MR::Log::LogString("Failed getting ("+std::to_string(id)+") geometry stream. It's out of bounds.");
#endif
        return nullptr;
    }
    //Create new stream
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
