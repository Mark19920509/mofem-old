#include "StaticLinear.h"

#include <FEM/FEM.h>
#include <Solution/SolutionCreate.h>
#include <Solver/CGS.h>

using namespace Control;

Status StaticLinear::run(Model::Data& model, Solution::Data& sol, Output::WriteTimestepFunc write_ts, Output::File file){
    std::cout << "STATIC LINEAR START" << std::endl;

    sol.vec[Solution::DISP].fill(0);

    Solution::assembleExtForce(model, sol, 3);

    Element::FlagVector flags;
    flags[Element::CALC_STIFF_LINEAR] = true;
    Solution::assembleElements(model, sol, flags);

    Solver::ConjugateGradient(sol.mat[Solution::STIFF_LINEAR], 
                              sol.vec[Solution::F_EXT], 
                              sol.vec[Solution::DISP]);

    write_ts(model, sol, 0, 0, file);

    std::cout << "DONE." << std::endl;

    return Status::SUCCESS;
}