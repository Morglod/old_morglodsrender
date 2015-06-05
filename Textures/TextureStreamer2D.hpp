#pragma once

#include <mu/Containers.hpp>
#include <stdlib.h>
#include <memory>
#include <mutex>
#include <iostream>

#include <glm/glm.hpp>

namespace mr {

class Texture2D;
class IGPUBuffer;

class TextureStreamer2D {
public:
    class IDataSource {
    public:
        virtual bool IsEnd() = 0;
        virtual size_t GetCurPos() = 0;
        virtual size_t Read(char* dstBuffer, size_t const& size) = 0;
        virtual bool Prepare() = 0; //init; before all readings.
        virtual void Close() = 0;
        virtual size_t GetTotalSize() = 0;
        virtual ~IDataSource() {}
    };

    typedef std::shared_ptr<IDataSource> IDataSourcePtr;

    class BufferDataSource : public IDataSource {
    public:
        inline mu::ArrayHandle<unsigned char> GetBuffer() { return _buffer; }
        inline bool IsEnd() override { return !(_curPos < _buffer.GetNum()); }
        inline size_t GetCurPos() override { return _curPos; }
        inline bool Prepare() override { return true; }
        inline void Close() override {}
        inline size_t GetTotalSize() override { return _buffer.GetNum(); }

        inline size_t Read(char* dstBuffer, size_t const& size) override {
            if(IsEnd()) return 0;
            const size_t sz_read = std::min(size, GetTotalSize() - GetCurPos());
            memcpy(dstBuffer, &(_buffer.GetArray()[_curPos]), sz_read);
            _curPos += sz_read;
            return sz_read;
        }

        inline void Reset(mu::ArrayHandle<unsigned char> buffer, size_t const& curPos) {
            _buffer = buffer;
            _curPos = curPos;
        }

        BufferDataSource(mu::ArrayHandle<unsigned char> buffer) : _buffer(buffer) {}
        virtual ~BufferDataSource() {}
    protected:
        mu::ArrayHandle<unsigned char> _buffer;
        size_t _curPos = 0;
    };

    inline Texture2D* GetTarget() const { return _target; }

    bool Reset(Texture2D* newTarget, IDataSourcePtr const& dataSource);
    void Destroy();
    inline bool IsSynced() const { return _synced; }

    inline bool IsFinished() const {
        if(_dataSource == nullptr) return true;
        if(_dataSource->IsEnd()) return true;
        return false;
    }

    /* return true if ok. false if end or error */
    bool Sync(); //should be called in main thread
    bool Step(); //should be called in second thread

    inline void Lock() { _mtx.lock(); }
    inline bool TryLock() { return _mtx.try_lock(); }
    inline void UnLock() { return _mtx.unlock(); }

    TextureStreamer2D() = default;
    TextureStreamer2D(Texture2D* target, IDataSourcePtr const& dataSource);
    virtual ~TextureStreamer2D();
protected:
    Texture2D* _target = nullptr;
    IDataSourcePtr _dataSource = nullptr;
    IGPUBuffer* _pbos[2];
    glm::uvec2 _pageSize;
    unsigned char _curPbo = 0;
    bool _synced = true;
    static const size_t _totalIterations = 2;
    size_t _iterations = 0;
    std::mutex _mtx;
};

}
