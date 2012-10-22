// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2009
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
#ifndef RUKIA_EXACT_DRAW_WITH_BOLTZ_CC
# define RUKIA_EXACT_DRAW_WITH_BOLTZ_CC
# include <iostream>

# include <boost/algorithm/string/join.hpp>

# include <rukia/automaton.hh>
# include <rukia/path.hh>
# include <rukia/io/dot/load.hh>
# include <rukia/draw.hh>
# include <rukia/random.hh>

using namespace rukia;

// PUT IN ANOTHER PLACE!
static double boltz_load_coeff (std::vector<double>& v,
				const std::string& filename)
{
  std::ifstream in (filename.c_str(), std::ios::in);
  unsigned ind;
  double z;
  in >> z;
  double val;
  for (unsigned i = 0; i < v.size (); ++i)
    {
      in >> ind;
      in >> val;
      assert (ind < v.size ());
      assert (val > 0);
      v[ind] = val;
    }
  return z;
}

// This program prints uniformly at random m paths of length n from a
// graph described in GRAPHVIZ format (.dot)
int main (int argc, char *argv[])
{
  typedef u_r_random<>	ralgo_type;
  typedef std::vector<double>			boltzmann_coeffs;
  typedef boltzmann_generator<
  automaton, boltzmann_coeffs, ralgo_type>	exact_draw_paths;

  if (argc != 5)
    {
      std::cerr << "Usage: " << argv[0] << " graph.dot m n coeffs" << std::endl;
      exit (1);
    }

  // Load graph
  automaton aut;
  io::dot::load (aut, argv[1]);

  unsigned m = atoi (argv[2]);

  // Load Boltzmann coeff (Should be computed!)
  unsigned n = atoi (argv[3]);
  boltzmann_coeffs boltz_coeffs (num_vertices (aut));
  double z = boltz_load_coeff (boltz_coeffs, argv[4]);
  

  // Draw m paths and print it
  ralgo_type ralgo;
  ralgo.seed ();
  exact_draw_paths draw (aut, boltz_coeffs, z, ralgo);
  for (unsigned i = 0; i < m; ++i)
    {
      trace_type trace = rukia::trace (draw (n), aut);
      std::cout << boost::algorithm::join (trace, " ")
		<< std::endl;
    }
}

#endif // ! RUKIA_EXACT_DRAW_WITH_BOLTZ_CC
