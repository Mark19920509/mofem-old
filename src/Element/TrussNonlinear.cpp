#include "Truss.h"

using namespace Element::Truss::Linear;


namespace Element{
    namespace Truss{
        namespace Nonlinear{
            
            enum {AREA, PARAM_COUNT};

            Status calculate(Model::Data const& model, Solution::Data& sol, Element::FlagVector& flags){

                // First and last truss element Ids
                Element::Id first_eid = model.elem_group_offset(Element::TRUSS_NONLINEAR);
                Element::Id last_eid = model.elem_group_offset(Element::TRUSS_NONLINEAR + 1);

                Node::Id nidA;              // Node Ids
                Node::Id nidB;

                Node::Pos nodeA;            // Node positions
                Node::Pos nodeB;

                VectorF<numeric, 6> u;      // Displacement vector
                auto u1 = u.head(3);        // Displacement of first node
                auto u2 = u.tail(3);        //                 second node

                // Difference between node positions
                VectorF<numeric, 3> diff_ref;
                VectorF<numeric, 3> diff_act;

                Material::Id mat_id;    // Material Id
                numeric A;              // Truss area
                numeric L_ref;          // Reference length
                numeric L_act;          // Actual length

                numeric E_11;           // Green-Lagrange Strain 
                numeric S_11;           // Second Piola-Kirchoff Stress
                numeric C_11;           // Tangent

                Vector<numeric> fint_local(2);      // Internal force in local CS
                Vector<numeric> fint_global(6);     // Internal force in global CS

                VectorF<numeric, 3> local_cs;       // Local CS
                MatrixF<numeric, 3, 1> T_node;      // Transformation from local to global CS @ node
                MatrixF<numeric, 6, 2> T;           // Complete transformation matrix
                T.fill(0);

                MatrixF<numeric, 2, 2> k_local;    // Stiffness matrix
                MatrixF<numeric, 6, 6> k_global;   


                for (Element::Id eid = first_eid; eid < last_eid; eid++){
                    // Nodes
                    nidA = model.enm(eid, 0);
                    nidB = model.enm(eid, 1);

                    // Gather displacements
                    // Note:  u1 and u2 are "pointers" to u, so they don't need to be updated
                    Model::gatherElementDOF(model, eid, sol.vec[Solution::DISP], u);

                    // Reference length
                    diff_ref = model.node_pos.row(nidB) - model.node_pos.row(nidA);
                    L_ref = diff_ref.norm();

                    // Actual length
                    diff_act = diff_ref + u2 - u1;
                    L_act = diff_act.norm();

                    // Local CS
                    local_cs = diff_act / L_act;

                    // Local to global transformation @ node
                    CS::calcTransform(local_cs, CS::global_basis_3D, T_node);

                    // Green Lagrange Strain
                    E_11 = 0.5*(L_act*L_act - L_ref*L_ref) / (L_ref*L_ref);

                    // Stress
                    mat_id = model.elem_mat_id(eid);
                    Material::calculate1D(model, mat_id, E_11, S_11, C_11);

                    // Complete transformation matrix
                    T.block<3, 1>(0, 0) = T_node;
                    T.block<3, 1>(3, 1) = T_node;

                    // Area
                    A = model.elem_param(eid, AREA);

                    // Calculate internal force
                    if (flags[Element::CALC_INT_FORCE]){
                        fint_local(0) = -S_11*A;
                        fint_local(1) =  S_11*A;

                        fint_global.head(3) = T_node*fint_local(0);
                        fint_global.tail(3) = T_node*fint_local(1);

                        // Scatter
                        Model::scatterForce(model, eid, fint_global, sol.vec[Solution::F_INT]);
                    }

                    // Calculate material stiffness
                    if (flags[Element::CALC_STIFF_MAT]){
                        numeric fac = C_11*A / L_ref;
                        k_local << fac, -fac, -fac, fac;
                        k_global = T*k_local*T.transpose();

                        // Scatter
                        Model::scatterStiffness(model, sol, eid, k_global, sol.mat[Solution::STIFF_MAT]);
                    }

                    // Calculate geometric stiffness
                    if (flags[Element::CALC_STIFF_GEO]){
                        numeric fac = S_11*A / L_ref;
                        k_global = fac*MatrixF<numeric, 6, 6>::Identity();
                        k_global.block<3, 3>(0, 3) = -fac*MatrixF<numeric, 3, 3>::Identity();
                        k_global.block<3, 3>(3, 0) = -fac*MatrixF<numeric, 3, 3>::Identity();

                        //std::cout << k_global << std::endl << std::endl;

                        // Scatter
                        Model::scatterStiffness(model, sol, eid, k_global, sol.mat[Solution::STIFF_GEO]);
                    }
                }
                return Status::SUCCESS;
            }
        }
    }
}

