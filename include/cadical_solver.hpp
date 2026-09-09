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
  bool set(const std::string& name, int val);
  void append_formula(const std::vector<std::vector<int>>& formula);
  void add_clause(const std::vector<int>& clause);

  // ---- Strict-LIFO clause contexts (CADET satsolver_push/pop analog) ----
  // Emulates removable clause groups on top of CaDiCaL (which has no native
  // push/pop). open_context(ctx_lit) begins a group tagged by the caller-
  // allocated variable `ctx_lit`; every clause added while it is the top
  // context is extended with `ctx_lit` (so it becomes (C | ctx), disabled
  // when ctx is true). solve() automatically assumes -ctx for every live
  // context so tagged clauses stay active. close_context() permanently
  // asserts the unit {ctx}, disabling that context's clauses for good.
  // Contexts nest strictly LIFO. `ctx_lit` MUST be a fresh variable owned by
  // the caller (this class never allocates variables) so numbering stays
  // consistent with clauses added directly.
  void open_context(int ctx_lit);
  void close_context();

  void assume(const std::vector<int>& assumptions);
  void constrain(const std::vector<int>& literals);
  int solve(const std::vector<int>& assumptions);
  int solve();
  int solve_limited(int conflict_limit);
  std::vector<int> get_failed(const std::vector<int>& assumptions);
  std::vector<int> get_values(const std::vector<int>& variables);
  std::vector<int> get_model();
  int val(int variable);
  void phase (int lit);
  void unphase (int lit);

 private:
  void set_assumptions(const std::vector<int>& assumptions);

  CaDiCaL::Solver solver;
  CaDiCaL::Tracer* tracer;

  // Live clause-context literals, outermost first (see open_context).
  std::vector<int> ctx_stack_;

  class CadicalTerminator: public CaDiCaL::Terminator {
   public:
    bool terminate() override;
  };

  static CadicalTerminator terminator;
};

}

#endif // CADICAL_HPP