#pragma once

#include <string>
#include <util/pugixml.hpp>
#include <functional>
#include <vector>

#include <Model/ModelData.h>
#include <Solution/SolutionData.h>

namespace Output{

    // Allowed output file types
    union Type{
        pugi::xml_document* xml;
    };

    // File including filepath
    struct File{
        std::string filepath;
        Type file;
    };

    // Signature of time-step output function
    using WriteTimestepFunc = std::function < Status(Model::Data const&, Solution::Data const&, int, numeric, Output::File&) > ;

}