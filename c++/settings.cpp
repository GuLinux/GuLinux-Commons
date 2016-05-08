/*
 * Copyright (C) 2016 Marco Gulino <marco@gulinux.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "settings.h"

GuLinux::Settings::EnvironmentReader::EnvironmentReader(const std::string& variables_prefix) : variables_prefix(variables_prefix)
{
}


#include <cstdlib>
#include <boost/algorithm/string.hpp>
boost::optional< std::string > GuLinux::Settings::EnvironmentReader::value(const std::string& key) const
{
  std::string key_tr = variables_prefix + boost::replace_all_copy(key, std::string{"-"}, std::string{"_"});
  boost::to_upper(key_tr);
  if(getenv(key_tr.c_str())) {
    return std::string{getenv(key_tr.c_str())};
  }
  return {};
}

GuLinux::Settings::CompositeReader::CompositeReader(const std::initializer_list< GuLinux::Settings::Reader::ptr >& readers) : readers{readers}
{
}

boost::optional< std::string > GuLinux::Settings::CompositeReader::value(const std::string& key) const
{
  for(auto reader: readers) {
    auto value = reader->value(key);
    if(value)
      return value;
  }
  return {};
}
