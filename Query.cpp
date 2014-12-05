#include "Query.hpp"
#include "MachineInfo.hpp"
#include "Utils/Log.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace mr {

void IQuery::Destroy() {
}

IQuery::~IQuery() {
    Destroy();
}

void Query::Begin() {
    if(_handle == 0) {
        glGenQueries(1, &_handle);
    }
    if(_target == Query::Target::TimeStamp) glQueryCounter(_handle, _target);
    else glBeginQuery(_target, _handle);

    _ended = false;
}

void Query::End() {
    if(_target != Query::Target::TimeStamp) glEndQuery(_target);
    _ended = true;
}

void Query::Destroy() {
    if(_handle != 0) {
        glDeleteQueries(1, &_handle);
        _handle = 0;
    }
}

bool Query::GetResult(void* dst) {
    if(_handle == 0) {
        mr::Log::LogString("Failed Query::GetResult(...). handle is null.", MR_LOG_LEVEL_WARNING);
        return false;
    }

    /*
    Zero every time. Not working. Fuck dat

    int av = -1;
    glGetQueryObjectiv(_handle, GL_QUERY_RESULT_AVAILABLE, &av);
    if(av == 0) {
        MR::Log::LogString("Failed Query::GetResult(...). Result isn't available.", MR_LOG_LEVEL_ERROR);
        return false;
    }*/

    switch(_target) {
    case Query::Target::AnySamplesPassed:
        glGetQueryObjectiv(_handle, GL_QUERY_RESULT, (int*)dst);
        return true;
        break;
    case Query::Target::AnySamplesPassedConservative:
        glGetQueryObjectiv(_handle, GL_QUERY_RESULT, (int*)dst);
        return true;
        break;
    case Query::Target::PrimitivesGenerated:
        glGetQueryObjectuiv(_handle, GL_QUERY_RESULT, (unsigned int*)dst);
        return true;
        break;
    case Query::Target::SamplesPassed:
        glGetQueryObjectuiv(_handle, GL_QUERY_RESULT, (unsigned int*)dst);
        return true;
        break;
    case Query::Target::TimeElapsed:
        glGetQueryObjectui64v(_handle, GL_QUERY_RESULT, (uint64_t*)dst);
        return true;
        break;
    case Query::Target::TimeStamp:
        glGetQueryObjectui64v(_handle, GL_QUERY_RESULT, (uint64_t*)dst);
        return true;
        break;
    case Query::Target::TransformFeedbackPrimitivesWritten:
        glGetQueryObjectuiv(_handle, GL_QUERY_RESULT, (unsigned int*)dst);
        return true;
        break;
    }

    mr::Log::LogString("Failed Query::GetResult(...). Unknown query target.", MR_LOG_LEVEL_ERROR);
    return false;
}

Query::Query(const Query::Target& target) : _handle(0), _target(target), _ended(true) {
    if(target == Query::Target::AnySamplesPassedConservative && mr::MachineInfo::gl_version_major() < 4) {
        _target = Query::Target::AnySamplesPassed;
    }
}

Query::~Query() {
}

Query Query::GetQuery(const Query::Target& target) {
    unsigned int handle = 0;
    glGetQueryiv(target, GL_CURRENT_QUERY, (int*)&handle);
    Query q(target);
    q._handle = handle;
    return q;
}

}
