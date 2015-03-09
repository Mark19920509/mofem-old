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
    Input::addMaterial(input, Material::LINEAR_ELASTIC, { 1, 1 });

    // Node definitions
    Input::addNode(input, 0, 0, 0);
    Input::addNode(input, 0.25, 0, 0);
    Input::addNode(input, 0.5, 0, 0);
    Input::addNode(input, 1, 0, 0);

    // Element definition
    Input::addElement(input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 0, 1 });
    Input::addElement(input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 1, 2 });
    Input::addElement(input, Element::TRUSS_LINEAR, 0, { 1.0 }, { 2, 3 });


    // BC Condition
    Input::addDirichletBC(input, 0, DOF::X, 0);
    Input::addDirichletBC(input, 0, DOF::Y, 0);
    Input::addDirichletBC(input, 1, DOF::Y, 0);
    Input::addDirichletBC(input, 2, DOF::Y, 0);
    Input::addDirichletBC(input, 3, DOF::Y, 0);

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

    Element::FlagVector flags;
    flags[Element::CALC_INT_FORCE] = true;
    flags[Element::CALC_STIFF_LINEAR] = true;
    Solution::assemble(model, sol, flags);

    std::cout << sol.f_int.transpose() << std::endl;
    std::cout << sol.f_dbc.transpose() << std::endl;

    return 0;
}