#include "whtmltemplateslocalizedstrings.h"
#include <boost/filesystem.hpp>
#include <Wt/WApplication>
#include <iostream>
#include <fstream>

using namespace Wt;
using namespace boost;
using namespace std;
using namespace WtCommons;
using namespace WtCommonsPrivate;

namespace fs = boost::filesystem;

namespace WtCommons { namespace WtCommonsPrivate {
class WHTMLTemplatesLocalizedStringsPrivate {
public:
  void processHtmlTemplate(boost::filesystem::path path);
  std::map<string,string> translationMap;
};
}}

WHTMLTemplatesLocalizedStrings::WHTMLTemplatesLocalizedStrings(const string& resourcesDir, WObject* parent): WObject(parent), d(new WHTMLTemplatesLocalizedStringsPrivate)
{
  fs::path p(resourcesDir);
  for(fs::directory_iterator it(p); it != fs::directory_iterator(); it++) {
      if(fs::is_regular_file(it->path()) && it->path().extension().string() == ".html")
        d->processHtmlTemplate(it->path());
  }
}

void WHTMLTemplatesLocalizedStringsPrivate::processHtmlTemplate(filesystem::path path)
{
  string key = path.filename().replace_extension().string();
  ifstream file(path.string());
  stringstream s;
  s << file.rdbuf();
  translationMap[key] = s.str();
}


bool WHTMLTemplatesLocalizedStrings::resolveKey(const std::string& key, std::string& result)
{
  if(d->translationMap.count(key)>0) {
    result = d->translationMap[key];
    return true;
  }
  return false;
}

WHTMLTemplatesLocalizedStrings::~WHTMLTemplatesLocalizedStrings()
{
  delete d;
}

