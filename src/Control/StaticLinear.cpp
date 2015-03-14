#include "StaticLinear.h"

#include <FEM/FEMCreate.h>
#include <Solution/SolutionCreate.h>
#include <Solver/CGS.h>

using namespace Control;

Status StaticLinear::run(FEM::Context& fem, Output::WriteTimestepFunc write_ts, Output::File file){
    FEM::prepareModel(fem);
    FEM::prepareSolution(fem);

    fem.solution.vec[Solution::DISP].fill(0);

    Solution::assembleExtForce(fem.model, fem.solution, 3);

    Element::FlagVector flags;
    flags[Element::CALC_STIFF_LINEAR] = true;
    Solution::assembleElements(fem.model, fem.solution, flags);

    Solver::ConjugateGradient(fem.solution.mat[Solution::STIFF_LINEAR], 
                              fem.solution.vec[Solution::F_EXT], 
                              fem.solution.vec[Solution::DISP]);

    write_ts(fem.model, fem.solution, 0, file);

    return Status::SUCCESS;
}