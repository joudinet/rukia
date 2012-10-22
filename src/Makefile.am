# Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2011
#  
# This file is part of Rukia.
#  
# Rukia is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#  
# Rukia is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#  
# You should have received a copy of the GNU Lesser General Public
# License along with Rukia.  If not, see <http://www.gnu.org/licenses/>.
#  
AM_CPPFLAGS = -I$(srcdir)/../include $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_REGEX_LDFLAGS)$(BOOST_GRAPH_LDFLAGS)
LDADD = $(BOOST_REGEX_LIBS) $(BOOST_GRAPH_LIBS)

bin_PROGRAMS =					\
bench_mpz					\
bench_dbl					\
bench_rw					\
bench_boltz					\
draw_exact_ctable				\
draw_exact_boltz				\
draw_lassos					\
bench_ctable_float				\
bench_dichopile					\
bench_divide_and_conquer

draw_exact_ctable_SOURCES = draw_with_ctable.cc
draw_exact_ctable_CXXFLAGS= -O3

bench_mpz_SOURCES = bench_path_cover.cc
bench_mpz_CXXFLAGS = -O3

bench_rw_SOURCES = bench_path_cover_rw.cc
bench_rw_CXXFLAGS = -O3

bench_dbl_SOURCES = bench_path_cover_dbl.cc
bench_dbl_CXXFLAGS = -O3

bench_boltz_SOURCES = bench_path_cover_boltz.cc
bench_boltz_CXXFLAGS = -O3

draw_exact_boltz_SOURCES = draw_exact_boltz.cc
draw_exact_boltz_CXXFLAGS = -g

draw_lassos_SOURCES = draw_lassos.cc
draw_lassos_CXXFLAGS= -O3

bench_ctable_float_SOURCES = bench_ctable_float.cc
bench_ctable_float_CXXFLAGS = -O3

bench_dichopile_SOURCES = bench_dichopile.cc
bench_dichopile_CXXFLAGS = -O3

bench_divide_and_conquer_SOURCES = bench_divide_and_conquer.cc
bench_divide_and_conquer_CXXFLAGS = -O3
