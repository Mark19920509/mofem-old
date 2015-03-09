#pragma once

#include <Util/Array.h>
#include <Util/Status.h>
#include <Model/ModelData.h>
#include <Solution/SolutionData.h>
#include <Element/Element.h>

namespace Model{

    /** Scatter local element stiffness matrix to global stiffness matrix. 
     *  Takes into account existence of Dirichlet BC. Also constructs the required RHS vector for 
     *  Dirichlet BC which are fixed with a non-zero value
     */
    template<typename D1, typename D2>
    Status scatterStiffness(Model::Data const& model, Solution::Data& sol, Element::Id eid, Eigen::MatrixBase<D1> const& local_k, SparseMatrix<D2>& global_k){
        int ndof = model.edm_bc.cols(eid);

        assert(local_k.rows() == ndof);
        assert(local_k.cols() == ndof);

        DOF::Id mapped_i;
        DOF::Id mapped_j;
        for (int i = 0; i < ndof; i++){
            // Check if DOF is fixed
            mapped_i = model.edm_bc(eid, i);
            if (mapped_i == DOF::INVALID_ID) continue;

            for (int j = 0; j < ndof; j++){
                // Check if DOF is fixed. If it is, move it to RHS vector
                mapped_j = model.edm_bc(eid, j);
                if (mapped_j == DOF::INVALID_ID){
                    sol.f_dbc(mapped_i) -= local_k(i, j)*model.dof_value(model.edm(eid,j));
                    continue;
                }

                global_k.coeffRef(mapped_i, mapped_j) += local_k(i, j);
            }
        }
        return Status::SUCCESS;
    }

    /** Scatter local element force vectors to global force vectors.
     *  Takes into account existence of Dirichlet BC.
     */
    template<typename D1, typename D2>
    Status scatterForce(Model::Data const& model, Element::Id eid, Eigen::MatrixBase<D1> const& lvec, Eigen::MatrixBase<D2>& gvec){
        int ndof = model.edm_bc.cols(eid);

        assert(lvec.rows() == ndof);

        DOF::Id mapped_i;
        for (int i = 0; i < ndof; i++){
            mapped_i = model.edm_bc(eid, i);
            if (mapped_i == DOF::INVALID_ID) continue;
            gvec(mapped_i) += lvec(i);
        }
        return Status::SUCCESS;
    }

    /** Gathers displacements from global displacement vector
     *  Takes into account existence of Dirichlet BC. If displacement is not found in the solution vector
     *  the required value is taken from the set value provided in the model. 
     */
    template<typename D1, typename D2>
    Status gatherDisplacement(Model::Data const& model, Element::Id eid, Eigen::MatrixBase<D1> const& global_u, Eigen::MatrixBase<D2>& local_u){
        int ndof = model.edm_bc.cols(eid);

        assert(local_u.rows() == ndof);

        DOF::Id mapped_i;
        for (int i = 0; i < ndof; i++){
            mapped_i = model.edm_bc(eid, i);

            if (mapped_i == DOF::INVALID_ID)
                local_u(i) = model.dof_value(model.edm(eid, i));
            else
                local_u(i) = global_u(mapped_i);
        }
        return Status::SUCCESS;
    }

}