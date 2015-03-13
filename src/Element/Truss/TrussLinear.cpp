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

                VectorF<numeric, 4> u;      // Displacement vector
                auto u1 = u.head(2);        // Displacement of first node
                auto u2 = u.tail(2);        //                 second node

                // Difference between node positions
                VectorF<numeric, 2> diff_ref;
                VectorF<numeric, 2> diff_act;

                Material::Id mat_id;    // Material Id
                numeric A;              // Truss area
                numeric L_ref;          // Reference length
                numeric L_act;          // Actual length

                numeric eps_11;         // Strain
                numeric sig_11;         // Stress
                numeric C_11;           // Tangent

                Vector<numeric> fint_local(2);      // Internal force in local CS
                Vector<numeric> fint_global(4);     // Internal force in global CS

                VectorF<numeric, 2> local_cs;       // Local CS
                MatrixF<numeric, 2, 1> T_node;      // Transformation from local to global CS @ node
                MatrixF<numeric, 4, 2> T;           // Complete transformation matrix
                T.fill(0);

                MatrixF<numeric, 2, 2> ke_local;    // Material Stiffness matrix
                MatrixF<numeric, 4, 4> ke_global;   

                for (Element::Id eid = first_eid; eid < last_eid; eid++){
                    // Nodes
                    nidA = model.enm(eid, 0);
                    nidB = model.enm(eid, 1);

                    // Gather displacements
                    // Note:  u1 and u2 are "pointers" to u, so they don't need to be updated
                    Model::gatherElementDOF(model, eid, sol.u, u);

                    // Reference length
                    diff_ref = (model.node_pos.row(nidB) - model.node_pos.row(nidA)).head(2);
                    L_ref = diff_ref.norm();

                    // Current length
                    L_act = (diff_ref + (u2 - u1)).norm();

                    // Local CS
                    local_cs = diff_ref / L_ref;

                    // Strain
                    eps_11 = (L_act - L_ref) / L_ref;

                    // Stress
                    mat_id = model.elem_mat_id(eid);
                    A = model.elem_param(eid, AREA);
                    Material::calculate1D(model, mat_id, eps_11, sig_11, C_11);

                    // Local to global transformation @ node
                    CS::calcTransform(local_cs, CS::global_basis_2D, T_node);

                    // Complete transformation matrix
                    T.block<2, 1>(0, 0) = T_node;
                    T.block<2, 1>(2, 1) = T_node;

                    // Calculate internal force
                    if (flags[Element::CALC_INT_FORCE]){
                        fint_local(0) = -sig_11*A;
                        fint_local(1) =  sig_11*A;

                        fint_global.head(2) = T_node*fint_local(0);
                        fint_global.tail(2) = T_node*fint_local(1);

                        // Scatter
                        Model::scatterForce(model, eid, fint_global, sol.f_int);
                    }

                    // Calculate linear stiffness
                    if (flags[Element::CALC_STIFF_LINEAR]){
                        numeric fac = C_11*A / L_ref;
                        ke_local << fac, -fac, -fac, fac;
                        ke_global = T*ke_local*T.transpose();

                        // Scatter
                        Model::scatterStiffness(model, sol, eid, ke_global, sol.k_e);
                    }

                }
                return Status::SUCCESS;
            }
        }
    }
}

