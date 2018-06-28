// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2010, 2018
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
#ifndef RUKIA_DRAW_HXX
# define RUKIA_DRAW_HXX
# include <iostream>
# include <cmath>
# include <boost/graph/iteration_macros.hpp>
# include "draw.hh"
# include "get_mem_usage.hh"

namespace rukia {
  //
  // Recursive method
  //
  template <class Automaton, class Ctable, class Random>
  draw_paths<Automaton,Ctable,Random>::draw_paths (const Automaton& aut,
						   const Ctable& ctable,
						   Random& ralgo)
    : aut_(aut), ctable_(ctable), ralgo_(ralgo)
  {
  }

  // TODO: vertex_descriptor s should be an argument.
  template <class Automaton, class Ctable, class Random>
  path_type
  draw_paths<Automaton,Ctable,Random>::operator()(unsigned n) const
  {
    typedef typename Ctable::value_type			value_type;

    // TODO: IndexMap should be a parameter
    typedef typename boost::property_map<Automaton,
      boost::vertex_index_t>::type IndexMap;
    IndexMap i_map = get (boost::vertex_index, aut_);

    path_type path (n);
    vertex_descriptor s = vertex (0, aut_);
    unsigned i = n;
    if (ctable_(s,n) == 0)
      {
	std::cerr << "There is no path starting from " << get(i_map, s)
		  << " of length " << n << "!" << std::endl;
	exit(2);
      }
    for (path_type::iterator it = path.begin ();
	 it != path.end (); ++it, --i)
      {
	value_type acc = 0;
	value_type r;
	r = ralgo_(ctable_ (s, i));
	BGL_FORALL_OUTEDGES_T(s, e, aut_, Automaton)
	  {
	    acc += ctable_ (target (e, aut_), i-1);
	    if (acc > r)
	      {
		*it = e;
		s = target (e, aut_);
		break;
	      }
	  }
      }
    return path;
  }

  //
  // Boltzmann generator
  //
  template <class Automaton, class Coeffs, class Random>
  boltzmann_generator<Automaton,Coeffs,Random>
  ::boltzmann_generator (const Automaton& aut,
			 const Coeffs& coeffs,
			 const value_type& z,
			 Random& ralgo)
    : aut_(aut), coeffs_(coeffs), z_(z), ralgo_(ralgo)
  {
  }

  // TODO: vertex_descriptor s should be an argument.
  template <class Automaton, class Coeffs, class Random>
  path_type
  boltzmann_generator<Automaton,Coeffs,Random>::operator()(unsigned n) const
  {
    // TODO: IndexMap should be a parameter
    typedef typename boost::property_map<Automaton,
      boost::vertex_index_t>::type IndexMap;
    IndexMap i_map = get (boost::vertex_index, aut_);

    path_type path (n);
    vertex_descriptor s = vertex (0, aut_);
    unsigned i;
    bool over;
    path_type::iterator path_iter;
    if (out_degree(s, aut_) == 0 || coeffs_[get(i_map, s)] == 0)
      {
	std::cerr << "There is no path starting from " << get(i_map, s)
		  << " of length " << n << "!" << std::endl;
	exit(2);
      }
    do {
      s = vertex (0, aut_);
      i = 0;
      over = false;
      path_iter = path.begin ();
      while (!over)
	{
	  over = true;
	  value_type acc = 0;
	  value_type r;
	  r = ralgo_(coeffs_[get(i_map, s)]);
	  BGL_FORALL_OUTEDGES_T(s, e, aut_, Automaton)
	    {
	      acc += z_ * coeffs_[get(i_map, target (e, aut_))];
	      if (acc > r)
		{
		  ++i;
		  if (path_iter == path.end ())
		    break; // path too long, abort.
		  over = false;
		  *path_iter++ = e;
		  s = target (e, aut_);
		  break;
		}
	    }
	}
    } while (i != n); // path too short, restart.
    return path;
  }

  //
  // Random Walks
  //
  template <class Automaton, class Random>
  random_walks<Automaton,Random>::random_walks (const Automaton& aut,
						Random& ralgo)
    : aut_(aut), ralgo_(ralgo)
  {
  }

