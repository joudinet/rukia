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
#ifndef RUKIA_DTABLE_HH
# define RUKIA_DTABLE_HH
# include <iostream>
# include <fstream>
# include <vector>
# include <set>
# include <boost/graph/iteration_macros.hpp>

namespace rukia {
    static const int MultiplyDeBruijnBitPosition2[32] = 
      {
	0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
	31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
      };

  template<class Automaton, class T = double,
	   class IndexMap = typename boost::property_map<Automaton, boost::vertex_index_t>::const_type>
  class dac_table
  {
  public:
    typedef typename boost::graph_traits<Automaton>::vertex_descriptor	vertex_descriptor;
    typedef typename boost::graph_traits<Automaton>::vertices_size_type	vertices_size_type;
    typedef T	value_type;
  
    dac_table (const Automaton& aut, unsigned n)
      : k_(lg(n)),
	dtable_(num_vertices(aut)*num_vertices(aut)*(k_+1), 0),
	i_map(get(boost::vertex_index, aut)),
	q_(num_vertices(aut))
    {
      BGL_FORALL_EDGES_T(e, aut, Automaton)
	{
	  (*this)(source(e,aut), target(e,aut), 0) += 1;
	}
      
      for (unsigned i = 1; i <= k_; ++i)
	{
	  BGL_FORALL_VERTICES_T(s, aut, Automaton)
	    {
	      BGL_FORALL_VERTICES_T(t, aut, Automaton)
		{
		  (*this)(s, t, i) = 0;
		  BGL_FORALL_VERTICES_T(v, aut, Automaton)
		    {
		      (*this)(s, t, i) += (*this)(s, v, i-1) * (*this)(v, t, i-1);
		    }
		}
	    }
	}
    }

    T operator()(vertex_descriptor s, vertex_descriptor t, unsigned k) const
    {
      assert (k <= k_);
      return dtable_[get(i_map, s) * q_ * (k_+1) + get(i_map, t) * (k_+1) + k];
    }

    // unsigned size() const { return k_; }

    // Fast computation of lg(n)
    static unsigned lg(unsigned n)
    {
      return MultiplyDeBruijnBitPosition2[(uint32_t)(n * 0x077CB531U) >> 27];
    }

  private:
    // Edit values is allowed only in the constructor of this class
    T& operator()(vertex_descriptor s, vertex_descriptor t, unsigned k)
    {
      assert (k <= k_);
      return dtable_[get(i_map, s) * q_ * (k_+1) + get(i_map, t) * (k_+1) + k];
    }
    
    // copy and assignment are not allowed
    dac_table (const dac_table&);
    dac_table& operator= (const dac_table&);

    unsigned k_;
    std::vector<T> dtable_;
    const IndexMap& i_map;
    vertices_size_type q_;
  };
} // end namespace RUKIA

#endif // ! RUKIA_DTABLE_HH
