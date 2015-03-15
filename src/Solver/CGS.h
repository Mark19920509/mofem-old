#pragma once

#include <Util/Precision.h>
#include <Util/Status.h>
#include <Util/Array.h>

namespace Solver{

    Status ConjugateGradient(SparseMatrix<numeric>& A, Vector<numeric>& b, Vector<numeric>& x, numeric tol = 1e-6, unsigned int max_iter = 0);

}