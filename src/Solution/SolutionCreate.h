#pragma once

#include <Model/ModelData.h>
#include <Solution/SolutionData.h>
#include <Element/Element.h>

namespace Solution{
    Status allocateMemory(Model::Data const& model, Solution::Data& sol);

    Status assembleElements(Model::Data const& model, Solution::Data& sol, Element::FlagVector& flags);
    Status assembleExtForce(Model::Data const& model, Solution::Data& sol, numeric t);

}