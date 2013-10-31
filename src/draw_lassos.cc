// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2010, 2013
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
#ifndef RUKIA_LASSOS_CC
# define RUKIA_LASSOS_CC
# include <iostream>
# include <fstream>
# include <set>

# include <boost/graph/depth_first_search.hpp>
# include <boost/graph/filtered_graph.hpp>
# include <boost/algorithm/string/join.hpp>
# include <boost/timer.hpp>

# include <rukia/automaton.hh>
# include <rukia/path.hh>
# include <rukia/io/dot/load.hh>
# include <rukia/ctable.hh>
# include <rukia/draw.hh>
# include <rukia/random.hh>
# include <rukia/get_mem_usage.hh>

using namespace rukia;

# ifndef EXACT
#  define _VTYPE mpf_class
# else
#  define _VTYPE mpz_class
# endif

//========================================================================
// backedge_recorder
template <class CounterMap, class EdgeSet, class Tag>
  struct backedge_recorder
  : public boost::base_visitor<backedge_recorder<CounterMap, EdgeSet, Tag> >
  {
    typedef Tag event_filter;
    backedge_recorder(CounterMap pa, EdgeSet& pb) : m_count(pa), m_set(pb) { }
    template <class Edge, class Graph>
    void operator()(Edge e, const Graph& g) {
      typename boost::graph_traits<Graph>::vertex_descriptor
        u = source(e, g);
      // Increment the counter of backedges starting from u
      put(m_count, u, get(m_count, u) + 1);
      // Add e to the set of backedges
      m_set.insert (e);
    }
    CounterMap m_count;
    EdgeSet& m_set;
  };
template <class CounterMap, class EdgeSet, class Tag>
backedge_recorder<CounterMap, EdgeSet, Tag>
record_backedges(CounterMap pa, EdgeSet& pb, Tag) {
  return backedge_recorder<CounterMap, EdgeSet, Tag> (pa, pb);
  }

//========================================================================
// dfs_lasso_visitor
// template < typename VMap, typename CMap, typename Iter>
// class dfs_lasso_visitor:public default_dfs_visitor {
//   typedef typename property_traits < VMap >::value_type V;
//   typedef typename property_traits < CMap >::value_type C;
//   typedef color_traits<C> Color;

// public:
//   dfs_lasso_visitor(VMap pmap, V s, Iter& cur, Iter end, CMap c)
//     : m_p(pmap), m_s(s), m_cur(cur), m_end(end), m_c(c) {
//   }
//   template < typename Vertex, typename Graph >
//     void discover_vertex(Vertex u, const Graph & g) const
//   {
//     if (u != m_s)
//       ++m_cur;
//   }
//   template <class Edge, class Graph>
//   void examine_edge(Edge e, const Graph& g) {
//     T u, v;
//     u = source(e, g);
//     v = target(e, g);
//     if (g[e].label != *m_cur)
//       put (m_c, v, Color::black ());
//   }

//   template <class Edge, class Graph>
//   void tree_edge(Edge e, const Graph& g) {
//     T u, v;
//     // assert (g[e].label == *cur)
//     u = source(e, g);
//     v = target(e, g);
//     if (u == m_s)
//       {
// 	put(m_p, v, m_s);
//       }
//     else
//       {
// 	put(m_p, v, get (m_p, u));
//       }
//     }
//   template < typename Vertex, typename Graph >
//     void finish_vertex(Vertex u, const Graph & g) const
//   {
//     if (u != m_s)
//       --m_cur;
//   }
//   VMap m_p;
//   V m_s; // s is the state where the cycle starts (and ends).
//   Iter& m_cur;
//   Iter m_end;
//   CMap m_c;
// };

//========================================================================
// is_not_in_subset Predicate (the one from Boost doesn't compile)
template <class Set>
struct is_not_in_subset {
  is_not_in_subset(): m_set(0) { }
  is_not_in_subset(const Set& s) : m_set(&s) { }
  template <typename Edge>
  bool operator()(const Edge& e) const {
    // Return true if e is not in m_set
    return m_set == 0 || m_set->find (e) == m_set->end ();
  }
  const Set* m_set;
};
template <typename Set>
is_not_in_subset<Set> not_in (const Set& s)
{
  return is_not_in_subset<Set> (s);
}

