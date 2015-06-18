#ifndef WT_COMMONS_UTILS_H
#define WT_COMMONS_UTILS_H
#include <Wt/WString>
#include <boost/format.hpp>
inline Wt::WString operator ""_ws(const char *s, std::size_t) {
  return {s};
}

inline Wt::WString operator ""_wtr(const char *s, std::size_t) {
  return Wt::WString::tr(s);
}

template<typename T>
inline Wt::WString operator%(Wt::WString s, const T &t) {
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
inline WtCommons::format<std::string> operator ""_fmt(const char *s, std::size_t) {
  return {{s}};
}
inline WtCommons::format<Wt::WString> operator ""_wfmt(const char *s, std::size_t) {
  return {{s}};
}
#endif