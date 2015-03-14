#include <iostream>
#include <assert.h>

#include <FEM/FEMContext.h>
#include <FEM/FEM.h>
#include <Input/LISA/InputLISA.h>
#include <Output/LISA/OutputLISA.h>
#include <Control/StaticLinear.h>


int main() {
    std::string filepath = "space_struct.liml";

    // FEM context contains the input, model and solution
    FEM::Context fem;
    FEM::init(fem);

    // Load file into the input
    CHECK_STATUS(Input::LISA::Load(fem.input, filepath));
    
    // Prepare output file
    Output::File out_file;
    Output::LISA::Open(filepath, out_file);

    // Start!
    CHECK_STATUS(FEM::run(fem, Output::LISA::WriteTimestep, out_file));

    // Close the output file
    Output::LISA::Close(out_file);

    return 0;
}