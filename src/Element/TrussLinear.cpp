#include "Truss.h"

using namespace Element::Truss::Linear;


namespace Element{
    namespace Truss{
        namespace Linear{
            
            enum {AREA, PARAM_COUNT};

            Status calculate(Model::Data const& model, Solution::Data& sol, Element::FlagVector& flags){

                // First and last truss element Ids
                Element::Id first_eid = model.elem_group_offset(Element::TRUSS_LINEAR);
                Element::Id last_eid = model.elem_group_offset(Element::TRUSS_LINEAR + 1);

                Node::Id nidA;              // Node Ids
                Node::Id nidB;

                Node::Pos nodeA;            // Node positions
                Node::Pos nodeB;

                VectorF<numeric, 6> u;      // Displacement vector
                auto u1 = u.head(3);        // Displacement of first node
                auto u2 = u.tail(3);        //                 second node

                // Difference between node positions
                VectorF<numeric, 3> diff_ref;

                Material::Id mat_id;    // Material Id
                numeric A;              // Truss area
                numeric L_ref;          // Reference length

                numeric eps_11;         // Strain
                numeric sig_11;         // Stress
                numeric C_11;           // Tangent

                Vector<numeric> fint_local(2);      // Internal force in local CS
                Vector<numeric> fint_global(6);     // Internal force in global CS

                VectorF<numeric, 3> local_cs;       // Local CS
                MatrixF<numeric, 3, 1> T_node;      // Transformation from local to global CS @ node
                MatrixF<numeric, 6, 2> T;           // Complete transformation matrix
                T.fill(0);

                MatrixF<numeric, 2, 2> ke_local;    // Material Stiffness matrix
                MatrixF<numeric, 6, 6> ke_global;   

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

                    // Local CS
                    local_cs = diff_ref / L_ref;

                    // Local to global transformation @ node
                    CS::calcTransform(local_cs, CS::global_basis_3D, T_node);

                    // Strain (convert displacement to truss CS)
                    eps_11 = (T_node.transpose()*(u2 - u1) / L_ref)(0);

                    // Stress
                    mat_id = model.elem_mat_id(eid);
                    A = model.elem_param(eid, AREA);
                    Material::calculate1D(model, mat_id, eps_11, sig_11, C_11);

                    // Complete transformation matrix
                    T.block<3, 1>(0, 0) = T_node;
                    T.block<3, 1>(3, 1) = T_node;

                    // Calculate internal force
                    if (flags[Element::CALC_INT_FORCE]){
                        fint_local(0) = -sig_11*A;
                        fint_local(1) =  sig_11*A;

                        fint_global.head(3) = T_node*fint_local(0);
                        fint_global.tail(3) = T_node*fint_local(1);

                        // Scatter
                        Model::scatterForce(model, eid, fint_global, sol.vec[Solution::F_INT]);
                    }

                    // Calculate linear stiffness
                    if (flags[Element::CALC_STIFF_LINEAR]){
                        numeric fac = C_11*A / L_ref;
                        ke_local << fac, -fac, -fac, fac;
                        ke_global = T*ke_local*T.transpose();
                        // Scatter
                        Model::scatterStiffness(model, sol, eid, ke_global, sol.mat[Solution::STIFF_LINEAR]);
                    }
                }
                return Status::SUCCESS;
            }
        }
    }
}

