# Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2012
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

AC_PREREQ(2.61)
AC_INIT([rukia], [0.43], [oudinet@lri.fr])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.10 -Wall -Werror tar-ustar no-define foreign dist-bzip2 nostdinc])

# If the user doesn't provide any CXXFLAGS, prevent Autoconf from
# its own default ones (e.g., `-g -O2' for g++).
if test ! ${CXXFLAGS+set}; then
   CXXFLAGS="-W -Wall"
fi

# Checks for programs.
AC_PROG_CXX

# Attempt to use strict language conformance checking.
AC_CXX_FLAGS

# Use libtool
AC_PROG_LIBTOOL

# Checks for libraries.
# test for GMP and c++ wrappers
AC_CHECK_LIB([gmp], [__gmpz_init], , [AC_MSG_ERROR([This package needs libgmp library])])
AC_CHECK_LIB([gmpxx], [__gmpz_init], , [AC_MSG_ERROR([This package needs libgmpxx library])])

# test for BOOST graph library using Tsuna's boost.m4 macro file
BOOST_REQUIRE([1.47.0])
BOOST_REGEX
BOOST_GRAPH

# Checks for header files.
AC_LANG_PUSH(C++)
AC_CHECK_HEADERS([gmpxx.h], , [AC_MSG_ERROR([This package needs gmpxx.h header])])
AC_LANG_POP

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.


AC_CONFIG_FILES([
	Makefile
	include/Makefile
	src/Makefile
])

AC_OUTPUT
