#include <iostream>
#include <assert.h>

#include <FEM/FEMContext.h>
#include <FEM/FEMCreate.h>
#include <Input/InputLISA.h>
#include <Output/OutputLISA.h>
#include <Control/StaticLinear.h>


int main() {
    std::string filepath = "space_struct.liml";

    FEM::Context fem;
    FEM::init(fem);

    CHECK_STATUS(Input::LoadLISA(fem.input, filepath));

    Control::StaticLinear::run(fem);

    Output::WriteLISA(fem.model, fem.solution, filepath);

    return 0;
}