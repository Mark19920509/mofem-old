#include "StaticLinear.h"

#include <FEM/FEMCreate.h>
#include <Solution/SolutionCreate.h>
#include <Solver/CGS.h>

using namespace Control;

Status StaticLinear::run(FEM::Context& fem){
    FEM::prepareModel(fem);
    FEM::prepareSolution(fem);

    fem.solution.u.fill(0);

    Solution::assembleExtForce(fem.model, fem.solution, 3);

    Element::FlagVector flags;
    flags[Element::CALC_STIFF_LINEAR] = true;
    Solution::assembleElements(fem.model, fem.solution, flags);

    Solver::ConjugateGradient(fem.solution.k_e, fem.solution.f_ext, fem.solution.u);

    return Status::SUCCESS;
}