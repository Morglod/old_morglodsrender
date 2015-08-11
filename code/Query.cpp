#include "Query.hpp"
#include "MachineInfo.hpp"
#include "Utils/Log.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace mr {

void Query::Begin() {
    unsigned int handle = 0;
    if((handle = GetGPUHandle()) == 0) {
        if(mr::gl::IsOpenGL45()) glCreateQueries(_target, 1, &handle);
        else glGenQueries(1, &handle);
        SetGPUHandle(handle);
    }
    if(_target == Query::Target::TimeStamp) glQueryCounter(handle, _target);
    else glBeginQuery(_target, handle);

    _ended = false;
}

void Query::End() {
    if(_target != Query::Target::TimeStamp) glEndQuery(_target);
    _ended = true;
}

void Query::Destroy() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        glDeleteQueries(1, &handle);
        SetGPUHandle(0);
    }
}

bool Query::GetResult(void* dst) {
    unsigned int handle = 0;
    if((handle = GetGPUHandle()) == 0) {
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
        glGetQueryObjectiv(handle, GL_QUERY_RESULT, (int*)dst);
        return true;
        break;
    case Query::Target::AnySamplesPassedConservative:
        glGetQueryObjectiv(handle, GL_QUERY_RESULT, (int*)dst);
        return true;
        break;
    case Query::Target::PrimitivesGenerated:
        glGetQueryObjectuiv(handle, GL_QUERY_RESULT, (unsigned int*)dst);
        return true;
        break;
    case Query::Target::SamplesPassed:
        glGetQueryObjectuiv(handle, GL_QUERY_RESULT, (unsigned int*)dst);
        return true;
        break;
    case Query::Target::TimeElapsed:
        glGetQueryObjectui64v(handle, GL_QUERY_RESULT, (uint64_t*)dst);
        return true;
        break;
    case Query::Target::TimeStamp:
        glGetQueryObjectui64v(handle, GL_QUERY_RESULT, (uint64_t*)dst);
        return true;
        break;
    case Query::Target::TransformFeedbackPrimitivesWritten:
        glGetQueryObjectuiv(handle, GL_QUERY_RESULT, (unsigned int*)dst);
        return true;
        break;
    }

    mr::Log::LogString("Failed Query::GetResult(...). Unknown query target.", MR_LOG_LEVEL_ERROR);
    return false;
}

Query::Query(const Query::Target& target) : _target(target), _ended(true) {
    if(target == Query::Target::AnySamplesPassedConservative) {
        if(mr::gl::GetMajorVersion() == 4)
            if(mr::gl::GetMinorVersion() < 3)
                _target = Query::Target::AnySamplesPassed;
        if(mr::gl::GetMajorVersion() < 4)
            _target = Query::Target::AnySamplesPassed;
    }
}

Query::~Query() {
}

Query Query::GetQuery(const Query::Target& target) {
    unsigned int handle = 0;
    glGetQueryiv(target, GL_CURRENT_QUERY, (int*)&handle);
    Query q(target);
    q.SetGPUHandle(handle);
    return q;
}

}
