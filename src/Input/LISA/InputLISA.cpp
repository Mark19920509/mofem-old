#include "InputLISA.h"

#include <Util/pugixml.hpp>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <math.h>

using std::vector;
using std::tuple;
using std::string;
using std::unordered_map;
using std::unordered_set;

using NodeTuple = tuple < Node::Id, numeric, numeric, numeric > ;
using ElementTuple = tuple < Element::Id, pugi::xml_node, pugi::xml_node >;
using MaterialTuple = tuple < Material::Id, pugi::xml_node >;
using MaterialMap = unordered_map < string, pugi::xml_node >;
using NodeGroups = unordered_map < string, unordered_set<Node::Id>> ;
using ElementGroups = unordered_map < string, unordered_set<Element::Id> > ;

Status createNodeGroups(Input::Data& input, pugi::xml_node& xml, NodeGroups& node_groups);
Status createNodes(Input::Data& input, pugi::xml_node& xml);

Status createElementGroups(Input::Data& input, pugi::xml_node& xml, ElementGroups& elem_groups);
Status createElementTuples(Input::Data& input, pugi::xml_node& xml, vector<ElementTuple>& element_tuples);
Status createElement(Input::Data& input, MaterialMap& mat_map, ElementTuple& et);
Status createTruss(Input::Data& input, MaterialMap& mat_map, ElementTuple& et, vector<Node::Id>& nodes);

Status processGeometric1D(pugi::xml_node geo, numeric& area);

Status createMaterialTuples(Input::Data& input, pugi::xml_node& xml, vector<MaterialTuple>& material_tuples, MaterialMap& material_map);
Status createMaterial(Input::Data& input, MaterialTuple& mt);
Status createLinearElastic(Input::Data& input, MaterialTuple& mt);

Status createDirichletBC(Input::Data& input, pugi::xml_node& liml8, NodeGroups& node_groups);
Status createNeumannBC(Input::Data& input, pugi::xml_node& liml8, NodeGroups& node_groups);

Status Input::LISA::Load(Input::Data& input, std::string filepath){

    pugi::xml_document xml;

    // Try open file
    auto result = xml.load_file(filepath.c_str());
    if (result.status == pugi::status_file_not_found) 
        return Status(Status::FILE_NOT_FOUND, "Could not load XML file " + filepath);
    if (result.status != pugi::status_ok) 
        return Status(Status::FILE_NOT_PARSED, "Could not parse XML file " + filepath);

    auto liml8 = xml.child("liml8");

    NodeGroups node_groups;
    createNodeGroups(input, liml8, node_groups);

    ElementGroups element_groups;
    createElementGroups(input, liml8, element_groups);
    
    createNodes(input, liml8);

    vector<ElementTuple> element_tuples;
    createElementTuples(input, liml8, element_tuples);

    // Material Tuples and material map
    vector<MaterialTuple> material_tuples;
    MaterialMap material_map;
    createMaterialTuples(input, liml8, material_tuples, material_map);

    // Process materials
    for (MaterialTuple mt : material_tuples){
        CHECK_STATUS(createMaterial(input, mt));
    }

    // Process elements
    for (ElementTuple& et : element_tuples){
        CHECK_STATUS(createElement(input, material_map, et));
    }

    // Process BC
    createDirichletBC(input, liml8, node_groups);
    createNeumannBC(input, liml8, node_groups);

    return Status::SUCCESS;
}

Status createNodeGroups(Input::Data& input, pugi::xml_node& liml8, NodeGroups& node_groups){
    // Get all node selections
    for (pugi::xml_node group : liml8.children("nodeselection")){
        string group_name = group.attribute("name").value();
        for (pugi::xml_node node : group.children("node")){
            node_groups[group_name].insert(std::stoi(node.attribute("nid").value()) - 1);
        }
    }
    return Status::SUCCESS;
}

Status createElementGroups(Input::Data& input, pugi::xml_node& liml8, ElementGroups& elem_groups){
    // Get all node selections
    for (pugi::xml_node group : liml8.children("elementselection")){
        string group_name = group.attribute("name").value();
        for (pugi::xml_node elem : group.children("element")){
            elem_groups[group_name].insert(std::stoi(elem.attribute("eid").value()) - 1);
        }
    }
    return Status::SUCCESS;
}

