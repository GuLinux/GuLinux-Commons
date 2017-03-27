/*
 * Copyright (C) 2016  Marco Gulino <marco@gulinux.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "demangle.h"
#include <stdlib.h>
#include <cxxabi.h>

using namespace GuLinux;
using namespace std;

GuLinux::Demangle::Demangle(const string &mangled) : demangled(abi::__cxa_demangle(mangled.c_str(), 0, 0, &_status))
{
}

GuLinux::Demangle::~Demangle()
{
  if(demangled && ok())
    free(demangled);
}


const char * GuLinux::Demangle::c_str() const
{
  return demangled;
}

string GuLinux::Demangle::get() const
{
  return {demangled};
}

GuLinux::Demangle::operator std::string() const
{
  return get();
}

int GuLinux::Demangle::status() const
{
  return _status;
}

std::string GuLinux::Demangle::error_message() const
{
  switch(status()) {
    case 0:
      return "Ok.";
    case -1:
      return "A memory allocation failure occurred.";
    case -2:
      return "a mangled_name is not a valid name under the C++ ABI mangling rules.";
    case -3:
      return "One of the arguments is invalid.";
  }
  return "Unknown error code";
}

bool GuLinux::Demangle::ok() const
{
  return status() == 0;
}




