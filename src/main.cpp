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


int main() {

    Element::defineTypes();
    Material::defineTypes();

    Input::Data input;

    Status result;

    // Material definitions
    result = Input::addMaterial(input, Material::LINEAR_ELASTIC, { 1, 1 });
    CHECK_STATUS(result);

    // Node definitions
    Input::addNode(input, 0, 0, 0);
    Input::addNode(input, 1, 0, 0);
    Input::addNode(input, 1, 1, 0);
    Input::addNode(input, 0, 1, 0);

    // Element definition
    result = Input::addElement(input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 0, 1 });
    result = Input::addElement(input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 1, 2 });
    result = Input::addElement(input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 2, 3 });
    result = Input::addElement(input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 3, 0 });
    result = Input::addElement(input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 0, 2 });
    CHECK_STATUS(result);

    // BC Condition
    result = Input::addDirichletBC(input, 1, DOF::X, 1);
    result = Input::addDirichletBC(input, 3, DOF::Y, 1);

    Model::Data model;
    
    Model::setupNodeData(input, model);
    Model::setupMaterialData(input, model);
    Model::setupElementData(input, model);

    Model::createNDS(model);
    Model::createNDM(model);
    Model::createEDM(model);

    Model::setupBC(input, model);

    Solution::Data sol;
    Solution::init(model, sol);
    Solution::clear(sol);

    std::cout << sol.ke << std::endl;

    Element::FlagVector flags;
    flags[Element::CALC_INT_FORCE] = true;
    flags[Element::CALC_STIFF_LINEAR] = true;
    Solution::assemble(model, sol, flags);
    
    std::cout << sol.ke << std::endl;
    sol.ke.makeCompressed();
    std::cout << sol.ke << std::endl;

    return 0;
}