#ifndef _MR_BOUNDING_BOX_
#define _MR_BOUNDING_BOX_

namespace MR{
    class BoundingBox{
    public:
        float min_x = 0, min_y = 0, min_z = 0, max_x = 0, max_y = 0, max_z = 0;

        //Returns true if collides
        bool Test(BoundingBox* bb);
        bool TestFrustum(float zNear, float zFar, float fovy);

        BoundingBox(){}
        BoundingBox(float x1, float y1, float z1, float x2, float y2, float z2);
    };
}

#endif