Status createNodes(Input::Data& input, pugi::xml_node& liml8){
    vector<NodeTuple> node_tuples;
    for (pugi::xml_node n : liml8.children("node")){
        Node::Id nid = std::stoi(n.attribute("nid").value()) - 1;
        numeric x = std::stod(n.attribute("x").value());
        numeric y = std::stod(n.attribute("y").value());
        numeric z = std::stod(n.attribute("z").value());
        
        node_tuples.push_back(NodeTuple(nid, x, y, z));
    }

    std::sort(node_tuples.begin(), node_tuples.end());
    for (NodeTuple nt : node_tuples){
        std::cout << "NODE " << std::get<1>(nt) << " " << std::get<2>(nt) << " " << std::get<3>(nt) << std::endl;
        Input::addNode(input, std::get<1>(nt), std::get<2>(nt), std::get<3>(nt));
    }

    return Status::SUCCESS;
}

Status createElementTuples(Input::Data& input, pugi::xml_node& liml8, vector<ElementTuple>& element_tuples){
    for (pugi::xml_node elset : liml8.children("elset")){
        for (pugi::xml_node elem : elset.children("elem")){
            Element::Id eid = std::stoi(elem.attribute("eid").value()) - 1;
            element_tuples.push_back(ElementTuple(eid, pugi::xml_node(elset), pugi::xml_node(elem)));
        }
    }
    std::sort(element_tuples.begin(), element_tuples.end(), [](ElementTuple const &t1, ElementTuple const &t2) { return std::get<0>(t1) < std::get<0>(t2); });

    return Status::SUCCESS;
}

Status createMaterialTuples(Input::Data& input, pugi::xml_node& liml8, vector<MaterialTuple>& material_tuples, MaterialMap& material_map){
    for (pugi::xml_node mat : liml8.children("mat")){
        Material::Id mid = std::stoi(mat.attribute("mid").value());
        string name = mat.attribute("name").value();
        material_map[name] = pugi::xml_node(mat);
        material_tuples.push_back(MaterialTuple(mid, pugi::xml_node(mat)));
    }
    std::sort(material_tuples.begin(), material_tuples.end(), [](MaterialTuple const &t1, MaterialTuple const &t2) { return std::get<0>(t1) < std::get<0>(t2); });

    return Status::SUCCESS;
}

Status createMaterial(Input::Data& input, MaterialTuple& mt){
    pugi::xml_node xml = std::get<1>(mt);
    pugi::xml_node mech = xml.child("mechanical");
    if (!mech.empty()){
        string mat_type = mech.attribute("type").value();
        if (mat_type == "Isotropic"){
            return createLinearElastic(input, mt);
        }
    }

    return Status::SUCCESS;
}

Status createLinearElastic(Input::Data& input, MaterialTuple& mt){
    pugi::xml_node mech = std::get<1>(mt).child("mechanical");

    if (mech.attribute("youngsmodulus").empty()) return{ Status::MATERIAL_INVALID_NUM_PARAMETERS, "E is missing" };
    if (mech.attribute("poissonratio").empty()) return{ Status::MATERIAL_INVALID_NUM_PARAMETERS, "nu is missing" };

    numeric E = std::stod(mech.attribute("youngsmodulus").value());
    numeric nu = std::stod(mech.attribute("poissonratio").value());

    numeric lambda = E*nu / (1+nu) / (1-2*nu);
    numeric mu = E / 2.0 / (1+nu);

    std::cout << "MAT LINEAR ELASTIC " << lambda << " " << mu << std::endl;
    Input::addMaterial(input, Material::LINEAR_ELASTIC, {lambda, mu});
    
    return Status::SUCCESS;
}

Status createElement(Input::Data& input, MaterialMap& mat_map, ElementTuple& et){

    pugi::xml_node elset = std::get<1>(et);
    pugi::xml_node elem = std::get<2>(et);

    string material = elset.attribute("material").value();
    string elem_shape = elem.attribute("shape").value();
    string nodes_str = elem.attribute("nodes").value();

    // Extract nodes from string
    vector<Node::Id> nodes;
    int start = 0;
    int len = 0;
    for (unsigned int i = 0; i < nodes_str.size(); i++){
        if (nodes_str[i] == ' '){
            if (len > 0){
                nodes.push_back(std::stoi(nodes_str.substr(start, len)) - 1);
            }
            start = i + 1;
            len = 0;
        }else{
            len++;
        }
    } 
    if (len > 0){
        nodes.push_back(std::stoi(nodes_str.substr(start, len)) - 1);
    }

    if (elem_shape == "line2"){
        if (elem.attribute("truss").empty()){
            return { Status::NOT_IMPLEMENTED, "Beam element not available" };
        }else{
           return createTruss(input, mat_map, et, nodes);
        }
    }

    return Status::SUCCESS;
}

