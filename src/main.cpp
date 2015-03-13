#include <iostream>
#include <assert.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <Util/Array.h>
#include <Util/Status.h>

#include <Material/Material.h>
#include <Element/Element.h>
#include <Input/InputData.h>
#include <Input/InputCreate.h>
#include <Model/ModelData.h>
#include <Model/ModelCreate.h>
#include <Solution/SolutionData.h>
#include <Solution/SolutionCreate.h>
#include <CS/CS.h>

#include <FEM/FEMContext.h>
#include <FEM/FEMCreate.h>

#include <Input/InputLISA.h>
#include <Output/OutputLISA.h>

#include <Solver/CGS.h>

int main() {

    FEM::Context fem;
    FEM::init(fem);

    std::string filepath = "space_struct.liml";

    CHECK_STATUS(Input::LoadLISA(fem.input, filepath));

    FEM::prepareModel(fem);
    FEM::prepareSolution(fem);
    fem.solution.u.fill(0);

    Solution::assembleExtForce(fem.model, fem.solution, 3);

    Element::FlagVector flags;
    flags[Element::CALC_INT_FORCE] = true;
    flags[Element::CALC_STIFF_LINEAR] = true;
    Solution::assembleElements(fem.model, fem.solution, flags);

    Vector<numeric> u(fem.model.ndof_solve);
    Solver::ConjugateGradient(fem.solution.k_e, fem.solution.f_ext, fem.solution.u);

    std::cout << fem.solution.k_e << std::endl;

    Output::WriteLISA(fem.model, fem.solution, filepath);

    return 0;
}