#pragma once

#include <Util/Precision.h>
#include <Util/Array.h>
#include <DOF/DOF.h>
#include <Material/Material.h>
#include <Element/Element.h>
#include <Node/Node.h>

namespace Model
{
    struct Data{
        // Node data
        MatrixF<numeric, Dynamic, 3> node_pos;   // Node positions
        MatrixF<numeric, Dynamic, 9> node_cs;    // Node coordinate systems

        // DOF Data
        Vector<char> is_dof_fixed;
        Vector<numeric> dof_value;

        // Element data
        Array<Element::Id> elem_group_offset;   // Starting position of every element group (elements are stored in type groups)
        Array<Element::Id> elem_ids;            // Element Global ID
        Array<Element::Type> elem_type;         // Element Type
        ArrayVC<numeric> elem_param;            // Element Parameters
        Array<Material::Id> elem_mat_id;        // Element 

        // Material data
        Array<Material::Type> mat_type;         // Material type
        ArrayVC<numeric> mat_param;             // Material parameters

        Array<DOF::Set> nds;      // Node DOF Sets
        ArrayVC<DOF::Id> ndm;     // Node DOF Map
        ArrayVC<Node::Id> enm;    // Element Node Map
        ArrayVC<Node::Id> edm;    // Element DOF Map

        // Maps modified for Dirichlet BC
        ArrayVC<Node::Id> ndm_bc;  
        ArrayVC<Node::Id> edm_bc;

        // Number of DOFs
        DOF::Count ndof;
        DOF::Count ndof_fixed;
        DOF::Count ndof_solve;
    };

}