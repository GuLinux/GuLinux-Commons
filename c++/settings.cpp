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
