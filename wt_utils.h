#ifndef WT_COMMONS_UTILS_H
#define WT_COMMONS_UTILS_H
#include <Wt/WString>
#include <boost/format.hpp>

template<typename T>
inline Wt::WString operator|(Wt::WString s, const T &t) {
  return s.arg(t);
}

namespace WtCommons {
template<typename T>
class format {
public:
  format(const std::string &formatString) : _format{formatString} {}
  template<typename F> format &operator%(const F &f) { _format % f; return *this; }
  template<typename F> format &operator%(F &f) { _format % f; return *this; }
  operator T() const { return {_format.str()}; }
private:
  boost::format _format;
};
}

#ifdef IN_IDE_PARSER
#define _ws +WString()
#define _wtr +WString()
#define _s +std::string()
#else
inline Wt::WString operator ""_ws(const char *s, std::size_t) {
  return {s};
}
inline Wt::WString operator ""_wtr(const char *s, std::size_t) {
  return Wt::WString::tr(s);
}

inline std::string operator ""_s(const char *s, std::size_t l) {
  return {s, l};
}

template<typename T>
inline WtCommons::format<std::string> operator%(const std::string &s, const T &t) {
  return WtCommons::format<std::string>{s} % t;
}

template<typename T>
inline WtCommons::format<Wt::WString> operator%(const Wt::WString &s, const T &t) {
  return WtCommons::format<Wt::WString>{s.toUTF8()} % t;
}
#endif
#endif