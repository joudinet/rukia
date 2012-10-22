// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2008, 2009, 2010, 2011
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
#ifndef RUKIA_IO_DOT_LOAD_HH
# define RUKIA_IO_DOT_LOAD_HH
# include <string>
# include <cctype>
# include <fstream>
# include <boost/shared_ptr.hpp>
# include <boost/graph/graphviz.hpp>
# include <boost/algorithm/string.hpp>
# include <boost/iterator/iterator_facade.hpp>
# include <rukia/automaton.hh>
namespace rukia
{

  namespace io
  {

    namespace dot
    {

      namespace detail
      {
	template <typename T, typename V = std::pair<T, T> >
	struct edge_parser
	{
	  typedef V	result_type;

	  edge_parser() {}
	  
	  static bool read(std::istream& file, V& value)
	  {
	    if (file.good () && file.peek () != '}')
	      {
		T src, dst;
		char c;
		std::string str;
		do { 
		  file >> src;
		  do { c = file.get (); } while (isspace (c));
		} while (c == ';');
		assert (c == '-');
		c = file.get (); assert (c == '>');
		file >> dst;
		getline (file, str);
		value = std::make_pair (src, dst);
		return true;
	      }
	    else
	      return false;
	  }
	};

	struct label_parser
	{
	  typedef std::string	result_type;

	  label_parser() {}

	  static bool read(std::istream& file, result_type& value)
	  {
	    if (file.good () && file.peek () != '}')
	      {
		std::string str;
		getline (file, str);
		std::vector<std::string> strs;
		boost::split(strs, str, boost::is_any_of("\""));

		assert (strs.size () == 3);
		value = strs[1];
		return true;
	      }
	    else
	      return false;
	  }
	};

	template <class Parser>
	class dot_graph_iterator
	  : public boost::iterator_facade<
	  dot_graph_iterator<Parser>
	  , typename Parser::result_type
	  , boost::single_pass_traversal_tag
	  , typename Parser::result_type const&
	  >
	{
	  typedef boost::iterator_facade<
	    dot_graph_iterator<Parser>
	    , typename Parser::result_type
	    , boost::single_pass_traversal_tag
	    , typename Parser::result_type const&
	    >	super_t;
	public:
	  dot_graph_iterator() : pfile_(), ok_(false) {}
	  dot_graph_iterator(const std::string& fname)
	    : pfile_(new std::ifstream(fname.c_str())), ok_(true)
	  {
	    if (! *pfile_)
	      {
		std::cerr << "error: file '" << fname
			  << "' not found!";
		ok_ = false;
	      }
	    else
	      {
		// Parse first line
		std::string str;
		getline (*pfile_, str);
		std::vector<std::string> strs;
		boost::split(strs, str, boost::is_any_of("\t "));
		
		assert (strs.size () == 3);
		assert (strs[0] == "digraph");
		assert (strs[2] == "{");
		ok_ = Parser::read(*pfile_, value_);
	      }
	  }

	  dot_graph_iterator(const dot_graph_iterator& it)
	    : pfile_(it.pfile_), ok_(it.ok_), value_(it.value_) {}

	  const typename Parser::result_type&
	  dereference() const
	  {
	    assert (ok_);
	    return value_;
	  }

	  void increment()
	  {
	    assert (ok_);
	    ok_ = Parser::read(*pfile_, value_);
	  }

	  bool equal(const dot_graph_iterator& it) const
	  {
	    return (ok_ == it.ok_) && (!ok_ || value_ == it.value_);
	  }

	private:
	  boost::shared_ptr<std::ifstream>	pfile_;
	  bool					ok_;
	  typename Parser::result_type		value_;

	};

