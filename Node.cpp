#include "Node.h"
#include "MorglodsRender.h"

void MR::Node::Draw(){
    if(this->OnDraw != NULL) this->OnDraw(this);

    //!!IF CHANGE THIS, CHANGE GEOMETRY BUFFER CODE OF DRAW FUNCTION! AND SCENE AND MESH
    glPushMatrix();

    glTranslatef(this->tX, this->tY, this->tZ);
    glRotatef(this->rX, 1, 0, 0);
    glRotatef(this->rY, 0, 1, 0);
    glRotatef(this->rZ, 0, 0, 1);
    glScalef(this->sX, this->sY, this->sZ);

    for(std::vector<Node*>::iterator it = this->Children.begin(); it != this->Children.end(); it++){
        (*it)->Draw();
    }

    glPopMatrix();
}

void MR::Node::CountObjectsOfTypeInNode(MR::Node* n, std::type_index type, unsigned int & result){
    if(n->GetType() == type) result++;
    for(std::vector<MR::Node*>::iterator it = n->Children.begin(); it != n->Children.end(); it++){
        MR::Node::CountObjectsOfTypeInNode(it[0], type, result);
    }
}

MR::Node::~Node(){
    this->Children.clear();
    this->parent = 0;
}
