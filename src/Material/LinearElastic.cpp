#include "LinearElastic.h"

Status Material::Linear::Elastic::calculate1D(Model::Data const& model, Material::Id mat_id, numeric eps_11, numeric& sigma_11, numeric& C_11){
    numeric lambda = model.mat_param(mat_id, LAMBDA);
    numeric mu = model.mat_param(mat_id, MU);
    numeric E = mu*(3 * lambda + 2 * mu) / (lambda + mu);
    sigma_11 = E*eps_11;
    C_11 = E;
    return Status::SUCCESS;
}