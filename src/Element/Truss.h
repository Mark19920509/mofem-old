#pragma once

#include <Util/Precision.h>
#include <Util/Array.h>
#include <CS/CS.h>
#include <Element/Element.h>
#include <Material/Material.h>
#include <Material/MaterialCalculate.h>
#include <Model/ModelData.h>
#include <Model/ModelGatherScatter.h>
#include <Solution/SolutionData.h>

namespace Element{
    namespace Truss{
        namespace Linear{
            Status calculate(Model::Data const& model, Solution::Data& sol, Element::FlagVector& flags);
        }
        namespace Nonlinear{
            Status calculate(Model::Data const& model, Solution::Data& sol, Element::FlagVector& flags);
        }
    }
}
