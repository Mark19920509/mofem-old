#pragma once

#include "FEMContext.h"
#include <Output/Output.h>
#include <Util/Status.h>

namespace FEM{
    Status init(Context& context);
    Status prepareModel(Context& context);
    Status prepareSolution(Context& context);
    Status run(Context& fem, Output::WriteTimestepFunc write_ts, Output::File file);
}