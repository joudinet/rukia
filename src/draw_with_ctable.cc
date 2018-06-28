// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2009, 2012, 2018
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
#ifndef RUKIA_DRAW_WITH_CTABLE_CC
# define RUKIA_DRAW_WITH_CTABLE_CC
# include <iostream>
# include <memory>

# include <boost/algorithm/string/join.hpp>

# include <rukia/automaton.hh>
# include <rukia/path.hh>
# include <rukia/io/dot/load.hh>
# include <rukia/ctable.hh>
# include <rukia/draw.hh>
# include <rukia/random.hh>

using namespace rukia;

// This program prints uniformly at random m paths of length n from a
// graph described in GRAPHVIZ format (.dot)
int main (int argc, char *argv[])
{
  typedef u_mpz_random	ralgo_type;
  typedef counting_table<automaton,mpz_class>	ctable_type;
  typedef draw_paths<automaton,ctable_type,ralgo_type>	exact_draw_paths;
  typedef std::unique_ptr<ctable_type>		pctable_type;
  if (argc < 4 || argc > 5)
    {
      std::cerr << "Usage: " << argv[0] << " graph.dot m n [final_states]" << std::endl;
      exit (1);
    }

  // Load graph
  automaton aut;
  io::dot::load (aut, argv[1]);

  // Compute ctable, the number of paths of length n that start from
  // each state
  unsigned n = atoi (argv[3]);

  pctable_type pctable;
  if (argc == 4)
    pctable = std::make_unique<ctable_type>(aut, n);
  else
    pctable = std::make_unique<ctable_type>(aut, n, argv[4]);
	std::cerr << "Number of paths of length " << n << ": "
						<< (*pctable)(vertex (0, aut), n) << std::endl;
  // Draw m paths and print it
  unsigned m = atoi (argv[2]);
  ralgo_type ralgo;
  ralgo.seed ();
  exact_draw_paths draw (aut, *pctable, ralgo);
  for (unsigned i = 0; i < m; ++i)
    {
      trace_type trace = rukia::trace (draw (n), aut);
      std::cout << boost::algorithm::join (trace, " ")
		<< std::endl;
    }
}

#endif // ! RUKIA_DRAW_WITH_CTABLE_CC
