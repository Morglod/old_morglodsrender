#include "RenderManager.hpp"

#include "MachineInfo.hpp"

#include "Geometry/GeometryInterfaces.hpp"
#include "Textures/TextureInterfaces.hpp"
#include "RenderTarget.hpp"
#include "Shaders/ShaderInterfaces.hpp"
#include "Shaders/ShaderObjects.hpp"
#include "Utils/Log.hpp"
#include "Geometry/GeometryBufferV2.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

void MR::RenderManager::SetVertexFormat(MR::IVertexFormat* f) {
    if(_vformat != f) {
        if(_vformat) _vformat->UnBind();
        if(f) f->Bind();
        _vformat = f;
    }
}

void MR::RenderManager::SetIndexFormat(MR::IIndexFormat* f) {
    if(_iformat != f) {
        if(_iformat) _iformat->UnBind();
        if(f) f->Bind();
        _iformat = f;
    }
}

void MR::RenderManager::DrawGeometry(IGeometry* g) {
    if(!g) {
        MR::Log::LogString("Failed RenderManager::DrawGeometry(null).", MR_LOG_LEVEL_ERROR);
        return;
    }
    this->DrawGeometryBuffer(g->GetGeometryBuffer(), g->GetDrawParams());
}

void MR::RenderManager::DrawGeometryBuffer(IGeometryBuffer* b, IGeometryDrawParamsPtr drawParams) {
    if(!b) {
        MR::Log::LogString("Failed RenderManager::DrawGeometryBuffer(null, ...) No GeometryBuffer.", MR_LOG_LEVEL_ERROR);
        return;
    }
    if(!drawParams) {
        MR::Log::LogString("Failed RenderManager::DrawGeometryBuffer(..., null). No DrawParams.", MR_LOG_LEVEL_ERROR);
        return;
    }

    void* drawCmd = drawParams->GetIndirectPtr();
    if(MR::MachineInfo::IsIndirectDrawSupported()) {
        if(!drawCmd && !MR::MachineInfo::IndirectDraw_UseGPUBuffer()) {
            MR::Log::LogString("RenderManager::DrawGeometryBuffer(...) drawCmd is null and not an offset (Feautre_DrawIndirect_UseGPUBuffer == false).", MR_LOG_LEVEL_ERROR);
        }
    } else if(drawCmd) {
        MR::Log::LogString("RenderManager::DrawGeometryBuffer(...). Feature_DrawIndirect is disabled but drawCmd is not null.", MR_LOG_LEVEL_WARNING);
    }

    drawParams->BeginDraw();

    if(MR::MachineInfo::IsNVVBUMSupported()) {
        SetVAO(0);
        SetVertexFormat(b->GetVertexFormat());
        if(!_vformat) {
            MR::Log::LogString("Failed RenderManager::DrawGeometryBuffer(...). No VertexFormat.", MR_LOG_LEVEL_ERROR);
            return;
        }

        TStaticArray<IVertexAttribute*> attrs = _vformat->_Attributes();
        TStaticArray<uint64_t> ptrs = _vformat->_Offsets();

        MR::IGPUGeometryBuffer* buff = b->GetVertexBuffer();
        uint64_t nv_res_ptr = 0;
        int nv_buf_size = 0;
        buff->GetNVGPUPTR(&nv_res_ptr, &nv_buf_size);

        if(!buff) {
            MR::Log::LogString("Failed RenderManager::DrawGeometryBuffer(...). No VertexBuffer.", MR_LOG_LEVEL_ERROR);
            return;
        }

        for(size_t i = 0; i < attrs.GetNum(); ++i) {
            glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                   attrs.At(i)->ShaderIndex(),
                                   nv_res_ptr + ptrs.At(i),
                                   nv_buf_size
                                   );
        }

        if((buff = b->GetIndexBuffer()) != nullptr && drawParams->GetUseIndexBuffer()){
            buff->GetNVGPUPTR(&nv_res_ptr, &nv_buf_size);
            SetIndexFormat(b->GetIndexFormat());
            if(!_iformat) {
                MR::Log::LogString("Failed RenderManager::DrawGeometryBuffer(...). No IndexFormat.", MR_LOG_LEVEL_ERROR);
            }
            glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, nv_res_ptr, nv_buf_size);
            if(MR::MachineInfo::IsIndirectDrawSupported()) glDrawElementsIndirect(b->GetDrawMode(), _iformat->GetDataType()->GPUDataType(), drawCmd);
            else {
                /*glDrawRangeElements(b->GetDrawMode(),
                                    drawParams->GetVertexStart(),
                                    drawParams->GetVertexStart()+drawParams->GetVertexCount(),
                                    drawParams->GetIndexCount(),
                                    _iformat->GetDataType()->GPUDataType(),
                                    (void*)(_iformat->Size() * drawParams->GetIndexStart()));*/
                glDrawElementsBaseVertex(  b->GetDrawMode(),
                                            drawParams->GetIndexCount(),
                                            _iformat->GetDataType()->GPUDataType(),
                                            (void*)(_iformat->Size() * drawParams->GetIndexStart()),
                                            drawParams->GetVertexStart());
            }
        }
        else {
            if(MR::MachineInfo::IsIndirectDrawSupported()) glDrawArraysIndirect(b->GetDrawMode(), drawCmd);
            else glDrawArrays(b->GetDrawMode(), drawParams->GetVertexStart(), drawParams->GetVertexCount());
        }
    }
    else {
        SetVAO(b->GetVAO());
        IGPUGeometryBuffer* ibuff = b->GetIndexBuffer();
        MR::IIndexFormat * iform = nullptr;

        if(_vao == 0) {
            IGPUGeometryBuffer* buff = b->GetVertexBuffer();
            if(!buff) {
                MR::Log::LogString("Failed RenderManager::DrawGeometryBuffer(...). No VertexBuffer.", MR_LOG_LEVEL_ERROR);
                return;
            }
            glBindBuffer(GL_ARRAY_BUFFER, buff->GetGPUBuffer()->GetGPUHandle());

            SetVertexFormat(b->GetVertexFormat());
            if(!_vformat) {
                MR::Log::LogString("Failed RenderManager::DrawGeometryBuffer(...). No VertexFormat.", MR_LOG_LEVEL_ERROR);
                return;
            }

            if(ibuff && drawParams->GetUseIndexBuffer()) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuff->GetGPUBuffer()->GetGPUHandle());
                SetIndexFormat(b->GetIndexFormat());
                if(!_iformat) {
                    MR::Log::LogString("Failed RenderManager::DrawGeometryBuffer(...). No IndexFormat.", MR_LOG_LEVEL_ERROR);
                    return;
                }
                iform = _iformat;
            }
        } else {
            iform = b->GetIndexFormat();
        }

        if(ibuff && drawParams->GetUseIndexBuffer()) {
            if(MR::MachineInfo::IsIndirectDrawSupported()) {
                glDrawElementsIndirect(b->GetDrawMode(), iform->GetDataType()->GPUDataType(), drawCmd);
            }
            else {
                glDrawElementsBaseVertex(  b->GetDrawMode(),
                                            drawParams->GetIndexCount(),
                                            iform->GetDataType()->GPUDataType(),
                                            (void*)(iform->Size() * drawParams->GetIndexStart()),
                                            drawParams->GetVertexStart());
            }
        }
        else {
            if(MR::MachineInfo::IsIndirectDrawSupported()) glDrawArraysIndirect(b->GetDrawMode(), drawCmd);
            else glDrawArrays(b->GetDrawMode(), drawParams->GetVertexStart(), drawParams->GetVertexCount());
        }

        SetVAO(0);
    }

    drawParams->EndDraw();
}

