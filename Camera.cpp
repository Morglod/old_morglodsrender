#include "Camera.h"

void MR::Camera::Use(){
    //if(this->renderTarget != NULL){
        //if(this->renderTarget->SizeChanged)
            //this->AspectRatio = this->renderTarget->Width/this->renderTarget->Height;
        gluPerspective(this->FovY, this->AspectRatio, this->zNear, this->zFar);
        gluLookAt(this->cameraX, this->cameraY, this->cameraZ, this->tX, this->tY, this->tZ, this->upX, this->upY, this->upZ);
    //}
}
