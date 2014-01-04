/*
#include "ObjImporter.h"
#include <vector>

typedef std::vector<float> float_vector;

MR::ImportedObjGeometry* MR::ImportObj(const char* file_text){
    std::string file_str = file_text;

    unsigned int objects_num = 0;
    std::vector<const char*> object_names;
    std::vector<float_vector> object_verts;

    for(unsigned int i = 0; i < file_str.find_first_of('\n'); ++i){
        //NEW LINE

        //NEW OBJECT
        if(file_str[i] == 'o'){
            object_names.push_back(file_str.substr(file_str.find_first_not_of(' '), file_str.find_first_of('\n')).c_str());
            object_verts.push_back(*(new std::vector<float>()));
            objects_num++;
        }
        //NEW VERTEX
        else if(file_str[i] == 'v'){
            file_str = file_str.substr(file_str.find_first_not_of(' '), file_str.length()-file_str.find_first_of('\n'));
            float x = ::atof(file_str.substr(0, file_str.find_first_of(' ')).c_str());
            file_str = file_str.substr(file_str.find_first_of(' ')+1, file_str.length() - file_str.find_first_of('\n'));
            float y = ::atof(file_str.substr(0, file_str.find_first_of(' ')).c_str());
            file_str = file_str.substr(file_str.find_first_of(' ')+1, file_str.length() - file_str.find_first_of('\n'));
            float z = ::atof(file_str.substr(0, file_str.find_first_of('\n')).c_str());

            int last_object = objects_num-1;
            object_verts[last_object].push_back(x);
            object_verts[last_object].push_back(y);
            object_verts[last_object].push_back(z);
        }

        //NEXT LINE
        file_str = file_str.substr(file_str.find_first_of('\n'), file_str.length() - file_str.find_first_of('\n'));
    }

    ImportedObjGeometry* io = new ImportedObjGeometry();
    io->objects_num = objects_num;
    io->object_names = object_names.data();
    float* a = (object_verts.data()[0].data());
    io->object_vertexes = &a;
}*/
