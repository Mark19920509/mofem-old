#pragma once

#include <Util/Precision.h>
#include <Node/Node.h>
#include <Element/Element.h>
#include <Material/Material.h>

#include <vector>
#include <array>

namespace Input
{
    struct Data
    {
        // Node Data
        std::vector<std::array<numeric, 3>> node_pos;
        std::vector<std::array<numeric, 9>> node_cs;

        // Element data
        std::vector<Element::Type> elem_type;
        std::vector<std::vector<Node::Id>> enm;
        std::vector<std::vector<numeric>> elem_param;
        std::vector<Material::Id> elem_mat_id;

        // Material data
        std::vector<Material::Type> mat_type;
        std::vector<std::vector<numeric>> mat_param;
    };

}

