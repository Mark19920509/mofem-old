
#include <Util/Array.h>
#include <Solution/SolutionCreate.h>
#include <Element/Truss/Truss.h>

Status Solution::init(Model::Data const& model, Solution::Data& sol){

    DOF::Count ndof = model.ndof;
    Element::Count nelem = model.elem_ids.rows();

    // Setup vectors
    sol.u.resize(ndof);
    sol.v.resize(ndof);
    sol.a.resize(ndof);
    sol.fint.resize(ndof);
    sol.fext.resize(ndof);

    // Create sparse matrix structure
    int nnz = 0;
    TripletVector<numeric> triplets;
    for (Element::Id eid = 0; eid < nelem; eid++){
        for (DOF::Id i = 0; i < model.edm.cols(eid); i++){
            for (DOF::Id j = 0; j < model.edm.cols(eid); j++){
                triplets.push_back(Triplet<numeric>(model.edm(eid, i), model.edm(eid, j), 1.0));
                nnz++;
            }
        }
    }

    // Setup matrices
    sol.ke.resize(model.ndof, model.ndof);
    sol.ke.reserve(nnz);
    sol.ke.setFromTriplets(triplets.begin(), triplets.end());

    return Status::SUCCESS;
}

Status Solution::clear(Solution::Data& sol){
    sol.u.fill(0);
    sol.v.fill(0);
    sol.a.fill(0);
    sol.fint.fill(0);
    sol.fext.fill(0);
    return Status::SUCCESS;
}

Status Solution::assemble(Model::Data const& model, Solution::Data& sol, Element::FlagVector& flags){
    sol.fint.fill(0.0);

    Element::Truss::Linear::calculate(model, sol, flags);

    return Status::SUCCESS;
}