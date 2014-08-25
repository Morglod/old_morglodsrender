#pragma once

#ifndef _MR_RENDER_MANAGER_H_
#define _MR_RENDER_MANAGER_H_

#include "Utils/Containers.hpp"
#include "Utils/Singleton.hpp"

#include <memory>

namespace MR {

class IGeometry;
class IGeometryBuffer;
class IGeometryDrawParams;
typedef std::shared_ptr<IGeometryDrawParams> IGeometryDrawParamsPtr;

class IVertexFormat;
class IIndexFormat;
class IShaderProgram;
class ITexture;
class ITextureSettings;
class RenderTarget;
class IEntity;
class ICamera;
class IMaterialPass;

class RenderManager : public Singleton<RenderManager> {
public:
    //Geometry
    void SetVertexFormat(IVertexFormat* f);
    void SetIndexFormat(IIndexFormat* f);
    void DrawGeometry(IGeometry* g);
    void DrawGeometryBuffer(IGeometryBuffer* b, IGeometryDrawParamsPtr drawParams);
    void SetVAO(const unsigned int& vao);
    void SetIndirectDrawParamsBuffer(const unsigned int& buf);

    //Material
    class TextureSlot {
    public:
        unsigned int target;
        unsigned int handle;
        ITextureSettings* settings;
        int self_unit;

        TextureSlot() : target(0), handle(0), settings(nullptr), self_unit(0) {}
    };

    inline IMaterialPass* GetActivedMaterialPass() { return _pass; }
    void SetActivedMaterialPass(IMaterialPass* p);
    void SetShaderProgram(IShaderProgram* p);
    bool SetTexture(ITexture*, const int& unit);
    bool SetTexture(const unsigned int& target, const unsigned int& handle, const int& unit);
    bool SetTextureSettings(ITextureSettings* ts, const int& unit);
    bool SetTexture(TextureSlot* ts);
    //ITextureBindedPtr BindTexture(ITexture* tx); //bind texture to any free slot
    bool UnBindTexture(const int& unit, TextureSlot* binded = nullptr); //binded may be nullptr. it stores info about unbinded texture
    void ActiveMaterialFlag(const unsigned char& flag);
    inline unsigned char GetActivedMaterialFlag() { return _material_flag; }

    //View
    void BindRenderTarget(RenderTarget* rt);
    void UnBindRenderTarget();
    void SetViewport(const short & x, const short & y, const short & w, const short & h);
    void UseCamera(ICamera* cam) {}

    //Frame
    class FrameInfo {
    public:
        float delta; //time from last frame
        ///TODO statistic
    };

    void BeginFrame(FrameInfo* frame);
    void EndFrame(FrameInfo* frame);

    void Reset();

    RenderManager();
    virtual ~RenderManager();
protected:
    IVertexFormat* _vformat;
    IIndexFormat* _iformat;
    unsigned int _vao;
    unsigned int _indirect_drawParams_buffer;

    IMaterialPass* _pass;
    IShaderProgram* _program;
    TextureSlot* _textures;
    RenderTarget* _target;
    unsigned char _material_flag;
};

}

#endif



/*
    class RenderCommand {
    public:
        enum RenderCommandType {
            RCT_Unknow = 0, //just exec
            RCT_StateChange = 1,
            RCT_DrawGeometry,
            RCT_BindVertexFormat
        };

        RenderCommandType type;

        RenderCommand(const RenderCommandType& t) : type(t) {}
        virtual ~RenderCommand() {}
    };

    class DrawGeometryCommand : public RenderCommand {
    public:
        IGeometry* geometry;
        DrawGeometryCommand(IGeometry* g) : RenderCommand(RenderCommand::RCT_DrawGeometry), geometry(g) {}
        virtual ~DrawGeometryCommand() {}
    };

    class BindVertexFormatCommand : public RenderCommand {
    public:
        IVertexFormat* format;
        BindVertexFormatCommand(IVertexFormat* f) : RenderCommand(RenderCommand::RCT_BindVertexFormat), format(f) {}
        virtual ~BindVertexFormatCommand() {}
    };

    class BindIndexFormatCommand : public RenderCommand {
    public:
        IIndexFormat* format;
        BindIndexFormatCommand(IIndexFormat* f) :
    };*/

//void Frame(const float& delta);

//MR::ThreadSafeQueue<RenderCommand*> _cmds;
