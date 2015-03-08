#pragma once

#include <Util/Precision.h>
#include <Model/ModelData.h>
#include <Material/Material.h>

namespace Material{
    namespace Linear{
        namespace Elastic{
            enum { LAMBDA, MU, NUM_PARAM };
            Status calculate1D(Model::Data const& model, Material::Id mat_id, numeric eps_11, numeric& sigma_11, numeric& C_11);
        }
    }
}
