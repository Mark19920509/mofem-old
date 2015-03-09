
#include <Util/Array.h>
#include <Solution/SolutionCreate.h>
#include <Element/Truss/Truss.h>

Status Solution::init(Model::Data const& model, Solution::Data& sol){

    Element::Count nelem = model.elem_ids.rows();

    // Setup vectors
    sol.u.resize(model.ndof_solve);
    sol.v.resize(model.ndof_solve);
    sol.a.resize(model.ndof_solve);
    sol.f_int.resize(model.ndof_solve);
    sol.f_ext.resize(model.ndof_solve);
    sol.f_dbc.resize(model.ndof_solve);

    clear(sol);

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
    sol.k_e.resize(model.ndof_solve, model.ndof_solve);
    sol.k_e.reserve(triplets.size());
    sol.k_e.setFromTriplets(triplets.begin(), triplets.end());

    return Status::SUCCESS;
}

Status Solution::clear(Solution::Data& sol){
    sol.u.fill(0);
    sol.v.fill(0);
    sol.a.fill(0);

    return Status::SUCCESS;
}

Status Solution::assemble(Model::Data const& model, Solution::Data& sol, Element::FlagVector& flags){

    sol.k_e *= 0;
    sol.f_dbc.fill(0);
    sol.f_int.fill(0);

    Element::Truss::Linear::calculate(model, sol, flags);

    return Status::SUCCESS;
}