	// Parse the edge list inside a GraphViz file to find the max
	// vertex index.
	size_t max_vertex_index(const std::string& fname)
	{
	  std::ifstream ifs (fname.c_str());
	  if (ifs.fail ())
	    {
	      std::cerr << "error: file '" << fname
			<< "' not found!";
	      return 0;
	    }
	  // Parse first line
	  std::string str;
	  getline (ifs, str);
	  std::vector<std::string> strs;
	  boost::split(strs, str, boost::is_any_of("\t "));
	
	  assert (strs.size () == 3);
	  assert (strs[0] == "digraph");
	  assert (strs[2] == "{");
    
	  size_t max = 0;
	  // Read each edge and check if one vertex index is greater than the
	  // current max value.
	  while (ifs.good () && ifs.peek () != '}')
	    {
	      size_t src, dst;
	      char c;
	      std::string str;
	      do { 
		ifs >> src;
		do { c = ifs.get (); } while (isspace (c));
	      } while (c == ';');
	      assert (c == '-');
	      c = ifs.get (); assert (c == '>');
	      ifs >> dst;
	      getline (ifs, str);
	      if (src > max)
		max = src;
	      if (dst > max)
		max = dst;
	    }
	  return max;
	}

      } // end namespace detail

      void load (old_automaton& a, const std::string& filename)
      {
	boost::dynamic_properties dp;
	dp.property ("id", boost::get (&::rukia::detail::state::id, a));
	dp.property ("label",
		     boost::get (&::rukia::detail::transition::label, a));
	std::ifstream file (filename.c_str());
	if (! file)
	  {
	    std::cerr << "error: file '" << filename
		      << "' not found!";
	    exit (1);
	  }
	read_graphviz (file, a, dp, "id");
      }

      // use a home-made read graphviz format for csr graph
      template<class Aut>
      void load (Aut& aut, const std::string& filename)
      {
	typedef typename boost::graph_traits<Aut>::vertex_descriptor vertex_descriptor;
#ifdef BOOST_GRAPH_USE_OLD_CSR_INTERFACE
	std::ifstream file (filename.c_str());
	if (! file)
	  {
	    std::cerr << "error: file '" << filename
		      << "' not found!";
	    exit (1);
	  }
	
	std::string str;
	char c;
	getline (file, str);
	std::vector<std::string> strs;
	boost::split(strs, str, boost::is_any_of("\t "));

	assert (strs.size () == 3);
	assert (strs[0] == "digraph");
	assert (strs[2] == "{");
	
// 	add_vertices (n_vertices, aut);
	while (file.good () && file.peek () != '}')
	  {
	    vertex_descriptor src, dst;
	    std::string label;
	    do { 
	      file >> src;
	      do { c = file.get (); } while (isspace (c));
	    } while (c == ';');
	    assert (c == '-');
	    c = file.get (); assert (c == '>');
	    file >> dst;
	    getline (file, str, '"');
	    getline (file, label, '"');
	    getline (file, str);
	    if (num_vertices (aut) <= src)
	      {
		add_vertices (src - num_vertices (aut) + 1, aut);
	      }
	    if (num_vertices (aut) <= dst)
	      {
		add_vertices (dst - num_vertices (aut) + 1, aut);
	      }
	    add_edge (src, dst, label, aut);
	  }
#else // BOOST_GRAPH_USE_NEW_CSR_INTERFACE
	typedef detail::edge_parser<vertex_descriptor> EdgeParser;
	typedef detail::label_parser LabelParser;
	typedef detail::dot_graph_iterator<EdgeParser>	edges_iterator;
	typedef detail::dot_graph_iterator<LabelParser>	labels_iterator;

	// Create every vertex descriptor before calling add_edges.
	aut.assign (Aut (detail::max_vertex_index (filename) + 1));
	add_edges (edges_iterator (filename.c_str()),
		   edges_iterator (),
		   labels_iterator (filename.c_str()),
		   labels_iterator (),
		   aut);
#endif // BOOST_GRAPH_USE_OLD_CSR_INTERFACE
      }

    } // end namespace rukia::io::dot

  } // end namespace rukia::io

} // end namespace rukia

#endif // ! RUKIA_IO_DOT_LOAD_HH