  // TODO: vertex_descriptor s should be an argument.
  template <class Automaton, class Random>
  path_type
  random_walks<Automaton,Random>::operator()(unsigned n) const
  {
    typedef typename boost::graph_traits<Automaton>
      ::degree_size_type value_type;

    // TODO: IndexMap should be a parameter
    typedef typename boost::property_map<Automaton,
      boost::vertex_index_t>::type IndexMap;
    IndexMap i_map = get (boost::vertex_index, aut_);

    path_type path (n);
    vertex_descriptor s = vertex (0, aut_);
    unsigned i;
    if (out_degree(s, aut_) == 0)
      {
	std::cerr << "There is no path starting from " << get(i_map, s)
		  << " of length " << n << "!" << std::endl;
	exit(2);
      }
    do {
      i = n;
      for (path_type::iterator it = path.begin ();
	   it != path.end (); ++it, --i)
	{
	  value_type acc = 0;
	  value_type r;
	  if (out_degree(s, aut_) == 0)
	    {
	      std::cerr << "Got a deadlock before reach the end of path. Retrying...\n";
	      break;
	    }
	  r = ralgo_(out_degree (s, aut_));
	  BGL_FORALL_OUTEDGES_T(s, e, aut_, Automaton)
	    {
	      ++acc;
	      if (acc > r)
		{
		  *it = e;
		  s = target (e, aut_);
		  break;
		}
	    }
	}
    } while (i != 0);
    return path;
  }

  template <class Automaton, class V, class Random>
  dichopile<Automaton,V,Random>::dichopile (const Automaton& aut,
					    const V& L0,
					    Random& ralgo)
    : aut_(aut), ralgo_(ralgo)
  {
    stack_.push (std::make_pair (0, L0));
    nb_calculation_ = 0;
    max_stack_size_ = 0;
    nb_calculation_ = 0;
    mem_size_ = 0;
  }

  // TODO: vertex_descriptor s should be an argument.
  template <class Automaton, class V, class Random>
  path_type
  dichopile<Automaton,V,Random>::operator()(unsigned n) const
  {
    typedef typename V::value_type	value_type;
    // TODO: IndexMap should be a parameter
    typedef typename boost::property_map<Automaton,
      boost::vertex_index_t>::type IndexMap;
    IndexMap i_map = get (boost::vertex_index, aut_);

    path_type path (n);
    vertex_descriptor s = *vertices (aut_).first;
    V Lcur, Lsuc, Lpre (num_vertices (aut_));
    nb_calculation_ = 0;
    max_stack_size_ = 0;
    path_type::iterator it = path.begin ();
    for (int i = n; i >= 0; --i)
      {
	int j;
	stack_element elem;
	elem = stack_.top ();
	j = elem.first;
	while (j > i)
	  {
	    // Remove useless values from the stack
	    stack_.pop ();
	    elem = stack_.top ();
	    j = elem.first;
	  }
	Lcur = elem.second;

	// Compute Li from Lj
	while (j < i - 1)
	  {
	    int k = (j+i) / 2; // we will keep Lk
	    for (; j < k; ++j)
	      {
		// Compute next_row (L_{j+1})
		BGL_FORALL_VERTICES_T(s, aut_, Automaton)
		  {
		    Lpre[get (i_map, s)] = 0;
		    BGL_FORALL_OUTEDGES_T(s, e, aut_, Automaton)
		      Lpre[get (i_map, s)] += Lcur[get (i_map, target (e, aut_))];
		  }
		Lcur.swap (Lpre);
		++nb_calculation_;
	      }
	    stack_.push (std::make_pair (k, Lcur));
	    j = k;
	  }
	if (j == i - 1)
	  {
	    // Compute next_row (L_{j+1})
	    BGL_FORALL_VERTICES_T(s, aut_, Automaton)
	      {
		Lpre[get (i_map, s)] = 0;
		BGL_FORALL_OUTEDGES_T(s, e, aut_, Automaton)
		  Lpre[get (i_map, s)] += Lcur[get (i_map, target (e, aut_))];
	      }
	    Lcur.swap (Lpre);
	    ++nb_calculation_;
	    ++j;
	  }
	assert (j == i);
	if (static_cast<unsigned>(i) < n)  // wait until Lsuc is defined
	  {
	    // Choose next edge according to Lsuc and Lcur values
	    value_type acc = 0;
	    value_type r;
	    r = ralgo_(Lsuc[get (i_map, s)]);
	    BGL_FORALL_OUTEDGES_T(s, e, aut_, Automaton)
	      {
		acc += Lcur[get (i_map, target (e, aut_))];
		if (acc > r)
		  {
		    *it++ = e;
		    s = target (e, aut_);
		    break;
		  }
	      }
	  }
	Lsuc = Lcur; // Keep the previous value of Lcur for the choosing step
	if (stack_.size () > max_stack_size_)
	  {
	    // std::cerr << "nb lassos: "
	    // 	      << Lcur[get (i_map, *vertices(aut_).first)]
	    // 	      << std::endl;
	    max_stack_size_ = stack_.size ();
	    mem_size_ = proc_mem_usage ();
	    // return path; // I JUST WANT THE MEMORY CONSUMPTION
	  }
      }
    // std::cerr << "MaxStackSize: " << max_stack_size_ << std::endl;
    // std::cerr << "NbCalculation: " << nb_calculation_ << std::endl;
    return path;
  }

