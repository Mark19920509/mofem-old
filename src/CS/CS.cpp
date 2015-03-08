#include "CS.h"
#include <iostream>

MatrixF<numeric, 3, 3> const CS::global_basis_3D = MatrixF<numeric, 3, 3>::Identity();
MatrixF<numeric, 2, 2> const CS::global_basis_2D = MatrixF<numeric, 2, 2>::Identity();
