#pragma once

#include <Util/Precision.h>
#include <Util/Array.h>

namespace Solution{
    struct Data{
        Vector<numeric> u;      // Displacement
        Vector<numeric> v;      // Velocity
        Vector<numeric> a;      // Acceleration

        SparseMatrix<numeric> ke;    // Linear Stiffness

        Vector<numeric> fint;   // Internal force
        Vector<numeric> fext;   // External force
    };
}