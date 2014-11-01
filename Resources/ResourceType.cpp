#include "ResourceType.hpp"
#include <functional>

std::hash<std::string> __MR_RES_TYPE_HASH_MAP_STR_;

namespace MR {

ResourceType& ResourceType::operator = (ResourceType const& rt) {
    _name = rt._name;
    _description = rt._description;
    _hash = rt._hash;
    return *this;
}

ResourceType::ResourceType(std::string const& name, std::string const& descr) : _name(name), _description(descr), _hash(__MR_RES_TYPE_HASH_MAP_STR_(name)) {
}

ResourceType::ResourceType(ResourceType const& copy_type) : _name(copy_type._name), _description(copy_type._description), _hash(copy_type._hash) {
}

ResourceType::~ResourceType() {
}

}
