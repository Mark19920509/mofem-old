#pragma once

#include <Util/Array.h>
#include <Util/Status.h>
#include <Model/ModelData.h>
#include <Element/Element.h>

namespace Model{
    template<typename D1, typename D2>
    Status gather(Model::Data const& model, Element::Id eid, Eigen::MatrixBase<D1> const& gvec, Eigen::MatrixBase<D2>& lvec){
        assert(lvec.rows() == model.edm.cols(eid));
        for (int i = 0; i < model.edm.cols(eid); i++){
            lvec(i) = gvec(model.edm(eid,i));
        }
        return Status::SUCCESS;
    }

    template<typename D1, typename D2>
    Status scatter(Model::Data const& model, Element::Id eid, Eigen::MatrixBase<D1> const& lvec, Eigen::MatrixBase<D2>& gvec){
        int ndof = model.edm.cols(eid);

        assert(lvec.rows() == ndof);

        for (int i = 0; i < ndof; i++){
            gvec(model.edm(eid, i)) = lvec(i);
        }
        return Status::SUCCESS;
    }

    template<typename D1, typename D2>
    Status scatter(Model::Data const& model, Element::Id eid, Eigen::MatrixBase<D1> const& lmat, SparseMatrix<D2>& gmat){
        int ndof = model.edm.cols(eid);

        assert(lmat.rows() == ndof);
        assert(lmat.cols() == ndof);

        for (int i = 0; i < ndof; i++){
            for (int j = 0; j < ndof; j++){
                gmat.coeffRef(model.edm(eid, i), model.edm(eid, j)) = lmat(i,j);
            }
        }
        return Status::SUCCESS;
    }
}