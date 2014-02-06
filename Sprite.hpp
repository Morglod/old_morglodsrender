#pragma once

#ifndef _MR_SPRITES_H_
#define _MR_SPRITES_H_

namespace MR {

class Sprite : public virtual MR::Entity {
protected:
    MR::Texture* texture;
public:
    inline MR::Texture* GetTexture(){
        return texture;
    }
};

}

#endif // _MR_SPRITES_H_
