#include "Element.h"

namespace Element{
    Element::TypeData type_data;
}

// Setup basic element information for assembly
Status Element::defineTypes()
{
    // Number of nodes
    type_data.num_nodes.resize(TYPE_COUNT);
    type_data.num_nodes(TRUSS_LINEAR) = 2;

    // Node DOF sets
    type_data.dof_sets.resize(TYPE_COUNT, type_data.num_nodes);
    type_data.dof_sets(TRUSS_LINEAR, 0) = DOF::setCreate(DOF::X, DOF::Y, DOF::Z);
    type_data.dof_sets(TRUSS_LINEAR, 1) = DOF::setCreate(DOF::X, DOF::Y, DOF::Z);

    // Number of parameters
    type_data.num_params.resize(TYPE_COUNT);
    type_data.num_params(TRUSS_LINEAR) = 1;   // Area

    // Calculate number of DOFs/element type
    DOF::Count ndof;
    type_data.num_dofs.resize(TYPE_COUNT);
    for (int t = 0; t < TYPE_COUNT; t++){
        ndof = 0;
        for (Node::Id nid = 0; nid < type_data.num_nodes(t); nid++)
            ndof += DOF::setSize(type_data.dof_sets(t, nid));
        type_data.num_dofs(t) = ndof;
    }
    

    return Status::SUCCESS;
}