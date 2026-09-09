#include "cadical_solver.hpp"

#include <unistd.h>
#include <cassert>

#include <iostream>
#include <string>

#include "interrupt.hpp"

namespace cadical_interface {

Cadical::CadicalTerminator Cadical::terminator;

Cadical::Cadical(): tracer(nullptr) {
  solver.connect_terminator(&terminator);
  solver.set("factor", 0);
  solver.set("factorcheck", 0);
}

Cadical::Cadical(CaDiCaL::Tracer* tracer, bool inprocessing): Cadical() {
  solver.set("inprocessing", inprocessing);
  this->tracer = tracer;
  solver.connect_proof_tracer(tracer, true);
}

Cadical::~Cadical() {
  if (tracer) {
    solver.disconnect_proof_tracer(tracer);
  }
  solver.disconnect_terminator();
}

bool Cadical::CadicalTerminator::terminate() {
  return interrupt_handler::interrupted(nullptr);
}

bool Cadical::set(const std::string& name, int val) {
  return solver.set(name.c_str(), val);
}

void Cadical::append_formula(const std::vector<std::vector<int>>& formula) {
  for (const auto& clause: formula) {
    add_clause(clause);
  }
}

void Cadical::add_clause(const std::vector<int>& clause) {
  if (ctx_stack_.empty()) {
    solver.clause(clause);
    return;
  }
  // Tag with the innermost (top) context only. Strict-LIFO nesting means an
  // outer context is closed only after every inner one, so guarding by the
  // top literal alone is enough: close_context on the top disables this
  // clause, and outer closes disable clauses tagged with their own literals.
  std::vector<int> tagged;
  tagged.reserve(clause.size() + 1);
  tagged.insert(tagged.end(), clause.begin(), clause.end());
  tagged.push_back(ctx_stack_.back());
  solver.clause(tagged);
}

void Cadical::open_context(int ctx_lit) {
  ctx_stack_.push_back(ctx_lit);
}

void Cadical::close_context() {
  int ctx = ctx_stack_.back();
  ctx_stack_.pop_back();
  // Permanently satisfy every (C | ctx) clause of this context by forcing
  // ctx true. This introduces ctx to the solver even if no clause used it.
  solver.clause(ctx);
}

void Cadical::assume(const std::vector<int>& assumptions) {
  for (auto l: assumptions) {
    solver.assume(l);
  }
}

void Cadical::constrain(const std::vector<int>& literals) {
  for (auto l: literals) {
    solver.constrain(l);
  }
  solver.constrain(0);
}

void Cadical::set_assumptions(const std::vector<int>& assumptions) {
  solver.reset_assumptions();
  assume(assumptions);
}

int Cadical::solve(const std::vector<int>& assumptions) {
  set_assumptions(assumptions);
  return solve();
}

int Cadical::solve() {
  // Keep every live context's clauses active by assuming -ctx. CaDiCaL clears
  // assumptions after each solve(), so re-issuing here each call is correct
  // and composes with any caller assumptions already set for this solve.
  for (int ctx : ctx_stack_) {
    solver.assume(-ctx);
  }
  int result = solver.solve();
  auto signal = interrupt_handler::interrupted(nullptr);
  if (signal > 1) {
    throw interrupted_exception();
  }
  return result;
}

int Cadical::solve_limited(int conflict_limit) {
  solver.limit("conflicts", conflict_limit);
  return solve();
}

std::vector<int> Cadical::get_failed(const std::vector<int>& assumptions) {
  std::vector<int> failed_literals;
  for (auto l: assumptions) {
    if (solver.failed(l)) {
      failed_literals.push_back(l);
    }
  }
  return failed_literals;
}

std::vector<int> Cadical::get_values(const std::vector<int>& variables) {
  std::vector<int> assignment;
  for (auto v: variables) {
    assignment.push_back(val(v));
  }
  return assignment;
}

std::vector<int> Cadical::get_model() {
  std::vector<int> assignment;
  for (int v = 1; v <= solver.vars(); v++) {
    assignment.push_back(val(v));
  }
  return assignment;
}

int Cadical::val(int variable) {
  auto l = solver.val(variable);
  auto v = abs(l);
  if (v == variable) {
    return l;
  } else {
    return variable;
  }
}

void Cadical::phase(int lit) {
  solver.phase(lit);
}

void Cadical::unphase(int lit) {
  solver.phase(-lit);
}

} // namespace cadical_interface