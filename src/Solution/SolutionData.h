#pragma once

#include <Util/Precision.h>
#include <Util/Array.h>

namespace Solution{

    enum VecTypes { DISP, VEL, ACC, F_INT, F_EXT, F_DBC, VEC_COUNT };
    enum MatTypes { STIFF_MAT, STIFF_GEO, STIFF_ALL, MAT_COUNT };

    struct Data{
        Vector<numeric> vec[VEC_COUNT];
        SparseMatrix<numeric> mat[MAT_COUNT];
    };
}