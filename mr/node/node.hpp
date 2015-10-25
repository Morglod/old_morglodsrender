#pragma once

#include "../build.hpp"

#include <unordered_map>
#include <string>
#include <memory>

/**

TEST ONLY

**/

namespace mr {

typedef std::shared_ptr<Node> NodePtr;

class MR_API Node {
public:
protected:
    size_t _parent;
    std::string _name;
};

class MR_API NodeMap {
public:
protected:
    std::unordered_map<size_t, NodePtr> nodes; // <hash name, ptr>
};

class MR_API NodeManager {
public:
};

}
