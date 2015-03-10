#pragma once

#include <Input/InputData.h>
#include <Model/ModelData.h>
#include <Solution/SolutionData.h>

namespace FEM{

    struct Context{
        Input::Data input;
        Model::Data model;
        Solution::Data solution;
    };

}