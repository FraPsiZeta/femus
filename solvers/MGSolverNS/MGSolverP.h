#ifndef __mgsolverPb_h__
#define __mgsolverPb_h__

#include "Equations_conf.h"
// ===================================
#ifdef NS_EQUATIONS
// ==================================
// config files ------------------

#include "MGFE_conf.h"
#include "MGSolverDA.h"
#include "Pparameters.h"
#include "UserP.h"

// Forward declarations ----------
class MGEquationsSystem;

// =================================================
/// Class for mg energy equation solvers with name T_EQUATIONS.
// Multilevel and mulitporcessor class (see <a href="ns_discretization.pdf"
// target="_blank"><b>Overview</b></a>)
class MGSolP : public MGSolDA {
  // ======================================================================================================
  // ============= MGSolP class data ======================================================================
  // ======================================================================================================
 public:
  P_param _P_parameter;

 private:
  int _nPdim;       //<dimension
  double _dt;       ///< =_mgutils.get_par("dt");
  int _FF_idx[30];  //< field equation flag
  double _euler_impl;
  // parameters--------------------------------------------------------------------------------------------
  // constant reference parameters (From paramater.in file _mgphys.get_par("Uref")) -----------------------
  const double _uref;                 /**< "Uref" */
  const double _lref; /**< "lref" */  // const double _Tref;/**< "Tref" */

  // constant fluid properties (From paramater.in file _mgphys.get_par("Uref")) ---------------------------
  const double _rhof;               /**< "rho0" */
  const double _muf; /**< "mu0" */  // const double _cp0;/**< "cp0" */
                                    //     const double _kappa0;    /**< "kappa0" see paramater.in file */

  // nondimensional numbers -------------------------------------------------------------------------------
  double _alpha; /**< conducibility*/
  double _IPrdl; /**< Prandl*/
  double _IRe;   /**< Reynolds*/
  double _qheat; /** vol source*/
  double _qs;    /**< surface flux*/

  // turbulence
  double _alpha_turb;  /**< turb conducibility*/
  double _IPrdl_turb;  /**< turb Prandl number*/
  double _kappa_g[2];  /**< reference kappa*/
  double _kappaT_g[2]; /**< reference omega */
  double _y_dist;      ///< distance from the wall
  double _sP;          ///< turbulent tensor modulus
  double _nut_ratio;   ///< effective turbulent viscosity

  // mesh -------------------------------------------------------------------------------------------------
  const int _offset;                     ///< = _mgmesh._NoNodes[_NoLevels-1]= mesh nodes
  double _xx_qnds[NDOF_FEM * DIMENSION]; /**< elem coords */
  double _xxb_qnds[NDOF_FEMB * DIMENSION];
  double _xyzg[DIMENSION];
  // -------------------- class field ---------------------------------------------------------------------
  // element boundary conditions
  int _bc_vol[NDOF_FEM]; /**<  b.cond from function */
  int _bc_bd[NDOF_FEM];  /**< b.cond flags */
  int _bc_el[NDOF_FEM];  /**<  b.cond in matrix assemblying */
  // ------------------ integration -----------------------
  //  fields at gaussian points
  double _ub_g[3][30];    /**< external field  (0-1-2 degree)*/
  double _xxg[DIMENSION]; /**< gauss pts*/
  double _InvJac2[DIMENSION * DIMENSION];
  double _InvJac1[DIMENSION * DIMENSION];
  double _ub_dxg[2 * DIMENSION];  ///< external field derivative  (0-1-2 degree)

  double _u_div[NDOF_FEM * DIMENSION];
  double _p_rhs[NDOF_P], _p_1ts[NDOF_P], _p_gdx[DIMENSION];

  bool _AxiSym;
  int _SolveP;
  double _factor;

  int _AssembleOnce;
  int _AlreadyAssembled;
  int _NodeIDrefPressure;

 public:
  // ==========================================================================
  // =========         Constructor - Destructor  ==============================
  // ==========================================================================
  /// This function constructs the 3d-2D MGSolP class
  MGSolP(                                      ///< Constructor
      MGEquationsSystem& mg_equations_map_in,  ///<  mg_equations_map_in pointer
      const int nvars_in[],                    ///< KLQ number of variables
      std::string eqname_in = "NS2P",          ///< equation name
      std::string varname_in = "p"             ///< basic variable name
  );
  // ==========================================================================
  /// This function destructs the 3d-2D MGSolP class
  ~MGSolP() {}  ///< Destructor

  //===========================================================================
  // =========== Read MGSolP functions  =======================================
  // ==========================================================================

  // This function reads the boundary conditions
  void bc_read(  ///< Read bc
      int bc_gam /**< group idx  */, int bc_mat /**< mat idx      */, double xp[] /**< pts coords */,
      int bc_Neu[] /**< bc flags   */, int bc_value[] /**< aux bc flags*/
  );
  // ==========================================================================
  // This function reads the initial solution
  void ic_read(  ///< Read bc
      int bc_gam /**< group idx    */, int bc_mat /**< mat id     */, double xp[] /**< pts coords   */,
      int iel /**< element id */, double u_value[] /**< value vector */
  );                                               ///< Read ic

  /// d)  Assemblying MGSolNS Operators

  // ==============================================================================
  // This function assembles the Volume integral
  void GenMatRhs(
      const double time, /**< time*/ const int Level /**< discrtization Level*/,
      const int mode /**< y/n assembnle rhs */
  );

  // ==============================================================================
  // This function assembles only RHS for finer level
  void GenRhs(
      const double time, /**< time*/ const int Level /**< discrtization Level*/,
      const int mode /**< y/n assembnle rhs */
  );

  // ==========================================================================
  /// This function  computes a time step
  void MGTimeStep(
      const double time,  ///< Time-step manager function
      const int iter);

  void MGTimeStep_no_up(
      const double time,  ///< Time-step manager function
      const int iter);

  void MGUpdateStep();

  // ==========================================================================
  /// This function  computes the  functional defined (user write}
  void MGFunctional(
      const double /*time*/, double /*starting_distance*/
  ) {}

  void get_el_data(int el_ndof[], int el_conn[], int offset);
};

#endif  // define NS_EQUATIONS
#endif  //__mgsolverP_h__
