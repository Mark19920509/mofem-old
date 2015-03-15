#include "CGS.h"

Status Solver::ConjugateGradient(SparseMatrix<numeric>& A, Vector<numeric>& b, Vector<numeric>& x, numeric tol /*= 1e-4*/, unsigned int max_iter /*= 0*/){
    // Default max iterations = 2*n 
    if (max_iter == 0) max_iter = 2 * b.size();

    // Initial guess is zero
    x.fill(0);

    Vector<numeric> r;
    Vector<numeric> p;
    Vector<numeric> Ap;
    numeric alpha;
    numeric beta;
    numeric rr;
    numeric error = std::numeric_limits<numeric>::max();

    r = b - A*x;
    p = r;
    error = r.norm();

    unsigned int iter = 0;
    while (error > tol && iter <= max_iter){
        Ap = A*p;
        rr = r.dot(r);
        alpha = rr / p.dot(Ap);
        x = x + alpha*p;
        r = r - alpha*Ap;
        beta = r.dot(r) / rr;
        p = r + beta*p;
        error = r.norm();
        iter++;
    }

    return Status::SUCCESS;

}