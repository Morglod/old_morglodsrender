#include "GeometryDrawParams.hpp"
#include "../MachineInfo.hpp"

namespace mr {

IGeometryDrawParamsPtr GeometryDrawParamsArrays::Create(unsigned int const& vStart, unsigned int const& vCount, unsigned int const& instancesNum, unsigned int const& firstInstance) {
    auto* dp = new GeometryDrawParamsArrays(vStart, vCount, instancesNum, firstInstance);
    auto* idp = static_cast<IGeometryDrawParams*>(dp);
    return IGeometryDrawParamsPtr(idp);
}

GeometryDrawParamsArrays::GeometryDrawParamsArrays
                        (unsigned int const& vStart,
                         unsigned int const& vCount,
                         unsigned int const& instancesNum,
                         unsigned int const& firstInstance)
: _cmd( DrawArraysIndirectCmd {vCount, instancesNum, vStart, firstInstance} ) {}

GeometryDrawParamsArrays::~GeometryDrawParamsArrays() {}



IGeometryDrawParamsPtr GeometryDrawParamsElements::Create(unsigned int const& iStart, unsigned int const& iCount, unsigned int const& vStart, unsigned int const& instancesNum, unsigned int const& firstInstance) {
    auto* dp = new GeometryDrawParamsElements(iStart, iCount, vStart, instancesNum, firstInstance);
    auto* idp = static_cast<IGeometryDrawParams*>(dp);
    return IGeometryDrawParamsPtr(idp);
}

GeometryDrawParamsElements::GeometryDrawParamsElements
                          (unsigned int const& iStart,
                           unsigned int const& iCount,
                           unsigned int const& vStart,
                           unsigned int const& instancesNum,
                           unsigned int const& firstInstance)
: _cmd( DrawElementsIndirectCmd {iCount, instancesNum, iStart, vStart, firstInstance} ) {}

GeometryDrawParamsElements::~GeometryDrawParamsElements() {}

}
