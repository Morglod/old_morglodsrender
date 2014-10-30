#pragma once

#ifndef _MR_OBJ_IMPORTER_H_
#define _MR_OBJ_IMPORTER_H_

#include <string>

namespace MR {

class IGeometry;

class ObjImporter {
public:
    static IGeometry* MakeFromFile(std::string const& file);
};

}

#endif // _MR_OBJ_IMPORTER_H_
