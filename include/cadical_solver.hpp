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