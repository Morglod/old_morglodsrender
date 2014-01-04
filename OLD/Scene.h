#pragma once

#ifndef _MR_SCENE_H_
#define _MR_SCENE_H_

#include "Node.h"
#include "MorglodsRender.h"

#include <typeinfo>       // operator typeid
#include <typeindex>      // std::type_index

namespace MR{
    class Scene : public virtual Node{
    public:
        //--------------------------------
        //Calls draw on all attached nodes
        void Draw();

        inline std::type_index GetType(){
            return typeid(MR::Scene);
        }

        //------------------------------------
        //Parent node is new on initialization
        //instance_name - name of new instance
        Scene(const std::string& instance_name);
    };
}

#endif
