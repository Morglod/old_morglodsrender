#include "mr/cmd/cmd.hpp"

namespace mr {

void CmdQueue::InsertCmd(Cmd& cmd) {
    _cmds.push_back(cmd);
}

void CmdQueue::PopCmd(Cmd* to) {
    memcpy(to, &_cmds[0], sizeof(Cmd));
    _cmds.pop_front();
}

void CmdQueue::_ExecUnsafe(Cmd* cmd) {
    CmdHeader* header = (CmdHeader*)cmd;
    switch(header->objectType) {
    default:
    case CmdObjectType::Num:
    case CmdObjectType::None:{

    }break;
    case CmdObjectType::Buffer:{
        switch(header->cmdType) {
        default:
        case CmdType::Num:
        case CmdType::None:{
        }break;
        case CmdType::BufferMap:{
            Cmd_BufferMap* cmd_bm = (Cmd_BufferMap*)cmd;
            Buffer::_Map(cmd_bm->targetObject, cmd_bm->offset, cmd_bm->length, cmd_bm->flags);
            (*cmd_bm->out) = cmd_bm->targetObject->GetMapState();
        }break;
        }
    }break;
    case CmdObjectType::Texture:{

    }break;
    }
}

}
