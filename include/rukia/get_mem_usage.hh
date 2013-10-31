// Copyright (C) Univ. Paris-SUD, Johan Oudinet <oudinet@lri.fr> - 2009, 2013
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
#ifndef RUKIA_GET_MEM_USAGE_HH
# define RUKIA_GET_MEM_USAGE_HH
# include <iostream>
# include <fstream>
# include <string>
# include <boost/lexical_cast.hpp>
# include <unistd.h> // Required to find getpid on MinGW

// Return used memory in MiB
unsigned long get_mem_usage ()
{
  unsigned long memtot = 0;
  unsigned long memfree = 0;
  unsigned long ul;
  
  std::ifstream ifs ("/proc/meminfo");

  while (ifs.good ())
    {
      std::string line;
      ifs >> line;
      if (line == "MemTotal:" || line == "SwapTotal:")
	{
	  ifs >> ul;
	  ifs >> line;
	  if (line == "MB")
	    ul *= 1024;
	  memtot += ul;
	}
      else if ((line == "MemFree:") ||
	       (line == "Buffers:") ||
	       (line == "Cached:")  ||
	       (line == "SwapFree:"))
	    {
	      ifs >> ul;
	      ifs >> line;
	      if (line == "MB")
		ul *= 1024;
	      memfree += ul;
	    }
    }

  return (memtot - memfree) / 1024;
}

// Return used memory by pid (in MiB)
// This function is more precise than using get_mem_usage before and
// after computations.
unsigned long proc_mem_usage (int pid = getpid())
{
  unsigned long mem = 0;
  std::string filename = "/proc/" +
			  boost::lexical_cast<std::string>(pid) +
			  "/status";
  std::ifstream ifs (filename.c_str ());

  while (ifs.good ())
    {
      std::string line;
      ifs >> line;
      if (line == "VmSize:")
	{
	  ifs >> mem;
	  ifs >> line;
	  if (line == "MB")
	    mem *= 1024;
	}
    }
  return mem / 1024;
}

#endif // ! RUKIA_GET_MEM_USAGE_HH