  //
  // Divide and Conquer method
  //
  template <class Automaton, class Dtable, class Random>
  dac<Automaton,Dtable,Random>::dac (const Automaton& aut,
				     const Dtable& dtable,
				     Random& ralgo)
    : aut_(aut), dtable_(dtable), ralgo_(ralgo)
  {
  }

  // TODO: vertex_descriptor s0 should be an argument.
  template <class Automaton, class Dtable, class Random>
  path_type
  dac<Automaton,Dtable,Random>::operator()(unsigned n) const
  {
    typedef typename Dtable::value_type			value_type;

    path_type path (n);
    std::vector<vertex_descriptor> path_states (n+1);
    vertex_descriptor s0 = vertex (0, aut_);
    path_states[0] = s0;
    unsigned k = Dtable::lg (n);

    // choice of the final state
    value_type dsum = 0;
    BGL_FORALL_VERTICES_T (s, aut_, Automaton)
      {
	dsum += dtable_ (s0, s, k);
      }
    value_type acc = 0;
    value_type r = ralgo_(dsum);
    BGL_FORALL_VERTICES_T (s, aut_, Automaton)
      {
	acc += dtable_ (s0, s, k);
	if (acc > r)
	  {
	    path_states[n] = s;
	    break;
	  }
      }

    // choice of other states by dichotomic
    for (unsigned i = 1; i <= k; ++i)
      {
	unsigned m = std::pow (2, k-i);
	for (unsigned j = 1; j <= std::pow (2, i-1); ++j)
	  {
	    vertex_descriptor s1 = path_states[(2*j-2)*m];
	    vertex_descriptor s2 = path_states[2*j*m];
	    value_type r = ralgo_(dtable_(s1, s2, k-i+1));
	    value_type acc = 0;
	    BGL_FORALL_VERTICES_T (s, aut_, Automaton)
	      {
		acc += dtable_ (s1, s, k-i) * dtable_ (s, s2, k-i);
		if (acc > r)
		  {
		    path_states[(2*j-1)*m] = s;
		    break;
		  }
	      }
	  }
      }

    // Choice of each transition
    unsigned i = 1;
    for (path_type::iterator it = path.begin ();
    	 it != path.end (); ++it, ++i)
      {
	// Choose transition between path_states[i-1] and
	// path_states[i]

	//Hack, for convienient access, I always choose the first edge
	// between u and v but if there are multiple edges, it should
	// choose uniformly at random among them.
	assert (i <= n);
	std::pair<edge_descriptor, bool> p = boost::edge (path_states[i-1], path_states[i], aut_);
	 // The CSR graph loader may assume vertices are sorted
	// If it is not the case, the following assertion could failed
	assert (p.second);
	*it = p.first;
      }
    return path;
  }


} // end namespace rukia

#endif // ! RUKIA_DRAW_HXX
