// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2008, 2009, 2010, 2018
//
// This file is part of Rukia.
//
// Rukia is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// Rukia is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with Rukia.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef RUKIA_AUTOMATON_HH
# define RUKIA_AUTOMATON_HH
# include <string>
# include <boost/graph/adjacency_list.hpp>
# include <boost/graph/compressed_sparse_row_graph.hpp>

namespace rukia {

  namespace detail {
    struct state {
      unsigned int id;
    };

    struct transition {
      transition(const std::string& l = "") : label(l) {}
      std::string label;
    };
  } // end namespace detail

  typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::directedS,
    detail::state,
    detail::transition>		old_automaton;

  typedef boost::compressed_sparse_row_graph<
    boost::directedS,
    boost::no_property,
    detail::transition>		automaton;

} // end namespace rukia

  // Hash value from an edge_descriptor
namespace boost {
  std::size_t hash_value(const graph_traits<rukia::automaton>::edge_descriptor& e)
  {
    return e.idx;
  }
} // end namespace boost


#endif // ! RUKIA_AUTOMATON_HH
