#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "cadical_solver.hpp"

namespace py = pybind11;
using namespace cadical_interface;

PYBIND11_MODULE(cadical_py, m) {
  py::class_<Cadical>(m, "cadical")
    .def(py::init<>())
    .def("append_formula", &Cadical::append_formula, "Append a CNF formula")
    .def("add_clause", &Cadical::add_clause, "Add a clause")
    .def("assume", &Cadical::assume, "Set assumptions")
    .def("solve", py::overload_cast<const std::vector<int>&>(&Cadical::solve), "Solve with assumptions")
    .def("solve", py::overload_cast<>(&Cadical::solve), "Solve without assumptions")
    .def("solve_limited", &Cadical::solve_limited, "Solve with a conflict limit")
    .def("get_failed", &Cadical::get_failed, "Get failed assumptions")
    .def("get_values", &Cadical::get_values, "Get values of given variables")
    .def("get_model", &Cadical::get_model, "Get the full model")
    .def("val", &Cadical::val, "Get the value of a variable");
}
