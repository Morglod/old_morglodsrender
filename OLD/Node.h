#pragma once

#ifndef _MR_NODE_H_
#define _MR_NODE_H_

#include <vector>
#include <algorithm>
#include <string>

#include <typeinfo>       // operator typeid
#include <typeindex>      // std::type_index

//GLEW
#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

//GLFW
#ifndef _glfw3_h_
#   include <GLFW\glfw3.h>
#endif

namespace MR{
    //----------------------------------
    //Handles render units in scene tree
    //----------------------------------
    class Node{
    public:
        //---------
        //Node name
        std::string name = "node_noname";

        //-------------------
        //Children nodes list
        std::vector<Node*> Children;

        //---------------
        //Pos, Rot, Scale
        //Used in Draw methods
        float tX = 0, tY = 0, tZ = 0; //translation ( used in Draw() )
        float rX = 0, rY = 0, rZ = 0; //rotation ( used in Draw() )
        float sX = 1, sY = 1, sZ = 1; //scale ( used in Draw() )

        //-----------
        //Parent node
        Node* parent = NULL;

        //------------------------------------
        //Calls in draw method before any work
        //As first argument passed "this"
        void (*OnDraw)(Node*) = NULL;

        //--------------------------------------
        //Calls in AttachTo method of child node
        //As first argument passed parent node (this), as second child node (not this)
        void (*OnChildAttached)(Node*, Node*) = NULL;

        //---------------------------------------
        //Calls Draw method in all children nodes
        //Overloads in objects like Mesh, Geom Buffers, Scene, etc.
        //!!Don not uses parent translations, rot, scale
        virtual void Draw();

        //-------------------
        //Returns parent node
        //Use this node instead of (this->parent)
        virtual inline Node* ParentNode(){return this->parent;}

        //---------------------------------------
        //Attachs this node to n by pointer to it
        virtual inline void AttachTo(Node* n){
            this->parent = n;
            n->Children.push_back(this);

            if(this->parent->OnChildAttached != NULL)
                this->parent->OnChildAttached(this->parent, this);
        }

        //------------------------
        //Detaches node from parent
        //Checks if parent is not null
        //Calls Detach(Node*) method of parent
        virtual inline void Detach(){
            if(this->parent != NULL){
                this->parent->Detach(this);
            }
        }

        //------------------
        //Detaches child node
        virtual inline void Detach(Node* n){
            std::vector<Node*>::iterator it = std::find(this->Children.begin(), this->Children.end(), n);
            if(it != this->Children.end()){
                this->Children.erase(it);
                n->parent = NULL;
            }
        }

        //--------------------------
        //Detaches all children nodes
        virtual inline void DetachAll(){
            this->Children.clear();
        }

        //-----------------------------------
        //Creates child node of this (parent)
        //Returns pointer to created child node
        virtual inline Node* CreateChild(){
            Node* ch = new Node();
            ch->AttachTo(this);
            return ch;
        }

        //-----------------------------------
        //Creates child node of this (parent)
        //new_name - name of new, child node
        //Returns pointer to created child node
        virtual inline Node* CreateChild(std::string new_name){
            Node* ch = new Node();
            ch->name = new_name;
            ch->AttachTo(this);
            return ch;
        }

        //------------------------
        //Returns typeid(MR::Node)
        virtual inline std::type_index GetType(){
            return typeid(MR::Node);
        }

        //--------------------------------------------------
        //Uses all parent transforms (and parent of parents)
        virtual inline void UseAllParentTransformations(){
            MR::Node* current_node = this->parent;
            while(current_node != NULL){
                current_node->UseTransformations();
                current_node = current_node->parent;
            }
        }

        //----------------------------
        //Uses current transformations
        virtual inline void UseTransformations(){
            glTranslatef(this->tX, this->tY, this->tZ);
            glRotatef(this->rX, 1, 0, 0);
            glRotatef(this->rY, 0, 1, 0);
            glRotatef(this->rZ, 0, 0, 1);
            glScalef(this->sX, this->sY, this->sZ);
        }

        static void _FindByName(MR::Node* n, std::string name, std::vector<MR::Node*> & vector_node){
            for(std::vector<MR::Node*>::iterator it = n->Children.begin(); it != n->Children.end(); ++it){
                if( (*it)->name == name ) vector_node.push_back(*it);
                _FindByName( (*it), name, vector_node );
            }
        }

        //-------------------------------------------
        //name - name of searching nodes (Node->name)
        //finded_nodes - dest array of finded nodes
        //num - num of finded nodes
        //Returns true if something finded
        virtual inline bool FindByName(std::string name, MR::Node* & finded_nodes, unsigned int & num){
            std::vector<MR::Node*> vec;
            _FindByName( this, name, vec );
            num = vec.size();
            finded_nodes = vec.data()[0];
            if(num != 0) return true;
            return false;
        }

        //----------------------------------------
        //Counts children nodes of same type_index
        static void CountObjectsOfTypeInNode(MR::Node* n, std::type_index type, unsigned int & result);

        virtual ~Node();
    };
}

#endif // _MR_NODE_H_
