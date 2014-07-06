#pragma once

#ifndef _MR_SVO_H_
#define _MR_SVO_H_

namespace MR {

class SVONode {
public:
    inline virtual SVONode* GetParent() { return _parent; }
    inline virtual SVONode** GetChildren() { return &_children[0][0]; }
protected:
    SVONode* _parent = 0;
    SVONode* _children[4][4];
};

class SVORootNode : public SVONode {
public:
    SVONode* GetParent() override { return 0; }
};

}

#endif // _MR_SVO_H_
