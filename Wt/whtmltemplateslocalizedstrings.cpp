/***********************************************************************
Copyright (c) 2013 "Marco Gulino <marco.gulino@gmail.com>"

This file is part of Pandorica: https://github.com/GuLinux/Pandorica

Pandorica is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details (included the COPYING file).

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/


#include "whtmltemplateslocalizedstrings.h"
#include <boost/filesystem.hpp>
#include <Wt/WApplication>
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>

using namespace Wt;
using namespace boost;
using namespace std;
using namespace WtCommons;

namespace fs = boost::filesystem;

namespace WtCommons { 
class WHTMLTemplatesLocalizedStrings::Private {
public:
  void processHtmlTemplate(boost::filesystem::path path, const std::string &locale = std::string());
  std::map<string,string> translationMap;
};
}

WHTMLTemplatesLocalizedStrings::WHTMLTemplatesLocalizedStrings(const string& resourcesDir, WObject* parent): WObject(parent), dptr()
{
  fs::path p(resourcesDir);
  for(fs::directory_iterator it(p); it != fs::directory_iterator(); it++) {
      if(fs::is_regular_file(it->path()) && it->path().extension().string() == ".html")
        d->processHtmlTemplate(it->path());
      else if(fs::is_directory(it->path())) {
        for(fs::directory_iterator sit(*it); sit != fs::directory_iterator(); sit++) {
          if(fs::is_regular_file(sit->path()) && sit->path().extension().string() == ".html")
            d->processHtmlTemplate(sit->path(), it->path().filename().string() );
        }
      }
  }
}

void WHTMLTemplatesLocalizedStrings::Private::processHtmlTemplate(filesystem::path path, const string &locale)
{
  string key = (locale.empty() ? string() : locale + "_") + path.filename().replace_extension().string();
  ifstream file(path.string());
  stringstream s;
  s << file.rdbuf();
  translationMap[key] = s.str();
}


bool WHTMLTemplatesLocalizedStrings::resolveKey(const std::string& key, std::string& result)
{
  string keyWithLocale = boost::regex_replace(wApp->locale().name(), boost::regex("-.*"), "" ) + "_" + key;
  if(d->translationMap.count(keyWithLocale)>0) {
    result = d->translationMap[keyWithLocale];
    return true;
  }
  if(d->translationMap.count(key)>0) {
    result = d->translationMap[key];
    return true;
  }
  return false;
}

WHTMLTemplatesLocalizedStrings::~WHTMLTemplatesLocalizedStrings()
{
}

