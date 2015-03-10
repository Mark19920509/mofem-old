#pragma once

#include "FEMContext.h"

#include <Util/Status.h>

namespace FEM{
    Status init(Context& context);
    Status prepareModel(Context& context);
    Status prepareSolution(Context& context);
}