// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2010, 2012
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
#ifndef RUKIA_DRAW_HH
# define RUKIA_DRAW_HH
# include <utility>
# include <stack>
# include <rukia/ctable.hh>
# include <rukia/dtable.hh>
# include <rukia/path.hh>

namespace rukia {

  // TODO: Factorize these four classes than make more or less the same thinks.
  // Recursive method
  template <class Automaton, class Ctable, class Random>
  class draw_paths
  {
  public:
    typedef typename boost::graph_traits<Automaton>
    ::vertex_descriptor	vertex_descriptor;

    draw_paths (const Automaton& aut, const Ctable& ctable, Random& ralgo);

    // draw a path of length n
    path_type operator()(unsigned n) const;

  private:
    const Automaton& aut_;
    const Ctable& ctable_;
    Random& ralgo_;
  };

  // Boltzmann generator
  // Version that draw only paths of length n (using reject)
  template <class Automaton, class Coeffs, class Random>
  class boltzmann_generator
  {
  public:
    typedef typename boost::graph_traits<Automaton>
    ::vertex_descriptor	vertex_descriptor;
    typedef typename Coeffs::value_type	value_type;

    boltzmann_generator (const Automaton& aut,
			 const Coeffs& coeffs,
			 const value_type& z,
			 Random& ralgo);

    // draw a path of length n
    path_type operator()(unsigned n) const;

  private:
    const Automaton&	aut_;
    const Coeffs&	coeffs_;
    const value_type&	z_;
    Random&	ralgo_;
  };

  // Isotropic Random Walk 
  template <class Automaton, class Random>
  class random_walks
  {
  public:
    typedef typename boost::graph_traits<Automaton>::vertex_descriptor	vertex_descriptor;

    random_walks (const Automaton& aut, Random& ralgo);

    // draw a path of length n
    path_type operator()(unsigned n) const;

  private:
    const Automaton& aut_;
    Random& ralgo_;
  };

  // Dichopile 
  template <class Automaton, class V, class Random>
  class dichopile
  {
  public:
    typedef typename boost::graph_traits<Automaton>::vertex_descriptor	vertex_descriptor;
    typedef typename std::pair<int, V>	stack_element;

    dichopile (const Automaton& aut, const V& L0, Random& ralgo);

    // draw a path of length n
    path_type operator()(unsigned n) const;

    // Get info about space and time complexities
    size_t nb_calculation () const { return nb_calculation_; }
    size_t max_stack_size () const { return max_stack_size_; }
    size_t mem_size () const { return mem_size_; }

  private:
    const Automaton& aut_;
    Random& ralgo_;
    mutable std::stack<stack_element> stack_;
    mutable size_t nb_calculation_;
    mutable size_t max_stack_size_;
    mutable size_t mem_size_;
  };

  // Dac 
  template <class Automaton, class Dtable, class Random>
  class dac
  {
  public:
    typedef typename boost::graph_traits<Automaton>::vertex_descriptor	vertex_descriptor;

    dac (const Automaton& aut, const Dtable& dtable, Random& ralgo);

    // draw a path of length n
    // For the moment, n should be a power of 2
    path_type operator()(unsigned n) const;

  private:
    const Automaton& aut_;
    const Dtable& dtable_;
    Random& ralgo_;
  };

} // end namespace rukia

# include "draw.hxx"

#endif // ! RUKIA_DRAW_HH
