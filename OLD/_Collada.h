#ifndef _MR_COLLADA_H_
#define _MR_COLLADA_H_

#ifndef TINYXML_INCLUDED
    #include "tinyxml/tinyxml.h"
#endif

#include "MorglodsRender.h"
#include <string>
#include <memory>

namespace MR{
    namespace Collada{
        class ColladaElement{
        public:
            // id - A text string containing the unique identifier of this element. This value must be unique within the instance document. Optional.
            // name - The text string name of this element. Optional.
            // node_name - name of tag in xml file ( <name> for example )
            std::string id = "", name = "", node_name = "";
        };

        class ColladaVertices : public ColladaElement{
        public:
            unsigned int inputs_num = 0;
            std::string* inputs_semantic = nullptr;
            std::string* inputs_source = nullptr;
        };

        class ColladaPolylist : public ColladaElement{
        public:
            unsigned int inputs_num = 0;
            unsigned int* inputs_offset = nullptr;
            std::string* inputs_semantic = nullptr;
            std::string* inputs_source = nullptr;

            unsigned int count = 0;
            std::string material = "";

            //Stride
            unsigned int vcount = 1;

            unsigned int indecies_num = 0;
            unsigned int* indecies = nullptr;
        };

        class ColladaAccessor : public ColladaElement{
        public:
            //The number of times the array is accessed. Required.
            unsigned int count = 0;
            //The index of the first value to be read from the array. The default is 0. Optional.
            unsigned int offset = 0;

            /*The location of the array to access using a URI expression. Required.
            This element may refer to a COLLADA array element or to an array data source outside the scope of the instance document;
            the source does not need to be a COLLADA document.*/
            std::string source = "";

            //The number of values that are to be considered a unit during each access to the array. The default is 1, indicating that a single value is accessed. Optional.
            unsigned short stride = 1;
        };

        class ColladaSourceArray : public ColladaElement{
        public:
            //The number of values in the array. Required.
            unsigned int count = 0;
        };

        class ColladaFloatArray : public ColladaSourceArray{
        public:
            //Array with (count) elements
            float* array;

            //The number of significant decimal digits of the float values that can be contained in the array. The default is 6. Optional.
            short digits = -1;

            //The largest exponent of the float values that can be contained in the array. The default is 38. Optional.
            short magnitude = -1;
        };

        class ColladaGeometrySource : public ColladaElement{
        public:
            // <"float_array"> for example
            ColladaSourceArray* array = nullptr;
            // <accessor>
            ColladaAccessor* access = nullptr;
        };

        class ColladaGeometry : public ColladaElement{
        public:
            //Array and it's size
            unsigned int num_sources = 0;
            ColladaGeometrySource* sources = nullptr;
            ColladaVertices* vertices = nullptr;
            ColladaPolylist* polylist = nullptr;
        };

        MR::GeometryBuffer* Convert(ColladaGeometry * cg);

        class ColladaData{
        protected:
            static inline std::string GetTiXmlSafeNodeValue(TiXmlNode* parent, std::string child_node_name){
                TiXmlNode* n = parent->FirstChild( child_node_name.c_str() );
                if(n != nullptr) return std::string( n->ValueTStr().c_str() );
                else return "";
            }

            static inline std::string GetTiXmlSafeNodeElAttrib(TiXmlNode* node, std::string attrib_name){
                const char* a = node->ToElement()->Attribute( attrib_name.c_str() );
                if(a != nullptr){
                    return std::string(a);
                }
                else return "";
            }

        public:
            ColladaData(){}

            std::string version = "", author = "", tool = "", comments = "", copy_right = "", source_data = "";
            unsigned int geom_num = 0;
            ColladaGeometry* geoms = nullptr;

            //Import data from collada scene format (xml)
            //xml_file - path to xml file
            //log - log info (true by default)
            //returns nullptr if failed
            static ColladaData* Import(std::string xml_file, bool log = true);
        };
    }
}

#endif // _MR_COLLADA_H_
