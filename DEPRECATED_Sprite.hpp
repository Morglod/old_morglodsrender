#pragma once

#ifndef _MR_SPRITES_H_
#define _MR_SPRITES_H_

#include <Entity.hpp>

namespace MR {

class Sprite : public virtual MR::Entity {
public:
    inline MR::Texture* GetTexture();
protected:
    MR::Texture* texture;
};

}

inline MR::Texture* MR::Sprite::GetTexture(){
}

#endif // _MR_SPRITES_H_
