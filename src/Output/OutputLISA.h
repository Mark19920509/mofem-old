#pragma once

#include <Util/Status.h>
#include <string>
#include <Model/ModelData.h>
#include <Solution/SolutionData.h>

namespace Output{

    Status WriteLISA(Model::Data const& model, Solution::Data const& sol, std::string filepath);

}
