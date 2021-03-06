#ifndef __userDS_h__
#define __userDS_h__

#include "Equations_conf.h"
// ===================================
#ifdef DS_EQUATIONS
// ==================================
#include <map>
#include <string>
#include <vector>
#include "Solvertype_enum.h"
class MGUtils;

enum bound_condDS {
  fix_in0 = 1,            ///<  1  normal velocity inlet  \f$ {\bf v}.{\bf n}= 0  \f$
  fix_tg0 = 2,            ///<  2  normal velocity inlet  \f$ {\bf v}.{\bf t}={ 0}{\bf t}  \f$
  fix_disp0 = 3,          ///<  3  normal velocity inlet  \f$ {\bf v}={\bf 0}  \f$
  fix_in = 5,             ///<  5  normal velocity inlet  \f$ {\bf v}.{\bf n}={\bf v}_0.{\bf n}  \f$
  fix_tg = 6,             ///<  6  normal velocity inlet  \f$ {\bf v}.{\bf t}={\bf v}_0.{\bf n}  \f$
  fix_dis = 7,            ///<  7  normal velocity inlet  \f$ {\bf v}={\bf v}_0  \f$
  free_disp = 10,         ///< 10  Neumann homogeneus         (dT.n=0)
  inter = 11,             ///< 11  Interior
  free_disp_outlet = 12,  ///< 12  Neumann homogeneus         (dT.n=0)
  free_wall_turb = 13,    ///< 13   Robin    \f$ \nabla T \cdot \widehat{n}= \beta T \f$  (dT.n=beta*T)
  free_disp_p = 14,       ///< 14  Neumann nonhomogeneus      (dT.n=q_0)
  free_disp_inlet = 16,   ///< 16  Neumann nonhomogeneus      (dT.n=q_0)
  simm_dispx = 21,   ///< 21  simmetry  \f$ (p{\bf n}+{\bf tau} \cdot \widehat{n})\cdot \widehat{i}_x=0 \f$
  simm_dispy = 22,   ///< 22  simmetry  \f$ \nabla T \cdot \widehat{n}=0 \f$    (dT.n=0)
  simm_dispz = 23,   ///< 23  simmetry  \f$ \nabla T \cdot \widehat{n}=0 \f$    (dT.n=0)
  simm_dispxy = 24,  ///< 24  simmetry  \f$ \nabla T \cdot \widehat{n}=0 \f$    (dT.n=0)
  simm_dispxz = 25,  ///< 25  simmetry  \f$ \nabla T \cdot \widehat{n}=0 \f$    (dT.n=0)
  simm_dispyz = 26   ///< 26  simmetry  \f$ \nabla T \cdot \widehat{n}=0 \f$    (dT.n=0)
};

// ============================================================================
/*!  \defgroup DS_param   Class Table:  energy equation parameters (T_param) */
/// \ingroup DS_param
// ============================================================================
class DS_param {
  //< This class defines the physical and numerical  energy equation parameters
 public:
  int _SolveSteady;          /// Flag for steady state solution - value: 0 or 1
  int _Supg;                 /// Flag for SUPG stabilization of advection term - value: 0 or 1
  double _Upwind;            /// Flag for normal upwind stabilization - value: 0 <Up <1
  double _UnderRelaxation;   /// Flag for stabilization based on UnderRelaxation - value between 0 and 1
  int _ReactionNumberBased;  /// Flag for stabilization based on reaction number - value: 0 or 1
  int _FlatProfile;  /// Flag for initial solution: flat profile or modulated as function of wall distance -
                     /// value: 0 or 1
  bool _Solve;       /// Flag for solution of temperature equation
  double _Prt;
  std::vector<int> _BoundaryGroupsIDs;  /// Vector containing boundary group ids
  std::map<int, bound_condDS>
      _map_DSgroup;  /// Map containing boundary group ids and their relative boundary condition
  std::map<std::string, bound_condDS>
      _BoundMap;  /// Map that associates a bound_condT condition to the relative string
  std::map<std::string, std::string> _FileMap;  /// String map containing Tproperties.in parameters
  SolverTypeM _SolverType;                      /// for other solver types see Solvertype_enum.h

 public:
  // constructor --------------------------------------------------------------
  DS_param() {  // SETTING DEFAULT VALUES
    _SolverType = GMRESM;
    _SolveSteady = 0;
    _Supg = 1;
    _Upwind = 0;
    _ReactionNumberBased = 0;
    _FlatProfile = 1;
    _UnderRelaxation = 0.;
    _Prt = 0.85;
    _BoundMap["fix_in0"] = fix_in0;
    _BoundMap["fix_tg0"] = fix_tg0;
    _BoundMap["fix_disp0"] = fix_disp0;
    _BoundMap["fix_in"] = fix_in;
    _BoundMap["fix_tg"] = fix_tg;
    _BoundMap["fix_dis"] = fix_dis;
    _BoundMap["free_disp"] = free_disp;
    _BoundMap["inter"] = inter;
    _BoundMap["free_disp_outlet"] = free_disp_outlet;
    _BoundMap["free_wall_turb"] = free_wall_turb;
    _BoundMap["free_disp_p"] = free_disp_p;
    _BoundMap["free_disp_inlet"] = free_disp_inlet;
    _BoundMap["simm_dispx"] = simm_dispx;
    _BoundMap["simm_dispy"] = simm_dispy;
    _BoundMap["simm_dispz"] = simm_dispz;
    _BoundMap["simm_dispxy"] = simm_dispxy;
    _BoundMap["simm_dispxz"] = simm_dispxz;
    _BoundMap["simm_dispyz"] = simm_dispyz;

  }  // END T_param CONSTRUCTOR

  ~DS_param() {
    _BoundMap.clear();
    _FileMap.clear();
    _map_DSgroup.clear();
    _BoundaryGroupsIDs.clear();
  };

  void read_param(MGUtils& mgutils);  /// This function sets all the T_param parameters
  void read_file();                   /// This function reads the parameters from Tproperties.in file
  void print_par();                   /// This function prints the parameters contained in Tproperties.in
};

#endif
#endif
