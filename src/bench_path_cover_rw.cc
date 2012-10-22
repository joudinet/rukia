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
#ifndef BENCH_PATH_COVER_RW_CC
# define BENCH_PATH_COVER_RW_CC
# include <iostream>
# include <string>

# include <boost/unordered_set.hpp>
# include <boost/functional/hash.hpp>
# include <boost/algorithm/string/join.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/timer.hpp>

# include <rukia/automaton.hh>
# include <rukia/io/dot/load.hh>
# include <rukia/ctable.hh>
# include <rukia/draw.hh>
# include <rukia/count.hh>
# include <rukia/random.hh>
# include <rukia/get_mem_usage.hh>

using namespace rukia;

// This program measures elapsed time and number of paths drawn to
// cover a percentage 'per' of the number of paths (of length n) in a
// graph described in GRAPHVIZ format (.dot)
int main (int argc, char *argv[])
{
  typedef u_i_random<>	ralgo_type;
  typedef random_walks<automaton,ralgo_type>	rw_type;
  typedef boost::unordered_set<std::size_t, boost::hash<std::size_t> > paths_type;

  if (argc != 5)
    {
      std::cerr << "Usage: " << argv[0] << " graph.dot n per filename" << std::endl;
      exit (1);
    }

  // Load graph
  automaton aut;
  io::dot::load (aut, argv[1]);

  unsigned n = atoi (argv[2]);  // path length
  mpz_class nb_paths = count_path<automaton,mpz_class> (aut, n);

 // expected percentage
  mpf_class per (boost::lexical_cast<double> (argv[3]));
  mpf_class nb_exp_paths (per * nb_paths);

  std::ofstream out (argv[4]);
  out << "NbExpPaths: " << nb_exp_paths << std::endl;

  boost::timer timer;
  // No Pre-computation for this algorithm
  double timer_prc =  0;
  
  ralgo_type ralgo;
  ralgo.seed ();
  rw_type draw (aut, ralgo);
  paths_type paths;
 
  // Draw nb_exp_paths different paths.
  timer.restart ();
  mpz_class n_path_drawn = 0;
  while (paths.size () < nb_exp_paths)
    {
      paths.insert (boost::hash_value (draw (n)));
      ++n_path_drawn;
    }
  double timer_drw =  timer.elapsed ();
  if (nb_exp_paths == 0)
    nb_exp_paths = 1;
  out << "UsedMemory: " << proc_mem_usage () << " MB"
      << std::endl;
  out << "ElapsedTime: " << timer_prc + timer_drw << " s"
      << std::endl;
  out << "RatioNbPathDrawnNbExpPaths: "
      << n_path_drawn / nb_exp_paths << std::endl;
  out.close();
}

#endif // ! BENCH_PATH_COVER_RW_CC
