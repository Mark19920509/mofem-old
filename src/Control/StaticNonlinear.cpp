#include "StaticNonlinear.h"

#include <FEM/FEM.h>
#include <Solution/SolutionCreate.h>
#include <Solver/CGS.h>

using namespace Control;

Status StaticNonlinear::run(Model::Data& model, Solution::Data& sol, Output::WriteTimestepFunc write_ts, Output::File file){
    
    // What to calculate
    Element::FlagVector flags;
    flags[Element::CALC_STIFF_LINEAR] = true;
    flags[Element::CALC_INT_FORCE] = true;

    // Solve Parameters
    numeric timesteps = model.control_param(StaticNonlinear::TIMESTEPS);
    numeric dt = model.control_param(StaticNonlinear::STEPSIZE);
    numeric tol = model.control_param(StaticNonlinear::NEWTON_TOL);
    int max_iter = (int)model.control_param(StaticNonlinear::NEWTON_MAX_ITER);

    // References to displacement, forces and stiffness matrices
    Vector<numeric>& U = sol.vec[Solution::DISP];
    Vector<numeric>& F_int = sol.vec[Solution::F_INT];
    Vector<numeric>& F_ext = sol.vec[Solution::F_EXT];
    SparseMatrix<numeric>& K_e = sol.mat[Solution::STIFF_LINEAR];

    // Residual and delta-u
    Vector<numeric> R(model.ndof_solve);
    Vector<numeric> delta_U(model.ndof_solve);

    // Initialize displacement
    sol.vec[Solution::DISP].fill(0);

    std::cout << "STATIC NON-LINEAR START\n-----------------------" << std::endl;

    Eigen::ConjugateGradient<SparseMatrix<numeric>, Eigen::Upper> solver;

    int iter;
    numeric res;
    numeric t;
    for (int i = 0; i < timesteps; i++){
        t = i*dt;
        std::cout << "TIME = " << t << std::endl;

        // Assemble, find residual
        Solution::assembleExtForce(model, sol, t);
        Solution::assembleElements(model, sol, flags);
        R = F_ext - F_int;
        res = R.norm();

        iter = 0;
        while (iter < max_iter ){

            // Find new displacement
            Solver::ConjugateGradient(K_e, R, delta_U);
            U += delta_U;

            // Reassemble, find residual
            Solution::assembleElements(model, sol, flags);
            R = F_ext - F_int;
            res = R.norm();

            std::cout << "ITER = " << iter+1 << std::endl;
            std::cout << "RES  = " << res << std::endl;

            // Check residual
            if (res < tol) break;

            iter++;
        }
        write_ts(model, sol, i, t, file);
        std::cout << std::endl;
    }

    std::cout << "DONE." << std::endl;

    return Status::SUCCESS;
}