template <class Set, class Graph>
void load_final_states (Set& s, const char* fname, const Graph& g)
{
  std::ifstream in (fname);
  while (in.good ())
    {
      unsigned i;
      in >> i;
      s.insert (vertex (i, g));
    }
}

// HACK: Exhaustive behavior on prop
// Requiere to know all actions of prop
bool is_ok (const std::string& prop, const std::string& lbl)
{
  if (prop == "true")
    return true;
  else
    {
      if (prop == "assert")
	return lbl == "ASSERTION_VIOLATION !9";
      else // prop == "~assert"
	return lbl != "ASSERTION_VIOLATION !9";
    }
}

template <class Automaton, class Set, class Iter>
bool
is_accepting_lasso_rec (const Automaton& aut, const Automaton& prop,
			const Set& finals,
			typename boost::graph_traits<Automaton>::vertex_descriptor u,
			typename boost::graph_traits<Automaton>::vertex_descriptor s_cycle,
			Iter first, const Iter last,
			bool b_cycle, bool b_accept)
{
  typedef typename boost::graph_traits<Automaton>::vertex_descriptor vertex_descriptor;
  typedef typename boost::graph_traits<Automaton>::out_edge_iterator out_edge_iterator;

  if (first != last)
    {
      out_edge_iterator ei, ei_end;
      boost::tie (ei, ei_end) = out_edges (u, prop);
      while (ei != ei_end)
	{
	  if (is_ok (prop[*ei].label, aut[*first].label))
	    {
	      // follow ei
	      vertex_descriptor v = target (*ei, prop);
	      if (!b_cycle)
		b_cycle = (s_cycle == target (*first, aut));
	      b_accept = (b_cycle && finals.find (v) != finals.end ());
	      
	      return is_accepting_lasso_rec (aut, prop, finals, v, s_cycle,
					     ++first, last, b_cycle, b_accept);
	    }
	}
      std::cerr << "NOTLASSO" << std::endl;
      return false;
    }
  else
    {
      // std::cerr << "LASSO" << std::endl;
      return b_accept;
    }
}

