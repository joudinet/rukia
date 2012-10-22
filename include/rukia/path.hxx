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
#ifndef RUKIA_PATH_HXX
# define RUKIA_PATH_HXX
# include "path.hh"

namespace rukia {
  template <class Automaton>
  trace_type trace (const path_type& p, const Automaton& aut)
  {
    trace_type t;
    t.reserve (p.size ());
    for (path_type::const_iterator it = p.begin ();
	 it != p.end (); ++it)
      t.push_back (aut[*it].label);
    return t;
  }
} // namespace rukia

#endif // ! RUKIA_PATH_HXX
