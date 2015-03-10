#include "ModelCreate.h"

#include <Util/Array.h>
#include <Element/Element.h>
#include <vector>

Status Model::setupNodeData(Input::Data& input, Model::Data& model)
{
    copy(input.node_pos, model.node_pos);
    copy(input.node_cs, model.node_cs);
    return Status::SUCCESS;
}

Status Model::setupElementData(Input::Data& input, Model::Data& model)
{
    using namespace std;

    // Elements are sorted into their types
    Element::Count num_elem = input.elem_type.size();
    vector<int> type_perm(num_elem);
    for (int i = 0; i < num_elem; i++) type_perm[i] = i;
    std::sort(type_perm.begin(), type_perm.end(), [&](int i, int j) { return input.elem_type[i] < input.elem_type[j]; });

    // Sort all other information by element type
    copy(type_perm, model.elem_ids);
    copy(input.elem_type, model.elem_type, type_perm);
    copy(input.elem_mat_id, model.elem_mat_id, type_perm);
    copy(input.elem_param, model.elem_param, type_perm);
    copy(input.enm, model.enm, type_perm);

    // Calculate element group offset
    Element::Id eid = 0;
    model.elem_group_offset.resize(Element::TYPE_COUNT+1);
    for (int t = 0; t < Element::TYPE_COUNT; t++){
        while (model.elem_type(eid) != t) eid++;
        model.elem_group_offset(t) = eid;
    }
    model.elem_group_offset(Element::TYPE_COUNT) = model.elem_ids.rows();

    return Status::SUCCESS;
}

Status Model::setupMaterialData(Input::Data& input, Model::Data& model)
{
    copy(input.mat_type, model.mat_type);
    copy(input.mat_param, model.mat_param);
    return Status::SUCCESS;
}

Status Model::setupBC(Input::Data& input, Model::Data& model){
    model.ndof_fixed = 0;
    model.is_dof_fixed = Vector<char>::Zero(model.ndof);
    model.dof_value = Vector<numeric>::Zero(model.ndof);

    for (Input::DirichletBC bc : input.dirichlet_bc){
        Node::Id nid = std::get<0>(bc);
        DOF::Lone dof = std::get<1>(bc);
        numeric val = std::get<2>(bc);

        if (DOF::setContains(model.nds(nid), dof)){
            int i = DOF::setIndexOf(model.nds(nid), dof);
            if (i >= 0){
                DOF::Id did = model.ndm(nid, i);
                if (!model.is_dof_fixed(did)){
                    model.is_dof_fixed(did) = true;
                    model.ndof_fixed++;
                }
                model.dof_value(did) = val;
            }
        }
    }
    model.ndof_solve = model.ndof - model.ndof_fixed;

    return Status::SUCCESS;
}

// Create node DOF Signatures
Status Model::createNDS(Model::Data& model)
{
    Element::Count num_elems = (Element::Count)model.elem_ids.rows();
    Node::Count num_nodes = (Node::Count)model.node_pos.rows();

    // Calculate size of array, fill with empty
    model.nds.resize(num_nodes);
    for (Node::Id nid = 0; nid < num_nodes; nid++)
        model.nds(nid) = DOF::SET_EMPTY;

    // Calculate the DOF signatures
    Node::Id nid;
    DOF::Set node_dof_sig;
    for (Element::Id eid = 0; eid < num_elems; eid++){
        for (Node::Id local_nid = 0; local_nid < model.enm.cols(eid); local_nid++){
            nid = model.enm(eid, local_nid);
            node_dof_sig = Element::type_data.dof_sets(model.elem_type(eid), local_nid);
            model.nds(nid) = DOF::setUnion(model.nds(nid), node_dof_sig);
        }
    }

    return Status::SUCCESS;
}


// Create node DOF map
Status Model::createNDM(Model::Data& model)
{
    using std::vector;

    Node::Count num_nodes = (Node::Count) model.node_pos.rows();

    // Number of DOFs per node
    vector<int> num_dof_per_node(num_nodes);
    for (int i = 0; i < model.nds.rows(); i++)
        num_dof_per_node[i] = DOF::setSize(model.nds(i));

    // Build Node DOF Map
    model.ndm.resize(num_nodes, num_dof_per_node);
    DOF::Id did = 0;
    for (Node::Id nid = 0; nid < num_nodes; nid++){
        for (int i = 0; i < model.ndm.cols(nid); i++){
            model.ndm(nid, i) = did;
            did++;
        }
    }

    // Set total number of DOFs
    model.ndof = did;

    return Status::SUCCESS;
}


// Create element DOF map
Status Model::createEDM(Model::Data& model)
{
    using std::vector;

    int num_elems = static_cast<int>(model.enm.rows());

    // Get number of DOFs per element
    vector<DOF::Count> dofs_per_element(num_elems);
    for (Element::Id eid = 0; eid < num_elems; eid++){
        dofs_per_element[eid] = Element::type_data.num_dofs(model.elem_type(eid));
    }

    // Build Element DOF Map
    model.edm.resize(num_elems, dofs_per_element);
    DOF::Id local_did;
    Node::Id nid;
    for (Element::Id eid = 0; eid < num_elems; eid++){
        local_did = 0;
        for (Node::Id local_nid = 0; local_nid < model.enm.cols(eid); local_nid++){
            nid = model.enm(eid, local_nid);
            for (int i = 0; i < model.ndm.cols(nid); i++){
                model.edm(eid, local_did) = model.ndm(nid, i);
                local_did++;
            }
        }
    }

    return Status::SUCCESS;
}


// Create mapping between original DOFs to new DOF Ids adjusted for presence of Dirichlet BC
Status Model::createBCDM(Model::Data& model){
    // Mapping to DOF ids taking into account BC
    model.bcdm.resize(model.ndof);
    DOF::Id did = 0;
    for (DOF::Id i = 0; i < model.ndof; i++){
        if (model.is_dof_fixed(i)){
            model.bcdm(i) = DOF::INVALID_ID;
            continue;
        }
        model.bcdm(i) = did;
        did++;
    }

    return Status::SUCCESS;
}

// Create NDM adjusted for Dirichlet BC
// DOF Ids are mapped to new ones. Fixed DOF Ids map to DOF::INVALID_ID
Status Model::createAdjustedNDM(Model::Data& model){
    model.ndm_bc.resize(model.ndm.rows(), model.ndm.cols());
    // Copy NDM to NDM_BC applying BCDM mapping
    for (Node::Id nid = 0; nid < model.ndm_bc.rows(); nid++){
        for (int i = 0; i < model.ndm_bc.cols(nid); i++)
            model.ndm_bc(nid, i) = model.bcdm(model.ndm(nid, i));
    }

    return Status::SUCCESS;
}

// EDM adjusted for Dirichlet BC
// DOF Ids are mapped to new ones. Fixed DOF Ids map to DOF::INVALID_ID
Status Model::createAdjustedEDM(Model::Data& model){
    model.edm_bc.resize(model.edm.rows(), model.edm.cols());

    // Copy EDM to EDM_BC applying BCDM mapping
    for (Element::Id eid = 0; eid < model.edm_bc.rows(); eid++){
        for (int i = 0; i < model.edm_bc.cols(eid); i++)
            model.edm_bc(eid, i) = model.bcdm(model.edm(eid, i));
    }

    return Status::SUCCESS;
}