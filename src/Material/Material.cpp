#include <Material/Material.h>
#include <Material/MaterialCalculate.h>
#include <Material/LinearElastic/LinearElastic.h>
#include <Model/ModelData.h>

namespace Material{
    Material::TypeData type_data;
}

Status Material::defineTypes(){
    Material::type_data.num_params.resize(Material::TYPE_COUNT);
    Material::type_data.num_params(LINEAR_ELASTIC) = Material::Linear::Elastic::NUM_PARAM;

    return Status::SUCCESS;
}

Status Material::calculate1D(Model::Data const& model, Material::Id mat_id, numeric eps_11, numeric& sigma_11, numeric& C_11){

    switch (mat_id){
    case LINEAR_ELASTIC: return Linear::Elastic::calculate1D(model, mat_id, eps_11, sigma_11, C_11);
    }

    return Status::MATERIAL_INVALID_TYPE;
}