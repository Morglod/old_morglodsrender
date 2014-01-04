#include "Collada.h"

MR::Collada::ColladaData* MR::Collada::ColladaData::Import(std::string xml_file, bool log){
	//Creating main objects
	ColladaData* cdata = new ColladaData();
	TiXmlDocument doc( xml_file.c_str() );

	//Load xml; If failed log and return null
	if( !doc.LoadFile() ){
		if(log) MR::Log::LogString("Error while loading xml file (ColladaData)");
		return nullptr;
	}

	//Finding root node; If failed log and return null
	TiXmlNode * ColladaRoot = doc.FirstChild("COLLADA");
	if( ColladaRoot == nullptr ){
		if(log) MR::Log::LogString("Can't find <COLLADA> node (ColladaData)");
		return nullptr;
	}

	//Get info (asset node)
	cdata->version = ColladaRoot->ToElement()->Attribute("version");
	if(log) MR::Log::LogString( std::string("COLLADA verison = ") + ColladaRoot->ToElement()->Attribute("version") );
	TiXmlNode * asset_node = ColladaRoot->FirstChild("asset");

	if(asset_node != nullptr){
		TiXmlNode * contr = asset_node->FirstChild("contributor");
		if(contr != nullptr){
			cdata->author = GetTiXmlSafeNodeValue(contr, "author");
			cdata->tool = GetTiXmlSafeNodeValue(contr, "authoring_tool");
			cdata->comments = GetTiXmlSafeNodeValue(contr, "comments");
			cdata->copy_right = GetTiXmlSafeNodeValue(contr, "copyright");
			cdata->source_data = GetTiXmlSafeNodeValue(contr, "source_data");

			if(log) MR::Log::LogString( "COLLADA author = " + cdata->author );
			if(log) MR::Log::LogString( "COLLADA tool = " + cdata->tool );
			if(log) MR::Log::LogString( "COLLADA comments = " + cdata->comments );
			if(log) MR::Log::LogString( "COLLADA copyright = " + cdata->copy_right );
			if(log) MR::Log::LogString( "COLLADA source_data = " + cdata->source_data );
		}
	}

	//Get lib geometry node
	TiXmlNode * lib_geometry_node = ColladaRoot->FirstChild("library_geometries");

	//Foreach geometry node cycle
	std::vector<ColladaGeometry> geoms;
	for( TiXmlNode * geom_node = lib_geometry_node->FirstChild("geometry"); geom_node != nullptr; geom_node = geom_node->NextSibling("geometry") ){
		ColladaGeometry cgeom;
		cgeom.node_name = "geometry";
		cgeom.id = GetTiXmlSafeNodeElAttrib(geom_node, "id");
		cgeom.name = GetTiXmlSafeNodeElAttrib(geom_node, "name");

		//Work with mesh data
		//Foreach geometry source node cycle
		std::vector<ColladaGeometrySource> cgsources;
		for( TiXmlNode * source_node = geom_node->FirstChild("mesh")->FirstChild("source"); source_node != nullptr; source_node = source_node->NextSibling("source") ){
			ColladaGeometrySource cgsource;
			cgsource.node_name = "source";
			cgsource.id = GetTiXmlSafeNodeElAttrib(source_node, "id");
			cgsource.name = GetTiXmlSafeNodeElAttrib(source_node, "name");

			//If float array node exists
			TiXmlNode * arr_node = source_node->FirstChild("float_array");
			if(arr_node != nullptr){
				ColladaFloatArray * fa = new ColladaFloatArray();
				fa->node_name = "float_array";
				fa->id = GetTiXmlSafeNodeElAttrib(arr_node, "id");
				fa->name = GetTiXmlSafeNodeElAttrib(arr_node, "name");
				fa->count = atoi( GetTiXmlSafeNodeElAttrib(arr_node, "count").c_str() );
				fa->array = new float[fa->count];

				std::string work_string = arr_node->ToElement()->GetText();
				for(unsigned int i = 0; i < fa->count; ++i){
					fa->array[i] = atof(work_string.substr(0, work_string.find_first_of(' ')).c_str() );
					work_string = work_string.erase(0, work_string.find_first_of(' ')+1);
				}

				cgsource.array = fa;
			}
			//else different arrays

			TiXmlNode * accessor_node = source_node->FirstChild("technique_common")->FirstChild("accessor");
			if(accessor_node != nullptr){
                ColladaAccessor * ca = new ColladaAccessor();
                ca->id = GetTiXmlSafeNodeElAttrib(accessor_node, "id");
                ca->name = GetTiXmlSafeNodeElAttrib(accessor_node, "name");
                ca->source = GetTiXmlSafeNodeElAttrib(accessor_node, "source");
                ca->count = atoi( GetTiXmlSafeNodeElAttrib(accessor_node, "count").c_str() );
                ca->stride = atoi( GetTiXmlSafeNodeElAttrib(accessor_node, "stride").c_str() );
                cgsource.access = ca;
			}

			cgsources.push_back(cgsource);
			//source_node = geom_node->FirstChild("mesh")->IterateChildren( source_node );
		}

		//Vertices
		TiXmlNode * vertices_node = geom_node->FirstChild("mesh")->FirstChild("vertices");
		if(vertices_node != nullptr){
            std::vector<std::string> semantics;
            std::vector<std::string> sources;

            //For each input
            for( TiXmlNode * input_node = vertices_node->FirstChild("input"); input_node != nullptr; input_node = input_node->NextSibling("input") ){
                semantics.push_back( GetTiXmlSafeNodeElAttrib(input_node, "semantic") );
                sources.push_back( GetTiXmlSafeNodeElAttrib(input_node, "source") );
            }

            ColladaVertices cv;
            cv.id = GetTiXmlSafeNodeElAttrib(vertices_node, "id");
            cv.name = GetTiXmlSafeNodeElAttrib(vertices_node, "name");
            cv.node_name = "vertices";
            cv.inputs_num = semantics.size();
            cv.inputs_semantic = semantics.data();
            cv.inputs_source = sources.data();

            cgeom.vertices = &cv;
		}

		//Polylist
		TiXmlNode * polylist_node = geom_node->FirstChild("mesh")->FirstChild("polylist");
		if(polylist_node != nullptr){
            std::vector<std::string> semantics;
            std::vector<std::string> sources;
            std::vector<unsigned int> offsets;

            //For each input
            for( TiXmlNode * input_node = polylist_node->FirstChild("input"); input_node != nullptr; input_node = input_node->NextSibling("input") ){
                semantics.push_back( GetTiXmlSafeNodeElAttrib(input_node, "semantic") );
                sources.push_back( GetTiXmlSafeNodeElAttrib(input_node, "source") );
                offsets.push_back( atoi( GetTiXmlSafeNodeElAttrib(input_node, "offset").c_str() ) );
            }

            ColladaPolylist cv;
            cv.id = GetTiXmlSafeNodeElAttrib(polylist_node, "id");
            cv.name = GetTiXmlSafeNodeElAttrib(polylist_node, "name");
            cv.material = GetTiXmlSafeNodeElAttrib(polylist_node, "material");
            cv.count = atoi( GetTiXmlSafeNodeElAttrib(polylist_node, "count").c_str() );
            cv.node_name = "polylist";
            cv.inputs_num = semantics.size();
            cv.inputs_offset = offsets.data();
            cv.inputs_semantic = semantics.data();
            cv.inputs_source = sources.data();

            TiXmlNode * p_node = polylist_node->FirstChild("p");
            if(p_node != nullptr){
                std::string work_string = p_node->ToElement()->GetText();
                std::vector<unsigned int> indecies;

				for(unsigned int i = 0; i < cv.count*3*cv.inputs_num; ++i){
					indecies.push_back( atoi(work_string.substr(0, work_string.find_first_of(' ')).c_str() ) );
					work_string = work_string.erase(0, work_string.find_first_of(' ')+1);
				}

				cv.indecies = indecies.data();
				cv.indecies_num = indecies.size();
            }

            cgeom.polylist = &cv;
		}

		//Save data
		cgeom.num_sources = cgsources.size();
		cgeom.sources = cgsources.data();

		geoms.push_back(cgeom);
		//geom_node = lib_geometry_node->IterateChildren( geom_node );
	}

	cdata->geoms = geoms.data();
	cdata->geom_num = geoms.size();

	return cdata;
}

