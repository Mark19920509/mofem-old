#include <Material/Material.h>
#include <Model/ModelData.h>

namespace Material{

    Status calculate1D(Model::Data const& model, Material::Id mat_id, numeric eps_11, numeric& sigma_11, numeric& C_11);

}
