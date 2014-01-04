#include "Scene.h"

void MR::Scene::Draw(){
    //!!NODE DRAW CODE----------------------
    glPushMatrix();

    glTranslatef(this->tX, this->tY, this->tZ);
    glRotatef(this->rX, 1, 0, 0);
    glRotatef(this->rY, 0, 1, 0);
    glRotatef(this->rZ, 0, 0, 1);
    glScalef(this->sX, this->sY, this->sZ);
    //!!END FIRST PART-----------------------

    //!!NODE DRAW CODE--------------
    for(std::vector<Node*>::iterator it = this->Children.begin(); it != this->Children.end(); it++){
        it[0]->Draw();
    }

    glPopMatrix();
    //!!END NODE DRAWING CODE-------
}
/*
void MR::Scene::_Optimize(){
    //Use value from material manager if it appears =D
    //unsigned int different_materials_num_in_scene = 0;
    //Use value from scene manager if it appears =D
    unsigned int different_buffers_num_in_scene = 0;

    MR::Scene::CountObjectsOfTypeInNode(this, typeid(MR::GeometryBuffer), different_buffers_num_in_scene);
}
*/
MR::Scene::Scene(const std::string instance_name){
    this->parent = new MR::Node();
}
