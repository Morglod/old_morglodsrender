#pragma once

#include "CoreObjects.hpp"

namespace mr {

class IQuery : public IGPUObjectHandle {
public:
    virtual void Begin() = 0;
    virtual void End() = 0;
    virtual unsigned int GetTarget() = 0;
    virtual bool GetResult(void* resultDst) = 0;
    virtual bool IsEnded() = 0;
    virtual ~IQuery(){}
    //virtual unsigned int GetGPUHandle() = 0;
    //virtual void Destroy();
};

typedef std::shared_ptr<IQuery> QueryPtr;

class Query : public IQuery {
public:
    enum Target {
        SamplesPassed = 0x8914, //uint
        AnySamplesPassed = 0x8C2F, //int (boolean)
        AnySamplesPassedConservative = 0x8D6A, //int (boolean), gl 4.3 req or QT_AnySamplesPassed will be used
        PrimitivesGenerated = 0x8C87, //uint
        TransformFeedbackPrimitivesWritten = 0x8C88, //uint
        TimeElapsed = 0x88BF, //uint64
        TimeStamp = 0x8E28 //uint64
    };

    void Begin() override;
    void End() override;
    unsigned int GetTarget() override { return _target; }
    bool GetResult(void* resultDst) override;
    void Destroy() override;
    bool IsEnded() override { return _ended; }

    Query(const Query::Target& target);
    virtual ~Query();

    static Query GetQuery(const Query::Target& target);
protected:
    unsigned int _target;
    bool _ended;
};

}