#include <sstream>
std::string ftos(float number){
    std::ostringstream buff;
    buff<<number;
    return buff.str();
}

MR::GeometryBuffer* MR::Collada::Convert(ColladaGeometry * cg){
    //find float array
    ColladaFloatArray* cfa = ((ColladaFloatArray*)cg->sources[0].array);
    float* farr = cfa->array;
    unsigned int size = cfa->count;

    for(unsigned int i = 0; i < size; ++i){
        MR::Log::LogString( ftos(farr[i]) + " ");
    }

    std::vector<unsigned int> indecies;
    for(unsigned int i = 0; i < cg->polylist->indecies_num; i += cg->polylist->inputs_num ){
        indecies.push_back( cg->polylist->indecies[i] );
    }

    //find positions array with <vertices> tag
    /*for(unsigned int i = 0; i < cg->num_sources; ++i){
        if( cg->sources[i].id.find("-positions") != 0){
            farr =
            size = ;
            break;
        }
    }*/

    MR::VertexBuffer* vb = new MR::VertexBuffer(farr, size, GL_STATIC_DRAW);
    MR::IndexBuffer* ib = new MR::IndexBuffer(indecies.data(), indecies.size(), GL_STATIC_DRAW);
    MR::GeometryBuffer* geom_buffer = new MR::GeometryBuffer(vb, ib, GL_POINTS);
    return geom_buffer;
}
