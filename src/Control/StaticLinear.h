#pragma once

#include <Util/Status.h>
#include <FEM/FEMContext.h>


namespace Control{

    namespace StaticLinear{
        Status run(FEM::Context& fem);
    }

}