//========================================================================
template <class Path, class Automaton, class Set>
bool is_accepting_lasso (const Path& lasso, const Automaton& aut,
			 const Automaton& prop, const Set& finals)
{
  typedef typename boost::graph_traits<Automaton>::vertex_descriptor Vertex;
  // typedef typename boost::graph_traits<Automaton>::vertex_iterator Iter;
  // typedef std::pair<Vertex, std::pair<Iter, Iter> > VertexInfo;
  // typedef boost::default_color_type ColorValue;
  // typedef boost::color_traits<ColorValue> Color;
  // bool res = false;
  typename Path::const_iterator first = lasso.begin ();
  typename Path::const_iterator last = lasso.end ();
  while (first != last && aut[*first].label == "")
    ++first;
  assert (first != last);

  // Iter ei, ei_end;
  // std::vector<VertexInfo> stack;

  // std::vector<color_type> color_vec (num_vertices(prop));
  // std::vector<vertex_descriptor> p (num_vertices(prop), *vertices(prop).first);
  // benefice boost::property_map methods.
  // color_type* color = &color_vec[0];
  // vertex_descriptor* pred = &p[0];

  // boost::depth_first_search
  //   (prop,
  //    visitor
  //    (boost::make_dfs_visitor
  //     (std::make_pair
  //      (update_predecessor(&p[0], boost::on_tree_edge()),
  // 	skip_edges(first, last, color_vec, boost::on_examine_edge ())
  // 	)))
  //    .colormap
  //    (&color_vec[0], get (boost::vertex_index, g)));

  // // personalize BGL's DFS to skip some edges (may be done with filtered_graph)
  // typename Path::const_iterator cur = first;
  // Vertex u = *vertices(prop).first;
  // put (color, u, Color::gray ());
  // // vis.discover_vertex(u, g);
  // Vertex ul = source(*cur, aut);
  // if (ul == s_cycle)
  //   b_cycle = true;
  // if (b_cycle && finals.find (u) != finals.end ())
  //   b_accept = true;
  // // End vis.discover_vertex(u, g);
  // boost::tie (ei, ei_end) = out_edges (u, prop);
  // stack.push_back(std::make_pair(u, std::make_pair(ei, ei_end)));
  // while (!stack.empty()) {
  //   VertexInfo& back = stack.back();
  //   u = back.first;
  //   tie(ei, ei_end) = back.second;
  //   stack.pop_back();
  //   // look for next out_edges to explore
  //   while (ei != ei_end && prop[*ei].label != aut[*cur].label)
  //     ++ei;
  //   while (ei != ei_end) {
  //     Vertex v = target(*ei, prop);
  //     // vis.examine_edge(*ei, g);
  //     ColorValue v_color = get(color, v);
  //     if (v_color == Color::white()) {
  // 	// vis.tree_edge(*ei, g);
  // 	++cur;
  // 	stack.push_back(std::make_pair(u, std::make_pair(++ei, ei_end)));
  // 	u = v;
  // 	put(color, u, Color::gray());
  // 	// vis.discover_vertex(u, g);
  // 	Vertex ul = source(*cur, aut);
  // 	if (ul == s_cycle)
  // 	  b_cycle = true;
  // 	if (b_cycle && finals.find (u) != finals.end ())
  // 	  b_accept = true;
  // 	// End vis.discover_vertex(u, g);
  // 	tie(ei, ei_end) = out_edges(u, g);
  // 	if (res) {
  // 	  ei = ei_end;
  // 	}
  //     } else if (v_color == Color::gray()) {
  //     	// vis.back_edge(*ei, g);
  //     	++ei;
  //     } else {
  // 	// vis.forward_or_cross_edge(*ei, g);
  // 	++ei;
  //     }
  //     while (ei != ei_end && prop[*ei].label != aut[*cur].label)
  // 	++ei;
  //   }
  //   put(color, u, Color::black());
  //   // vis.finish_vertex(u, g);
  //   if (cur != first)
  //     --cur;
  // }
  // return res;
  Vertex u =  *vertices(prop).first;
  Vertex s_cycle = target (*lasso.rbegin(), aut); // state where the cycle starts.
  bool b_cycle = (s_cycle == source (*first, aut));
  bool b_accept = (b_cycle && finals.find (u) != finals.end ());
  return is_accepting_lasso_rec (aut, prop, finals, u, s_cycle,
				 first, last, b_cycle, b_accept);
}

