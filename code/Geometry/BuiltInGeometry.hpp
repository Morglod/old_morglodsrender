#pragma once

namespace mr {

class IGeometry;

class BuiltInGeometry {
public:
    static IGeometry* CreateScreenQuad();
};

}
