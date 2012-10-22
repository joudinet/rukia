// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2007, 2009, 2010
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
#ifndef RUKIA_PATH_HH
# define RUKIA_PATH_HH
# include <string>
# include <vector>
# include <rukia/automaton.hh>

namespace rukia {
  typedef boost::graph_traits<automaton>::edge_descriptor	edge_descriptor;

  typedef std::vector<edge_descriptor> path_type;
  typedef std::vector<std::string> trace_type;

  // Get the trace of a path (i.e., labels of each transition)
  template <class Automaton>
  trace_type trace (const path_type& p, const Automaton& aut);
}

# include "path.hxx"
#endif // ! RUKIA_PATH_HH
