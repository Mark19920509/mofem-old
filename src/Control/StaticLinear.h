#pragma once

#include <Util/Status.h>
#include <FEM/FEMContext.h>
#include <Output/Output.h>

namespace Control{

    namespace StaticLinear{
        Status run(Model::Data& model, Solution::Data& sol, Output::WriteTimestepFunc write_ts, Output::File file);
    }

}