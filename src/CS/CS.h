#pragma once

#include <Util/Precision.h>
#include <Util/Array.h>

namespace CS{

    /** Global 2D Basis */
    extern MatrixF<numeric, 2, 2> const global_basis_2D;


    /** Global 3D Basis */
    extern MatrixF<numeric, 3, 3> const global_basis_3D;


    /**
     *  Calculates the transformation required to express a vector (defined using basis A) using basis B
     */
    template <typename D1, typename D2, typename D3>
    void calcTransform(Eigen::MatrixBase<D1> const& A, Eigen::MatrixBase<D2> const& B, Eigen::MatrixBase<D3>& T){
        T.resize(B.cols(), A.cols());

        for (int i = 0; i < T.rows(); i++){
            for (int j = 0; j < T.cols(); j++){
                T(i, j) = B.col(i).dot( A.col(j) );
            }
        }

    }

}