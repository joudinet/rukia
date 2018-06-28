// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2007, 2018
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
#ifndef RUKIA_CTABLE_HH
# define RUKIA_CTABLE_HH
# include <iostream>
# include <fstream>
# include <vector>
# include <set>
# include <boost/graph/iteration_macros.hpp>

namespace rukia {
  template<class Automaton,
	   class T = double,
	   class VertexIndexMap =
	   typename boost::property_map<Automaton,
					boost::vertex_index_t>::const_type>
  class counting_table
  {
  public:
    using vertex_descriptor	=
      typename boost::graph_traits<Automaton>::vertex_descriptor;
    using value_type		= T;
    using vertex_index_map_type = VertexIndexMap;

    // Assume all final states, so initialize ctable_ values to 1.
    counting_table (const Automaton& aut, unsigned n)
      : ctable_ (num_vertices(aut)*(n+1), 1), n_(n),
	vi_map(get(boost::vertex_index, aut))
    {
      compute_ctable_values (aut);
    }

    // fname file lists final states.
    counting_table (const Automaton& aut, unsigned n, const char* fname)
      : ctable_ (num_vertices(aut)*(n+1), 0), n_(n),
	vi_map(get (boost::vertex_index, aut))
    {
      std::set<unsigned> ids;
      std::ifstream in (fname);
      while (in.good ())
	{
	  unsigned i;
	  in >> i;
	  ids.insert (i);
	}
      BGL_FORALL_VERTICES_T(s, aut, Automaton)
	{
	  auto it = ids.find (get(vi_map, s));
	  if (it != ids.end ())
	    {
	      std::cerr << get(vi_map, s) << " is final." << std::endl;
	      (*this)(s, 0) = 1;
	      ids.erase (it);
	      if (ids.empty ())
		break;
	    }
	}
      compute_ctable_values (aut);
    }

    // vector L0 is the number of paths of length 0
    template <class V>
    counting_table (const Automaton& aut, unsigned n, const std::vector<V>& L0)
      : ctable_ (num_vertices(aut)*(n+1)), n_(n),
	vi_map(get (boost::vertex_index, aut))
    {
      BGL_FORALL_VERTICES_T(s, aut, Automaton)
	(*this)(s, 0) = L0[get(vi_map, s)];
      compute_ctable_values (aut);
    }

    T& operator()(vertex_descriptor s, unsigned n)
    {
      assert (n <= n_);
      return ctable_[get(vi_map, s) * (n_+1) + n];
    }

    T operator()(vertex_descriptor s, unsigned n) const
    {
      assert (n <= n_);
      return ctable_[get(vi_map, s) * (n_+1) + n];
    }

    unsigned size() const { return n_; }

  private:
    // copy and assignment are not allowed
    counting_table (const counting_table& ctable);
    counting_table& operator= (const counting_table& ctable);

    void compute_ctable_values(const Automaton& aut)
    {
      for (unsigned i = 1; i <= n_; ++i)
	{
	  BGL_FORALL_VERTICES_T(s, aut, Automaton)
	    {
	      (*this)(s, i) = 0;
	      BGL_FORALL_OUTEDGES_T(s, e, aut, Automaton)
		(*this)(s, i) += (*this)(target (e, aut), i-1);
	    }
	}
    }

    std::vector<T> ctable_;
    unsigned n_;
    /// A map to get an index from a vertex.
    VertexIndexMap vi_map;
  };
} // end namespace RUKIA

#endif // ! RUKIA_CTABLE_HH
