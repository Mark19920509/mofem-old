#pragma once

#include <Util/Precision.h>
#include <Util/Array.h>

namespace Solution{

    enum VecTypes { DISP, VEL, ACC, F_INT, F_EXT, F_DBC, VEC_COUNT };
    enum MatTypes { STIFF_LINEAR , MAT_COUNT };

    struct Data{
        Vector<numeric> vec[VEC_COUNT];
        SparseMatrix<numeric> mat[MAT_COUNT];

        //Vector<numeric> u;      // Displacement
        //Vector<numeric> v;      // Velocity
        //Vector<numeric> a;      // Acceleration

        //SparseMatrix<numeric> k_e;    // Linear Stiffness

        //Vector<numeric> f_int;   // Internal force
        //Vector<numeric> f_ext;   // External force
        //Vector<numeric> f_dbc;   // RHS due to Dirichlet BC
    };
}