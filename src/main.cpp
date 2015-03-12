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

int main() {

    FEM::Context fem;
    FEM::init(fem);

    CHECK_STATUS(Input::LoadLISA(fem.input, "test.liml"));

    // Material definitions
    Input::addMaterial(fem.input, Material::LINEAR_ELASTIC, { 1, 1 });

    // Node definitions
    Input::addNode(fem.input, 0, 0, 0);
    Input::addNode(fem.input, 0.25, 0, 0);
    Input::addNode(fem.input, 0.5, 0, 0);
    Input::addNode(fem.input, 0.75, 0, 0);
    Input::addNode(fem.input, 1, 0, 0);

    // Element definition
    Input::addElement(fem.input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 0, 1 });
    Input::addElement(fem.input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 1, 2 });
    Input::addElement(fem.input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 2, 3 });
    Input::addElement(fem.input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 3, 4 });

    // BC Condition
    Input::addDirichletBC(fem.input, 0, DOF::X, 0);
    Input::addDirichletBC(fem.input, 0, DOF::Y, 0);
    Input::addDirichletBC(fem.input, 1, DOF::Y, 0);
    Input::addDirichletBC(fem.input, 2, DOF::Y, 0);
    Input::addDirichletBC(fem.input, 3, DOF::Y, 0);
    Input::addDirichletBC(fem.input, 4, DOF::Y, 0);

    FEM::prepareModel(fem);
    FEM::prepareSolution(fem);

    Element::FlagVector flags;
    flags[Element::CALC_INT_FORCE] = true;
    flags[Element::CALC_STIFF_LINEAR] = true;
    Solution::assemble(fem.model, fem.solution, flags);

    return 0;
}