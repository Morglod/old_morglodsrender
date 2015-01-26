#include "FrameBufferInterfaces.hpp"
#include <map>

std::map<mr::IFrameBuffer::CompletionStatus, std::string> _MR_FB_COMPLETION_ST_TO_STIRNG_ {
    std::pair<mr::IFrameBuffer::CompletionStatus, std::string>(mr::IFrameBuffer::Complete, "Complete"),
    std::pair<mr::IFrameBuffer::CompletionStatus, std::string>(mr::IFrameBuffer::IncompleteAttachment, "Incomplete attachment"),
    std::pair<mr::IFrameBuffer::CompletionStatus, std::string>(mr::IFrameBuffer::IncompleteMissingAttachment, "Incomplete missing attachment"),
    std::pair<mr::IFrameBuffer::CompletionStatus, std::string>(mr::IFrameBuffer::IncompleteDrawBuffer, "Incomplete draw buffer"),
    std::pair<mr::IFrameBuffer::CompletionStatus, std::string>(mr::IFrameBuffer::IncompleteReadBuffer, "Incomplete read buffer"),
    std::pair<mr::IFrameBuffer::CompletionStatus, std::string>(mr::IFrameBuffer::Unsupported, "Unsupported"),
    std::pair<mr::IFrameBuffer::CompletionStatus, std::string>(mr::IFrameBuffer::IncompleteMultisample, "Incomplete multisample"),
    std::pair<mr::IFrameBuffer::CompletionStatus, std::string>(mr::IFrameBuffer::IncompleteLayerTargets, "Incomplete layer targets")
};

namespace mr {

std::string IFrameBuffer::CompletionStatusToString(CompletionStatus const& cs) {
    return _MR_FB_COMPLETION_ST_TO_STIRNG_[cs];
}

}
