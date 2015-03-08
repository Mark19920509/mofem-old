#pragma once

#include <Util/Status.h>
#include <Util/Array.h>
#include <Util/CompileType.h>
#include <DOF/DOF.h>
#include <Node/Node.h>

namespace Element{
    /** Element types available */
    enum Type { TRUSS_LINEAR, TYPE_COUNT };

    /** Flags to instruct element routines what is required */
    enum CalcFlags { CALC_INT_FORCE, CALC_STIFF_LINEAR, FLAG_COUNT };

    using Id = int;
    using Count = int;
    using FlagVector = std::array < bool, FLAG_COUNT > ;

    /** Most basic element information required */
    struct TypeData{
        ArrayVC<DOF::Set> dof_sets;   
        Array<Node::Count> num_nodes; 
        Array<int> num_params;   
        Array<DOF::Count> num_dofs;  
    };
    extern Element::TypeData type_data;

    /** Element types are defined here */
    Status defineTypes();
}
