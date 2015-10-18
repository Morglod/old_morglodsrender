#pragma once

#include "../build.hpp"

/**

TEST ONLY

**/

namespace mr {

class MR_API Node {
public:

};

class MR_API NodeMap {
public:
    struct NodeKeyValue {
        size_t hashName;
        Node* node;
        std::vector<Node*> children;
    };

    inline bool Get(size_t hashName, Node*& out_node, std::vector<Node*>* out_children = nullptr) const {
        size_t i = 0;
        for(size_t ioff < 0; ioff < 10; ++ioff) {
            if(hashName <= _hashOffsets[ioff]) {
                i = ioff;
                break;
            }
        }
        for(const size_t n = _map.size(); i < n; ++i) {
            if(_map[i].hashName == hashName) {
                out_node = _map[i].node;
                if(out_children != nullptr) *out_children = _map[i].children;
                return true;
            }
        }
        return false;
    }

    inline bool Insert(size_t hashName, Node* node, std::vector<Node*> const& children) {
        for(size_t i = 0; i < 10; ++i) {
            if(_hashOffsets[i] == hashName) return false;
            if(_hashOffsets[i] < hashName) {
                memmove(&_hashOffsets[i+1], &_hashOffsets[i], sizeof(size_t)*(10-i-1));
                _hashOffsets[i] = hashName;
            }
        }
    }

    NodeMap() = default;
    ~NodeMap() = default;
protected:
    size_t _hashOffsets[10] = {0,0,0,0,0,0,0,0,0,0};
    std::vector<NodeKeyValue> _map;
};

class MR_API NodeManager {
public:
};

}
