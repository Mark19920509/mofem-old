#include "FEMCreate.h"

#include <Element/Element.h>
#include <Material/Material.h>

#include <Model/ModelCreate.h>
#include <Solution/SolutionCreate.h>

Status FEM::init(FEM::Context& fem){
    CHECK_STATUS( Element::defineTypes() );
    CHECK_STATUS( Material::defineTypes() );

    return Status::SUCCESS;
}

Status FEM::prepareModel(Context& fem){
    // Copy node, material, element data
    CHECK_STATUS( Model::setupNodeData(fem.input, fem.model) );
    CHECK_STATUS( Model::setupMaterialData(fem.input, fem.model) );
    CHECK_STATUS( Model::setupElementData(fem.input, fem.model) );

    // Create the DOF signature and (node/element) DOF maps using the element node map
    CHECK_STATUS( Model::createNDS(fem.model) );
    CHECK_STATUS( Model::createNDM(fem.model) );
    CHECK_STATUS( Model::createEDM(fem.model) );

    // Setup BC Data - this needs to be done after DOF maps are created as we need to know 
    // how many DOFs there are
    CHECK_STATUS( Model::setupBC(fem.input, fem.model) );

    // Create the adjusted DOF maps which take into account presence of Dirichlet BC
    CHECK_STATUS( Model::createBCDM(fem.model) );
    CHECK_STATUS( Model::createAdjustedNDM(fem.model) );
    CHECK_STATUS( Model::createAdjustedEDM(fem.model) );

    return Status::SUCCESS;
}

Status FEM::prepareSolution(Context& fem){
    CHECK_STATUS( Solution::allocateMemory(fem.model, fem.solution) );

    return Status::SUCCESS;
}