Status createTruss(Input::Data& input, MaterialMap& mat_map, ElementTuple& et, vector<Node::Id>& nodes){

    Status result;

    // Get parent element set
    pugi::xml_node elset = std::get<1>(et);

    // Get material name
    string mat_name = elset.attribute("material").value();
    if (mat_name == "") 
        return{ Status::LISA_MATERIAL_ERROR, "No material specified for truss element." };

    // Get material xml
    auto material_found = mat_map.find(mat_name);
    if (material_found == mat_map.end()) 
        return{ Status::LISA_MATERIAL_ERROR, "Material " + mat_name + " not found." };
    
    // Material ID
    Material::Id mid = std::stoi(mat_map[mat_name].attribute("mid").value()) - 1;

    // Get geometric information
    pugi::xml_node geo = mat_map[mat_name].child("geometric");
    if (geo.empty()) 
        return{ Status::LISA_GEOMETRY_ERROR, "Material " + mat_name + " does not have required geometric information for truss element." };

    // Get area
    numeric A;
    result = processGeometric1D(geo, A);
    if (result != Status::SUCCESS) return result;

    // Add truss
    std::cout << "ELEM TRUSS_LINEAR " << mid << " " << A << " " << std::to_string(nodes);
    return Input::addElement(input, Element::TRUSS_LINEAR, mid, {A}, nodes);
}


Status processGeometric1D(pugi::xml_node geo, numeric& area){
    string geo_type = geo.attribute("type").value();

    if (geo_type == "CircularBar"){
        numeric d = std::stod(geo.attribute("roundod").value());
        area = M_PI*d*d / 4.0;
    }else if (geo_type == "UniformGeneralSection"){
        area = std::stod(geo.attribute("area").value());
    }else{
        return{ Status::LISA_GEOMETRY_ERROR, "Unknown geometry type " + geo_type };
    }

    return Status::SUCCESS;
}

Status createDirichletBC(Input::Data& input, pugi::xml_node& liml8, NodeGroups& node_groups){
    // Displacements 
    for (pugi::xml_node disp : liml8.children("displacement")){
        string node_group_name = disp.attribute("selection").value();
        auto node_group = node_groups[node_group_name];
        DOF::Lone dof = DOF::Lone(std::stoi(disp.attribute("axis").value())-1);
        numeric val = std::stod(disp.child("value").text().get());
        for (Node::Id nid : node_group){
            std::cout << "DIRICHLET BC " << nid << " " << std::to_string(dof) << " " << val << std::endl;
            Input::addDirichletBC(input, nid, dof, val);
        }
    }

    return Status::SUCCESS;
}

Status createNeumannBC(Input::Data& input, pugi::xml_node& liml8, NodeGroups& node_groups){
    // Forces
    for (pugi::xml_node force : liml8.children("force")){
        string node_group_name = force.attribute("selection").value();
        auto node_group = node_groups[node_group_name];

        vector<vector<Input::TimestepPair>> pairs(3);

        // Find timestep pairs for this node group
        DOF::Id dof_id = 0;
        for (string dof_name : {"x", "y", "z"}){
            for (pugi::xml_node dof : force.children(dof_name.c_str())){
                numeric timestep;
                if (dof.attribute("time").empty()) timestep = 0;
                else timestep = std::stod(dof.attribute("time").value());
                numeric val = std::stod(dof.text().get());
                pairs[dof_id].push_back({timestep, val});
            }
            dof_id++;
        }

        // Make sure timesteps are sorted by time
        for (DOF::Id dof_id = 0; dof_id < 3; dof_id++){
            std::sort(pairs[dof_id].begin(), pairs[dof_id].end(), [](Input::TimestepPair const &t1, Input::TimestepPair const &t2) { return t1.t < t2.t; });
        }
        
        // Add neumann BC to each node in group
        for (Node::Id nid : node_group){
            for (DOF::Id dof_id = 0; dof_id < 3; dof_id++){
                std::cout << "NEUMANN BC " << nid << " " << DOF::setStr(DOF::Lone(dof_id)) << std::endl;
                for (auto tsp : pairs[dof_id]){
                    std::cout << "\t" << tsp.t << " " << tsp.val << std::endl;
                }
                Input::addNeumannBC(input, nid, DOF::Lone(dof_id), pairs[dof_id]);
            }
        }

    }

    return Status::SUCCESS;
}