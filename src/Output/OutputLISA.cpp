#include "OutputLISA.h"

#include <Util/pugixml.hpp>
#include <Util/Precision.h>
#include <Model/ModelGatherScatter.h>

using std::string;

Status createEmptyNodes(Model::Data const& model, pugi::xml_node& liml8);
Status fillNodeDisplacements(Model::Data const& model, Solution::Data const& sol, pugi::xml_node& liml8);

Status Output::WriteLISA(Model::Data const& model, Solution::Data const& sol, std::string filepath){

    pugi::xml_document xml;

    // Try open file
    auto result = xml.load_file(filepath.c_str());
    if (result.status == pugi::status_file_not_found)
        return Status(Status::FILE_NOT_FOUND, "Could not load XML file " + filepath);
    if (result.status != pugi::status_ok)
        return Status(Status::FILE_NOT_PARSED, "Could not parse XML file " + filepath);

    // Find parent element
    auto liml8 = xml.child("liml8");

    // Remove old solution, make new one
    xml.child("liml8").remove_child("solution");

    // Make new solution tree
    pugi::xml_document temp;
    for (auto child : xml.child("liml8").children())
        temp.append_copy(child);
    xml.child("liml8").append_child("solution");
    for (auto child : temp.children())
        xml.child("liml8").child("solution").append_copy(child);
    pugi::xml_node results = liml8.child("solution").append_child("results");


    createEmptyNodes(model, results);
    fillNodeDisplacements(model, sol, results);

    xml.save_file(filepath.c_str(), "  ");

    return Status::SUCCESS;
}

Status createEmptyNodes(Model::Data const& model, pugi::xml_node& results){
    pugi::xml_node node;
    pugi::xml_attribute name;
    for (Node::Id nid = 0; nid < model.nds.rows(); nid++){
        node = results.append_child("node");
        name = node.append_attribute("nid");
        name.set_value(nid + 1);
    }

    return Status::SUCCESS;
}

Status fillNodeDisplacements(Model::Data const& model, Solution::Data const& sol, pugi::xml_node& results){
    static char* labels[DOF::COUNT] = {"displx", "disply", "displz", "rotx", "roty", "rotz"};

    Node::Id nid;
    DOF::Set nds;
    pugi::xml_attribute att;
    Vector<numeric> u(DOF::COUNT);

    // For every node
    for (pugi::xml_node node : results.children("node")){
        nid = node.attribute("nid").as_int() - 1;
        nds = model.nds(nid);

        // Gather displacements into u
        Model::gatherNodeDOF(model, nid, sol.vec[Solution::DISP], u);

        // Go through every available DOF and fill in the correct value by
        // checking against the NDS
        int i = 0;
        for (int d = DOF::X; d < DOF::COUNT; d++){
            att = node.append_attribute(labels[d]);
            if (DOF::setContains(nds, (DOF::Lone)d))
                att.set_value(u(i));
            else
                att.set_value(0);
            i++;
        }
    }

    return Status::SUCCESS;
}
