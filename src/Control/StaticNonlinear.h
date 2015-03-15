#pragma once

#include <Util/Status.h>
#include <FEM/FEMContext.h>
#include <Output/Output.h>

namespace Control{

    namespace StaticNonlinear{
        enum { TIMESTEPS, STEPSIZE, NEWTON_TOL, NEWTON_MAX_ITER, COUNT };
        Status run(Model::Data& model, Solution::Data& sol, Output::WriteTimestepFunc write_ts, Output::File file);
    }

}