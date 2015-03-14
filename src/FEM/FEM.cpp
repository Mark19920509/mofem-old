#include "FEM.h"

#include <Element/Element.h>
#include <Material/Material.h>

#include <Model/ModelCreate.h>
#include <Solution/SolutionCreate.h>

#include <Control/StaticLinear.h>

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
    CHECK_STATUS( Model::setupControl(fem.input, fem.model) );

    // Create the DOF signature and (node/element) DOF maps using the element node map
    CHECK_STATUS( Model::createNDS(fem.model) );
    CHECK_STATUS( Model::createNDM(fem.model) );
    CHECK_STATUS( Model::createEDM(fem.model) );

    // Setup BC Data - this needs to be done after DOF maps are created as we need to know 
    // how many DOFs there are
    CHECK_STATUS( Model::setupDirichletBC(fem.input, fem.model) );
    CHECK_STATUS( Model::setupNeumannBC(fem.input, fem.model) );

    // Create the adjusted DOF maps which take into account presence of Dirichlet BC
    CHECK_STATUS( Model::createBCDM(fem.model) );
    CHECK_STATUS( Model::createAdjustedNDM(fem.model) );
    CHECK_STATUS( Model::createAdjustedEDM(fem.model) );

    return Status::SUCCESS;
}

Status FEM::prepareSolution(Context& context){
    CHECK_STATUS( Solution::allocateMemory(context.model, context.solution) );

    return Status::SUCCESS;
}

Status FEM::run(Context& fem, Output::WriteTimestepFunc write_ts, Output::File file){

    FEM::prepareModel(fem);
    FEM::prepareSolution(fem);

    switch (fem.model.control_type){
    case Control::STATIC_LINEAR: 
        return Control::StaticLinear::run(fem.model, fem.solution, write_ts, file);
    }

    return{ Status::NOT_IMPLEMENTED, "Control type " + std::to_string(fem.model.control_type) + " not implemented"};
}