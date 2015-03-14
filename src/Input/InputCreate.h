#pragma once

#include <Util/Precision.h>
#include <Element/Element.h>
#include <Material/Material.h>
#include <Control/Control.h>
#include <vector>

namespace Input
{
    // Nodes
    Status addNode(Input::Data& input, 
                   numeric x, numeric y, numeric z);

    // Dirichlet BC
    Status addDirichletBC(Input::Data& input,
                          Node::Id nid,
                          DOF::Lone dof,
                          numeric val);

    // Neumann BC
    Status addNeumannBC(Input::Data& input,
                        Node::Id nid,
                        DOF::Lone dof,
                        std::vector<TimestepPair> table);

    // Element input handlers
    Status addElement(Input::Data& input,
                      Element::Type elem_type,
                      Material::Id mat_id,
                      std::vector<numeric> elem_param,
                      std::vector<Node::Id> node_ids);

    // Material input handlers
    Status addMaterial(Input::Data& input,
                       Material::Type mat_type,
                       std::vector<numeric> mat_param);

    // Control type and parameter
    Status setControl(Input::Data& input,
                      Control::Type control_type,
                      std::vector<numeric> control_param);

}
