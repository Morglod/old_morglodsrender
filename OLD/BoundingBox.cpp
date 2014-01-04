#include "BoundingBox.h"

bool MR::BoundingBox::Test(MR::BoundingBox* bb){
    if( this->max_x < bb->min_x || this->min_x > bb->max_x ) return false;
    if( this->max_y < bb->min_y || this->min_y > bb->max_y ) return false;
    if( this->max_z < bb->min_z || this->min_z > bb->max_z ) return false;
    return true;
}

MR::BoundingBox::BoundingBox(float x1, float y1, float z1, float x2, float y2, float z2){
    float ix, iy, iz, ax, ay, az; // i - min, a - max

    if(x1 < x2){
        ix = x1;
        ax = x2;
    }
    else{
        ix = x2;
        ax = x1;
    }
    if(y1 < y2){
        iy = y1;
        ay = y2;
    }
    else{
        iy = y2;
        ay = y1;
    }
    if(z1 < z2){
        iz = z1;
        az = z2;
    }
    else{
        iz = z2;
        az = z1;
    }
}