void MR::RenderManager::SetVAO(const unsigned int& vao) {
    if(_vao != vao) {
        glBindVertexArray(vao);
        _vao = vao;
    }
}

void MR::RenderManager::SetIndirectDrawParamsBuffer(const unsigned int& buf) {
    if(_indirect_drawParams_buffer != buf) {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buf);
        _indirect_drawParams_buffer = buf;
    }
}

void MR::RenderManager::SetActivedMaterialPass(MR::IMaterialPass* p) {
    _pass = p;
}

bool MR::RenderManager::SetTexture(MR::ITexture* t, const int& unit) {
    if(unit > MR::MachineInfo::MaxActivedTextureUnits()) {
        MR::Log::LogString("Failed RenderManager::SetTexture(...). unit ("+std::to_string(unit)+") > " + std::to_string(MR::MachineInfo::MaxActivedTextureUnits())+".", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(unit < 0) {
        MR::Log::LogString("Failed RenderManager::SetTexture(...). unit ("+std::to_string(unit)+") is less than zero.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    if(SetTexture(t->GetTarget(), t->GetGPUHandle(), unit)) {
        return SetTextureSettings(t->GetSettings(), unit);
    }
    return false;
}

bool MR::RenderManager::SetTexture(const unsigned int& target, const unsigned int& handle, const int& unit) {
    if(unit > MR::MachineInfo::MaxActivedTextureUnits()) {
        MR::Log::LogString("Failed RenderManager::SetTexture(...). unit ("+std::to_string(unit)+") > " + std::to_string(MR::MachineInfo::MaxActivedTextureUnits())+".", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(unit < 0) {
        MR::Log::LogString("Failed RenderManager::SetTexture(...). unit ("+std::to_string(unit)+") is less than zero.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    #ifdef MR_CHECK_SMALL_GL_ERRORS
    if(handle == 0) {
        MR::Log::LogString("RenderManager::SetTexture(...). handle is zero.", MR_LOG_LEVEL_WARNING);
    }
    #endif // MR_CHECK_SMALL_GL_ERRORS

    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        glBindMultiTextureEXT(GL_TEXTURE0+unit, target, handle);
    }
    else {
        int actived_tex = 0;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &actived_tex);
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(target, handle);
        glActiveTexture(actived_tex);
    }

    _textures[unit].handle = handle;
    _textures[unit].target = target;

    /// TODO glBindTextures
    return true;
}

bool MR::RenderManager::SetTextureSettings(ITextureSettings* ts, const int& unit) {
    if(unit > MR::MachineInfo::MaxActivedTextureUnits()) {
        MR::Log::LogString("Failed RenderManager::SetTextureSettings(...). unit ("+std::to_string(unit)+") > " + std::to_string(MR::MachineInfo::MaxActivedTextureUnits())+".", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(unit < 0) {
        MR::Log::LogString("Failed RenderManager::SetTextureSettings(...). unit ("+std::to_string(unit)+") is less than zero.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    glBindSampler(unit, (ts) ? ts->GetGLSampler() : 0);
    _textures[unit].settings = ts;

    return true;
}

bool MR::RenderManager::SetTexture(TextureSlot* ts) {
    if(!ts) {
        MR::Log::LogString("Failed RenderManager::SetTexture(passed TextureSlot is null).", MR_LOG_LEVEL_ERROR);
        return false;
    }
    return (SetTexture(ts->target, ts->handle, ts->self_unit) && SetTextureSettings(ts->settings, ts->self_unit));
}
/*
MR::ITextureBindedPtr MR::RenderManager::BindTexture(MR::ITexture* tx) {
    MR::ITextureBinded* tb = nullptr;
    if(tx) {
        if(MR::MachineInfo::FeatureNV_GPUPTR()) {

        }
    }
    return tb;
}
*/
bool MR::RenderManager::UnBindTexture(const int& unit, TextureSlot* binded) {
    if(unit > MR::MachineInfo::MaxActivedTextureUnits() || unit < 0) {
        MR::Log::LogString("Failed RenderManager::UnBindTexture(...). Bad texture unit.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(binded) {
        binded->handle = _textures[unit].handle;
        binded->self_unit = unit;
        binded->settings = _textures[unit].settings;
        binded->target = _textures[unit].target;
    }
    return SetTexture(_textures[unit].target, 0, unit);
}

void MR::RenderManager::ActiveMaterialFlag(const unsigned char& flag) {
    if(_material_flag != flag) {
        _material_flag = flag;
    }
}

void MR::RenderManager::BindRenderTarget(RenderTarget* rt) {
    if(_target != rt) {
        if(_target) UnBindRenderTarget();
        glBindFramebuffer(GL_FRAMEBUFFER, rt->GetFrameBuffer());
        SetViewport(0, 0, rt->GetWidth(), rt->GetHeight());
        _target = rt;
    }
}

void MR::RenderManager::UnBindRenderTarget() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _target = nullptr;
}

void MR::RenderManager::SetViewport(const short & x, const short & y, const short & w, const short & h) {
    glViewport(x, y, w, h);
}

void MR::RenderManager::Reset() {
    SetVertexFormat(nullptr);
    SetIndexFormat(nullptr);
    SetVAO(0);
    SetIndirectDrawParamsBuffer(0);
    SetActivedMaterialPass(nullptr);
    MR::UseNullShaderProgram();
    //SetShaderProgram(nullptr);
    if(_textures) {
        for(int i = 0; i < MR::MachineInfo::MaxActivedTextureUnits(); ++i){
            UnBindTexture(i, nullptr);
        }
    }
    else {
        _textures = new TextureSlot[MR::MachineInfo::MaxActivedTextureUnits()];
        for(int i = 0; i < MR::MachineInfo::MaxActivedTextureUnits(); ++i){
            _textures[i].self_unit = i;
        }
    }

    UnBindRenderTarget();
    UseCamera(nullptr);

    _material_flag = 0;
}

MR::RenderManager::RenderManager() : _vformat(nullptr), _iformat(nullptr), _vao(0), _indirect_drawParams_buffer(0), _pass(nullptr), _textures(nullptr), _target(nullptr), _material_flag(0) {
    _textures = new TextureSlot[MR::MachineInfo::MaxActivedTextureUnits()];
    for(int i = 0; i < MR::MachineInfo::MaxActivedTextureUnits(); ++i){
        _textures[i].self_unit = i;
    }
}

MR::RenderManager::~RenderManager() {
    delete [] _textures;
}
