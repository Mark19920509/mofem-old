#include "OutputLISA.h"

#include <iostream>
#include <Util/pugixml.hpp>
#include <Util/Precision.h>
#include <Model/ModelGatherScatter.h>
#include <Control/StaticNonlinear.h>

using std::string;

enum {TIMESTEP_ID, COUNT};

Status createEmptyNodes(Model::Data const& model, pugi::xml_node& liml8);
Status fillNodeDisplacements(Model::Data const& model, Solution::Data const& sol, pugi::xml_node& liml8);

Status Output::LISA::Open(Model::Data const& model, std::string filepath, Output::File& out){

    // Setup XML file
    out.filepath = filepath;
    out.file.xml = new pugi::xml_document;
    pugi::xml_document& xml = *out.file.xml;

    // Try open file
    auto result = xml.load_file(filepath.c_str());
    if (result.status == pugi::status_file_not_found)
        return Status(Status::FILE_NOT_FOUND, "Could not load XML file " + filepath);
    if (result.status != pugi::status_ok)
        return Status(Status::FILE_NOT_PARSED, "Could not parse XML file " + filepath);

    // LIML8 is the parent element
    pugi::xml_node liml8 = xml.child("liml8");

    // Remove old solution to make way for the new one below
    xml.child("liml8").remove_child("solution");

    // The solution node contains all information that appears under the parent element, for some
    // reason. Here we copy everything from the parent element down to the solution subtree.
    pugi::xml_document temp;
    for (auto child : xml.child("liml8").children())
        temp.append_copy(child);
    xml.child("liml8").append_child("solution");
    for (auto child : temp.children())
        xml.child("liml8").child("solution").append_copy(child);

    // Results section
    pugi::xml_node results = liml8.child("solution").append_child("results");

    // Build empty timesteps for nonlinear control 
    if (model.control_type == Control::STATIC_NONLINEAR){

        int timesteps = (int) model.control_param(Control::StaticNonlinear::TIMESTEPS);
        numeric dt = model.control_param(Control::StaticNonlinear::STEPSIZE);

        // Create timesteps
        numeric t = 0;
        for (int i = 0; i < timesteps; i++){
            pugi::xml_node timestep = results.append_child("timestep");
            timestep.append_attribute("id").set_value(i);
            timestep.append_attribute("time").set_value(t);
            t += dt;
        }

    }


    return Status::SUCCESS;
}

Status Output::LISA::WriteTimestep(Model::Data const& model, Solution::Data const& sol, int timestep_id, numeric timestep, Output::File& out){
    // Get results section
    pugi::xml_node results = (*out.file.xml).child("liml8").child("solution").child("results");

    if (model.control_type == Control::STATIC_LINEAR){
        createEmptyNodes(model, results);
        fillNodeDisplacements(model, sol, results);

    // Write timesteps for nonlinear solution
    }else if (model.control_type == Control::STATIC_NONLINEAR){
        pugi::xml_node timestep = results.find_child_by_attribute("timestep", "id", std::to_string(timestep_id).c_str());
        createEmptyNodes(model, timestep);
        fillNodeDisplacements(model, sol, timestep);
    }

    return Status::SUCCESS;
}

Status Output::LISA::Close(Output::File& out){
    pugi::xml_document& xml = *out.file.xml;
    xml.save_file(out.filepath.c_str(), "  ");
    delete out.file.xml;

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