//========================================================================
// This program measures time and memory needed to draw uniformly at
// random m paths of length n from a graph described in GRAPHVIZ
// format (.dot)
int main (int argc, char *argv[])
{
  typedef urandom<_VTYPE>::type	ralgo_type;
  typedef std::vector<_VTYPE>	vec_type;
  typedef std::set < edge_descriptor > edges_type;
  typedef boost::filtered_graph<automaton, is_not_in_subset<edges_type> >
    filter_graph;
  typedef dichopile<filter_graph,vec_type,ralgo_type>	draw_type;
  typedef boost::graph_traits < automaton >::vertices_size_type size_type;
  typedef boost::graph_traits<automaton>::vertex_descriptor	vertex_descriptor;
  typedef std::set<vertex_descriptor>	vertices_type;

  if (argc != 5)
    {
      std::cerr << "Usage: " << argv[0] << " graph.dot prop.dot prop.final m" << std::endl;
      exit (1);
    }

  // Load graph
  automaton aut;
  io::dot::load (aut, argv[1]);
  automaton prop;
  io::dot::load (prop, argv[2]);

  // Load final states of prop
  vertices_type prop_finals;
  load_final_states (prop_finals, argv[3], prop);

  // unsigned n = atoi (argv[3]);
  // std::ofstream out (argv[3]);
  
  boost::timer timer;
  timer.restart ();


  vec_type L1 (num_vertices(aut), 0);
  std::vector < size_type > d (num_vertices(aut), 0);
  std::vector < vertex_descriptor > pred (num_vertices(aut),
					  *vertices(aut).first);
  edges_type backset;
  edges_type fictive_edges;
  std::pair<edge_descriptor, bool> p = edge (vertex(0,aut), vertex(0,aut), aut);
  if (p.second)
    fictive_edges.insert (p.first);
  // Run DFS and record backedge set and DFS depth
  boost::depth_first_search
    (filter_graph (aut, not_in (fictive_edges)),
     visitor
     (boost::make_dfs_visitor
      (std::make_pair
       (boost::record_distances(&d[0], boost::on_tree_edge()),
	std::make_pair
	(boost::record_predecessors(&pred[0], boost::on_tree_edge()),
	 record_backedges(&L1[0], backset, boost::on_back_edge ())
	 )))));
  std::size_t n = *std::max_element(d.begin (), d.end ());
  std::cerr << "n = " << n << std::endl;
  std::cerr << "number of back edges = " << backset.size () << std::endl;

  // Analyse one lasso per backedge (should be enough for some properties)
  size_t n_dfs = 1;
  for (edges_type::const_iterator bei = backset.begin ();
       bei != backset.end (); ++bei)
    {
      // build lasso from the backedge to the top of the DFS
      path_type lasso (n+1);
      path_type::reverse_iterator li = lasso.rbegin ();
      vertex_descriptor v = source (*bei, aut);
      *li++ = *bei;
      for (; li != lasso.rend (); ++li)
	{
	  vertex_descriptor u = pred[v];
	  edge_descriptor e;
	  bool b;
	  boost::tie (e, b) = edge (u, v, aut);
	  assert (b);
	  *li = e;
	  v = u;
	}
	  // Print the lasso
	  trace_type trace = rukia::trace (lasso, aut);
	  std::cout << boost::algorithm::join (trace, " ")
		    << std::endl;
      // Check this lasso
      if (is_accepting_lasso (lasso, aut, prop, prop_finals))
	{
	  std::cerr << "[DFS] Accepted...STOP" << std::endl;
	  // Print the lasso
	  // trace_type trace = rukia::trace (lasso, aut);
	  // std::cout << boost::algorithm::join (trace, " ")
	  // 	    << std::endl;
	  return 42;
	}
      else
	{
	  std::cerr << "[DFS" << n_dfs++
		    << "] Non accepted... CONTINUE" << std::endl;
	}
      
    }

  // std::cerr << "0 || ";
  // for (vec_type::const_iterator it = L1.begin (); it != L1.end (); ++it)
  //   std::cerr << *it << " | ";
  // std::cerr << std::endl;

  // Remove back edges from the original graph
  filter_graph g (aut, not_in (backset));


  std::cerr << "Pre: " << timer.elapsed () << " s" << std::endl;

  // Draw m paths and print it
  bool premier_appel = true;
  unsigned m = atoi (argv[4]);
  ralgo_type ralgo;
  ralgo.seed ();
  u_i_random<> rng; // random generator for choosing the end of a lasso
  rng.seed ();
  timer.restart ();
  draw_type draw (g, L1, ralgo);
  for (unsigned i = 0; i < m; ++i)
    {
      path_type lasso = draw (n);
      vertex_descriptor u = target (*lasso.rbegin (), aut);
      // Add a backedge that start from u to terminate the lasso
      edges_type edges4u;
      for (edges_type::const_iterator it = backset.begin ();
	   it != backset.end (); ++it)
	if (source (*it, aut) == u)
	  edges4u.insert (*it);
      assert (! edges4u.empty ());
      edges_type::const_iterator it = edges4u.begin ();
      if (edges4u.size () > 1)
	{
	  size_t ui = rng (edges4u.size ());
	  for (size_t j = 0; j < ui; ++j, ++it)
	    ;
	}
      lasso.push_back (*it);


      if (is_accepting_lasso (lasso, aut, prop, prop_finals))
	{
	  std::cerr << "Accepted...STOP" << std::endl;
	  // Print the lasso
	  trace_type trace = rukia::trace (lasso, aut);
	  std::cout << boost::algorithm::join (trace, " ")
		    << std::endl;
	  return 42;
	}
      else
	{
	  std::cerr << "Non accepted... CONTINUE" << std::endl;
	}

      if (premier_appel)
	{
	  // std::cerr << "MaxStackSize: " << draw.max_stack_size () << std::endl;
	  // std::cerr << "NbCalculation: " << draw.nb_calculation () << std::endl;
	  premier_appel = false;
	}
    }

  std::cerr << "Draw: " << timer.elapsed () << " s" << std::endl;
  std::cerr << "Mem: " << draw.mem_size () << " MiB" << std::endl;
  // out.close();
}

#endif // ! RUKIA_LASSOS_CC
