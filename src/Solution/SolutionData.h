#pragma once

#include <Util/Precision.h>
#include <Util/Array.h>

namespace Solution{
    struct Data{
        Vector<numeric> u;      // Displacement
        Vector<numeric> v;      // Velocity
        Vector<numeric> a;      // Acceleration

        SparseMatrix<numeric> k_e;    // Linear Stiffness

        Vector<numeric> f_int;   // Internal force
        Vector<numeric> f_ext;   // External force
        Vector<numeric> f_dbc;   // RHS due to Dirichlet BC
    };
}