#pragma once

#include "Output.h"
#include <Util/Status.h>
#include <string>
#include <Model/ModelData.h>
#include <Solution/SolutionData.h>

namespace Output{

    Status OpenLISA(std::string filepath, Output::File& out);
    Status WriteTimestepLISA(Model::Data const& model, Solution::Data const& sol, numeric t, Output::File& out);
    Status CloseLISA(Output::File& out);

}
