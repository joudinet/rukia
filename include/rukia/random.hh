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
#ifndef RUKIA_RANDOM_HH
# define RUKIA_RANDOM_HH
# include <ctime>
# include <gmpxx.h>
# include <boost/random.hpp>

// This file provide wrappers for random object useful for drawing classes.


#ifdef BOOST_NO_STDC_NAMESPACE
namespace std {
  using ::time;
}
#endif

namespace rukia {
  // Random wrapper for mpz_class numbers
  class u_mpz_random
  {
  public:
    typedef mpz_class	value_type;

    u_mpz_random () : ralgo (gmp_randinit_mt) {}

    void seed (unsigned long int i = std::time(0)) { ralgo.seed (i); }

    value_type operator()(const value_type& max)
    {
      return ralgo.get_z_range (max);
    }

    private:
    gmp_randclass ralgo;
  };

  // Random wrapper for mpf_class numbers
  class u_mpf_random
  {
  public:
    typedef mpf_class	value_type;

    u_mpf_random () : ralgo (gmp_randinit_mt) {}

    void seed (unsigned long int i = std::time(0)) { ralgo.seed (i); }

    value_type operator()(const value_type& max)
    {
      return ralgo.get_f (max.get_prec ()) * max;
    }

    private:
    gmp_randclass ralgo;
  };

  // Random wrapper for Integer numbers
  template <class UniformRandomGenerator = boost::mt19937,
	    class IntType = unsigned>
  class u_i_random
  {
  public:
    typedef UniformRandomGenerator base_type;
    typedef IntType	value_type;

    u_i_random () : rng_(42u), ralgo_(rng_) {};

    void seed (unsigned int i = std::time(0)) { rng_.seed (i); }

    value_type operator()(const value_type& max)
    {
      return ralgo_ (max);
    }

  private:
    base_type	rng_;
    boost::random_number_generator<base_type, value_type> ralgo_;
  };

  // Random wrapper for Real numbers
  template <class UniformRandomGenerator = boost::lagged_fibonacci607,
	    class RealType = double>
  class u_r_random
  {
  public:
    typedef UniformRandomGenerator base_type;
    typedef RealType	value_type;

    u_r_random () : rng_(){};
    void seed (unsigned int i = std::time(0)) { rng_.seed (i); }

    value_type operator()(const value_type& max)
    {
      return boost::uniform_real<RealType> (0, max) (rng_);
    }

  private:
    base_type	rng_;
  };

  // Uniform random traits
  template<typename T>
  struct urandom { };

  template<>
  struct urandom<mpz_class> { typedef u_mpz_random type; };

  template<>
  struct urandom<mpf_class> { typedef u_mpf_random type; };



} // namespace rukia

#endif // ! RUKIA_RANDOM_HH
