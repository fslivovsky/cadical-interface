#ifndef CADICAL_HPP
#define CADICAL_HPP

#include <vector>
#include <cstdio>
#include <string>

#include "cadical.hpp"

namespace cadical_interface {

class Cadical {
 public:
  Cadical();
  Cadical(CaDiCaL::Tracer* tracer, bool inprocessing = false);
  ~Cadical();
  void append_formula(const std::vector<std::vector<int>>& formula);
  void add_clause(const std::vector<int>& clause);
  void assume(const std::vector<int>& assumptions);
  int solve(const std::vector<int>& assumptions);
  int solve();
  int solve_limited(int conflict_limit);
  std::vector<int> get_failed(const std::vector<int>& assumptions);
  std::vector<int> get_values(const std::vector<int>& variables);
  std::vector<int> get_model();
  int val(int variable);
  void phase (int lit);
  void unphase (int lit);

  // ====== BEGIN IPASIR-UP ================================================

  // Add call-back which allows to learn, propagate and backtrack based on
  // external constraints. Only one external propagator can be connected
  // and after connection every related variables must be 'observed' (use
  // 'add_observed_var' function).
  // Disconnection of the external propagator resets all the observed
  // variables.
  //
  //   require (VALID)
  //   ensure (VALID)
  //
  void connect_external_propagator (CaDiCaL::ExternalPropagator *propagator);
  void disconnect_external_propagator ();

  // Mark as 'observed' those variables that are relevant to the external
  // propagator. External propagation, clause addition during search and
  // notifications are all over these observed variables.
  // A variable can not be observed without having an external propagator
  // connected. Observed variables are "frozen" internally, and so
  // inprocessing will not consider them as candidates for elimination.
  // An observed variable is allowed to be a fresh variable and it can be
  // added also during solving.
  //
  //   require (VALID_OR_SOLVING)
  //   ensure (VALID_OR_SOLVING)
  //
  void add_observed_var (int var);

  // Removes the 'observed' flag from the given variable. A variable can be
  // set unobserved only between solve calls, not during it (to guarantee
  // that no yet unexplained external propagation involves it).
  //
  //   require (VALID)
  //   ensure (VALID)
  //
  void remove_observed_var (int var);

  // Removes all the 'observed' flags from the variables. Disconnecting the
  // propagator invokes this step as well.
  //
  //   require (VALID)
  //   ensure (VALID)
  //
  void reset_observed_vars ();

  // Get reason of valid observed literal (true = it is an observed variable
  // and it got assigned by a decision during the CDCL loop. Otherwise:
  // false.
  //
  //   require (VALID_OR_SOLVING)
  //   ensure (VALID_OR_SOLVING)
  //
  bool is_decision (int lit);

  // Force solve to backtrack to certain decision level. Can be called only
  // during 'cb_decide' of a connected External Propagator.
  // Invoking in any other time will not have an effect.
  // If the call had an effect, the External Propagator will be notified
  // about the backtrack via 'notify_backtrack'.
  //
  //   require (SOLVING)
  //   ensure (SOLVING)
  //
  void force_backtrack (size_t new_level);

  // ====== END IPASIR-UP ==================================================

 private:
  void set_assumptions(const std::vector<int>& assumptions);

  CaDiCaL::Solver solver;
  CaDiCaL::Tracer* tracer;

  class CadicalTerminator: public CaDiCaL::Terminator {
   public:
    bool terminate() override;
  };

  static CadicalTerminator terminator;
};

}

#endif // CADICAL_HPP