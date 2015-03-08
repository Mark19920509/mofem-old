#include <Util/Status.h>
#include <Input/InputData.h>
#include <Input/InputCreate.h>

using namespace std;

// Checking functions
Status checkIfValidNodeIDs(Node::Count, vector<Node::Id>&);
Status checkIfValidMaterialID(Material::Count, Material::Id);
Status checkMaterialDefinition(Material::Type, int num_param);
Status checkElementDefinition(Element::Type, Node::Count, int num_param);


Status Input::addNode(Input::Data& input, numeric x, numeric y, numeric z)
{
    input.node_pos.push_back({{ x, y, z }});
    input.node_cs.push_back({ { 1.0, 0.0, 0.0, 
                                0.0, 1.0, 0.0, 
                                0.0, 0.0, 1.0 } });
    return Status::SUCCESS;
}


Status Input::addElement(Input::Data& input, Element::Type elem_type, Material::Id mat_id, vector<numeric> elem_param, vector<Node::Id> node_ids)
{
    Status result;
    Element::Id eid = input.elem_type.size();

    result = checkElementDefinition(elem_type, node_ids.size(), elem_param.size());
    result = { checkIfValidMaterialID(input.mat_type.size(), mat_id), result };
    result = { checkIfValidNodeIDs(input.node_pos.size(), node_ids), result };
    if (result != Status::SUCCESS) return {result, "ElemID: " + to_string(eid)};

    eid = input.elem_type.size();
    input.elem_type.push_back(elem_type);
    input.elem_param.push_back(elem_param);
    input.elem_mat_id.push_back(mat_id);
    input.enm.push_back(node_ids);

    return Status::SUCCESS;
}

Status Input::addMaterial(Input::Data& input, Material::Type mat_type, vector<numeric> mat_param)
{
    Status result;
    Material::Id mid = input.mat_type.size();

    result = checkMaterialDefinition(mat_type, mat_param.size());
    if (result != Status::SUCCESS) return { result, "MatID: " + to_string(mid) };

    input.mat_type.push_back(mat_type);
    input.mat_param.push_back(mat_param);
    return Status::SUCCESS;
}


// Checks if given nodes are valid
Status checkIfValidNodeIDs(Node::Count num_nodes, vector<Node::Id>& node_ids)
{
    for (Node::Id nid : node_ids){
        if (nid >= num_nodes || nid < 0)
            return { Status::NODE_INVALID_ID, "NodeID : " + to_string(nid)};
    }
    return Status::SUCCESS;
}

Status checkIfValidMaterialID(Material::Count num_mat, Material::Id mat_id)
{
    if (mat_id < 0 || mat_id >= num_mat) return{ Status::MATERIAL_INVALID_ID, "MatID: " + to_string(mat_id) };
    return Status::SUCCESS;
}

Status checkElementDefinition(Element::Type tid, Node::Count num_nodes, int num_params)
{
    if (tid >= Element::TYPE_COUNT || tid < 0)
        return Status::ELEMENT_INVALID_TYPE;
    if (Element::type_data.num_nodes(tid) != num_nodes)
        return{ Status::ELEMENT_INVALID_NUM_NODES, "Given: " + to_string(num_nodes) };
    if (Element::type_data.num_params(tid) != num_params)
        return{ Status::ELEMENT_INVALID_NUM_PARAMETERS, "Given: " + to_string(num_params) };
    return Status::SUCCESS;
}

Status checkMaterialDefinition(Material::Type tid, int num_params)
{
    if (tid >= Material::TYPE_COUNT || tid < 0)
        return Status::MATERIAL_INVALID_TYPE;
    if (Material::type_data.num_params(tid) != num_params) 
        return Status::MATERIAL_INVALID_NUM_PARAMETERS;
    return Status::SUCCESS;
}