// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2010
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
#ifndef RUKIA_COUNT_HH
# define RUKIA_COUNT_HH
# include <fstream>
# include <rukia/automaton.hh>
# include <boost/graph/iteration_macros.hpp>

namespace rukia {
  
  template <class Automaton, class T>
  T count_path (Automaton aut, unsigned n,
  	   typename boost::graph_traits<Automaton>::vertex_descriptor s,
  	   std::vector<T>& cur_values)
  {
    std::vector<T> next_values (num_vertices (aut));
    
    for (unsigned i = 1; i <= n; ++i)
      {
  	BGL_FORALL_VERTICES_T(s, aut, Automaton)
  	  {
  	    next_values[s] = 0;
  	    BGL_FORALL_OUTEDGES_T(s, e, aut, Automaton)
  	      next_values[s] += cur_values[target (e, aut)];
  	  }
  	cur_values.swap (next_values); 
      }
    return cur_values[s];
  }

  template <class Automaton, class T>
  T count_path (Automaton aut, unsigned n,
	   typename boost::graph_traits<Automaton>::vertex_descriptor s)
  {
    std::vector<T> cur_values (num_vertices (aut), 1);
    return count_path<Automaton,T> (aut, n, s, cur_values);
  }

  template <class Automaton, class T>
  T count_path (Automaton aut, unsigned n, const char* fname)
  {
    std::vector<T> cur_values (num_vertices (aut), 0);
    std::ifstream in (fname);
    while (in.good ())
      {
  	unsigned i;
  	in >> i;
  	cur_values[i] = 1;
      }
    return count_path<Automaton,T> (aut, n, vertex(0, aut), cur_values);
  }

  template <class Automaton, class T>
  T count_path (Automaton aut, unsigned n)
  {
    return count_path<Automaton,T> (aut, n, vertex(0, aut));
  }

} // namespace rukia

#endif // ! RUKIA_COUNT_HH
