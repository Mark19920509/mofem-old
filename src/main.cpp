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

#include <Solver/CGS.h>

int main() {

    FEM::Context fem;
    FEM::init(fem);

    CHECK_STATUS(Input::LoadLISA(fem.input, "truss.liml"));

    FEM::prepareModel(fem);
    FEM::prepareSolution(fem);
    fem.solution.u.fill(0);

    Solution::assembleExtForce(fem.model, fem.solution, 3);

    Element::FlagVector flags;
    flags[Element::CALC_INT_FORCE] = true;
    flags[Element::CALC_STIFF_LINEAR] = true;
    Solution::assembleElements(fem.model, fem.solution, flags);

    std::cout << "F_int = " << fem.solution.f_int.transpose() << std::endl;
    std::cout << "F_ext = " << fem.solution.f_ext.transpose() << std::endl;
    std::cout << "K_e = \n" << fem.solution.k_e.toDense() << std::endl;

    Vector<numeric> u(fem.model.ndof_solve);
    Solver::ConjugateGradient(fem.solution.k_e, fem.solution.f_ext, u);
    std::cout << "u = " << u << std::endl;


    return 0;
}