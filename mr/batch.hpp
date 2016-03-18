#pragma once

#include "build.hpp"

#include <memory>
#include <vector>

namespace mr {

typedef std::shared_ptr<class Mesh> MeshPtr;
typedef std::shared_ptr<class Batch> BatchPtr;

class MR_API Batch {
public:
    static bool Create(std::vector<MeshPtr> const& meshes, std::vector<MeshPtr>& out_Meshes);
protected:
    Batch() = default;
};

}
