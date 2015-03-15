#pragma once

#include <Output/Output.h>
#include <Util/Status.h>
#include <string>
#include <Model/ModelData.h>
#include <Solution/SolutionData.h>

namespace Output{

    namespace LISA{
        Status Open(Model::Data const& model, std::string filepath, Output::File& out);
        Status WriteTimestep(Model::Data const& model, Solution::Data const& sol, int timestep_id, numeric timestep, Output::File& out);
        Status Close(Output::File& out);
    }

}
