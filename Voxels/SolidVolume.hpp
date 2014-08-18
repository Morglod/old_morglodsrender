#pragma once

#ifndef _MR_SOLID_VOLUME_H_
#define _MR_SOLID_VOLUME_H_

#include "../Types.hpp"

namespace MR {

class SolidVolume {
public:
    struct UIndex3 {
    public:
        unsigned char x;
        unsigned char y;
        unsigned char z;

        UIndex3();
        UIndex3(const glm::vec3& v);
        UIndex3(const glm::uvec3& v);
        UIndex3(const unsigned char& _x, const unsigned char& _y, const unsigned char& _z);
    };

    typedef MR::Byte Slice[8];

    inline unsigned char* GetPtr() { return &_volume[0][0].byte; }

    inline bool Get(const UIndex3& pos) {
        switch(pos.z) {
        case 0:
            return _volume[pos.x][pos.y].bit1;
            break;
        case 1:
            return _volume[pos.x][pos.y].bit2;
            break;
        case 2:
            return _volume[pos.x][pos.y].bit3;
            break;
        case 3:
            return _volume[pos.x][pos.y].bit4;
            break;
        case 4:
            return _volume[pos.x][pos.y].bit5;
            break;
        case 5:
            return _volume[pos.x][pos.y].bit6;
            break;
        case 6:
            return _volume[pos.x][pos.y].bit7;
            break;
        case 7:
            return _volume[pos.x][pos.y].bit8;
            break;
        }
        return false;
    }

    inline void Set(const UIndex3& pos, const bool& state) {
        switch(pos.z) {
        case 0:
            _volume[pos.x][pos.y].bit1 = state;
            break;
        case 1:
            _volume[pos.x][pos.y].bit2 = state;
            break;
        case 2:
            _volume[pos.x][pos.y].bit3 = state;
            break;
        case 3:
            _volume[pos.x][pos.y].bit4 = state;
            break;
        case 4:
            _volume[pos.x][pos.y].bit5 = state;
            break;
        case 5:
            _volume[pos.x][pos.y].bit6 = state;
            break;
        case 6:
            _volume[pos.x][pos.y].bit7 = state;
            break;
        case 7:
            _volume[pos.x][pos.y].bit8 = state;
            break;
        }
    }

    inline Slice* GetSlicePtr(const unsigned char& level) {
        return &_volume[level];
    }

    SolidVolume();
    ~SolidVolume();
protected:
    Slice _volume[8];
};

}

#endif // _MR_SOLID_VOLUME_H_
