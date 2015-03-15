
#include <Util/Array.h>
#include <Solution/SolutionCreate.h>
#include <Element/Truss.h>

Status Solution::allocateMemory(Model::Data const& model, Solution::Data& sol){

    Element::Count nelem = model.elem_ids.rows();

    // Setup vector sizes
    for (int i = 0; i < VEC_COUNT; i++){
        sol.vec[i].resize(model.ndof_solve);
    }

    // Create sparse matrix structure
    DOF::Id mapped_i;
    DOF::Id mapped_j;
    auto edm = model.edm_bc;
    TripletVector<numeric> triplets;
    for (Element::Id eid = 0; eid < nelem; eid++){
        for (DOF::Id i = 0; i < edm.cols(eid); i++){
            mapped_i = edm(eid, i);
            if (mapped_i == DOF::INVALID_ID) continue;
            for (DOF::Id j = 0; j < edm.cols(eid); j++){
                mapped_j = edm(eid, j);
                if (mapped_j == DOF::INVALID_ID) continue;
                triplets.push_back(Triplet<numeric>(mapped_i, mapped_j, 1.0));
            }
        }
    }

    // Setup matrices
    for (int i = 0; i < MAT_COUNT; i++){
        sol.mat[i].resize(model.ndof_solve, model.ndof_solve);
        sol.mat[i].reserve(triplets.size());
        sol.mat[i].setFromTriplets(triplets.begin(), triplets.end());
    }

    return Status::SUCCESS;
}

Status Solution::assembleElements(Model::Data const& model, Solution::Data& sol, Element::FlagVector& flags){
    sol.mat[STIFF_LINEAR] *= 0;
    sol.vec[F_DBC].fill(0);
    sol.vec[F_INT].fill(0);

    Element::Truss::Linear::calculate(model, sol, flags);

    return Status::SUCCESS;
}

Status Solution::assembleExtForce(Model::Data const& model, Solution::Data& sol, numeric t){
    auto& table_time = model.force_time;
    auto& table_value = model.force_value;
    auto& f_ext = sol.vec[F_EXT];

    // Interpolation variables
    numeric start_val; 
    numeric end_val;
    numeric start_time; 
    numeric end_time;
    numeric lambda;

    int ncols;
    DOF::Id post_dbc_did;
    for (DOF::Id did = 0; did < model.ndof; did++){
        ncols = table_time.cols(did);
        post_dbc_did = model.bcdm(did);

        // This DOF has a Dirichlet BC on it
        if (post_dbc_did == DOF::INVALID_ID) continue;

        // No force
        if (ncols == 0){
            f_ext(post_dbc_did) = 0;
            continue;
        }

        // Single value
        if (ncols == 1){
            f_ext(post_dbc_did) = table_value(did, 0);
            continue;
        }

        // Multiple values, interpolate linearly
        int start_idx = 0;
        int end_idx = 0;
        for (int i = 0; i < ncols; i++){
            if (t >= table_time(did, i)){
                start_idx = i;
                end_idx = i;
            }
            else if (t < table_time(did, i)){
                end_idx = i;
                break;
            }
        }

        // Interpolate 
        start_val = table_value(did, start_idx); 
        end_val = table_value(did, end_idx);
        start_time = table_time(did, start_idx);
        end_time = table_time(did, end_idx);
        if (start_time == end_time) lambda = 0.5;
        else lambda = (t-start_time)/(end_time-start_time);
        f_ext(post_dbc_did) = (1 - lambda) * start_val + lambda * end_val;
    }

    return Status::SUCCESS;
}
