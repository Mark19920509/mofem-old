#include "InputLISA.h"

#include <Util/pugixml.hpp>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_map>

using std::vector;
using std::tuple;
using std::string;
using std::unordered_map;

using StringMap = unordered_map < string, string > ;
using NodeTuple = tuple < Node::Id, numeric, numeric, numeric > ;
using ElementTuple = tuple < Element::Id, StringMap> ;

StringMap convertNodeToMap(pugi::xml_node const& node);

Status Input::LoadLISA(Input::Data& input, std::string filepath){

    pugi::xml_document xml;

    // Try open file
    auto result = xml.load_file(filepath.c_str());
    if (result.status == pugi::status_file_not_found) 
        return Status(Status::FILE_NOT_FOUND, "Could not load XML file " + filepath);
    if (result.status != pugi::status_ok) 
        return Status(Status::FILE_NOT_PARSED, "Could not parse XML file " + filepath);

    auto liml8 = xml.child("liml8");

    // Find all nodes
    vector<NodeTuple> node_tuples;
    for (pugi::xml_node n : liml8.children("node")){
        Node::Id nid = std::stoi(n.attribute("nid").value()) - 1;
        numeric x = std::stod(n.attribute("x").value());
        numeric y = std::stod(n.attribute("y").value());
        numeric z = std::stod(n.attribute("z").value());
        node_tuples.push_back(NodeTuple(nid, x, y, z));
    }

    // Sort by ID then add nodes
    std::sort(node_tuples.begin(), node_tuples.end());
    for (NodeTuple nt : node_tuples){
        Input::addNode(input, std::get<0>(nt), std::get<1>(nt), std::get<2>(nt));
        std::cout << "Adding node " << std::get<0>(nt) << " , " << std::get<1>(nt) << " , " << std::get<2>(nt) << std::endl;
    }

    // Element Tuples
    // The tuples contain string mapping which will then be interpreted later
    vector<ElementTuple> element_tuples;
    for (pugi::xml_node elset : liml8.children("elset")){
        for (pugi::xml_node elem : elset.children("elem")){
            Element::Id eid = std::stoi(elem.attribute("eid").value()) - 1;
            StringMap map = convertNodeToMap(elem);
            map["material"] = elset.attribute("material").value();
            element_tuples.push_back(ElementTuple(eid, map));
        }
    }

    // Sort element tuples by element ID
    std::sort(element_tuples.begin(), element_tuples.end(), [](ElementTuple const &t1, ElementTuple const &t2) { return std::get<0>(t1) < std::get<0>(t2); });


    return Status::SUCCESS;
}


StringMap convertNodeToMap(pugi::xml_node const& node){
    StringMap map;
    for (pugi::xml_attribute att : node.attributes()) map[att.name()] = att.value();
    return map;
}