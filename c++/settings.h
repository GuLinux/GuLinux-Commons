#ifndef GULINUX_SETTINGS_READER_H
#define GULINUX_SETTINGS_READER_H
#include <memory>
#include <boost/optional.hpp>
#include <list>

namespace GuLinux {
  namespace Settings {
    
    
    template<typename T> class value_with_default {
    public:
      value_with_default(const boost::optional<T> &value, const T &default_value) : _value(value), _default_value(default_value) {}
      bool is_set() const { return (_value ? true: false); }
      operator T() const { if(_value) return *_value; return _default_value; };
      T value() const { return *this; }
    private:
      boost::optional<T> _value;
      T _default_value;
    };
    
    class Reader {
    public:
      virtual boost::optional<std::string> value(const std::string &key) const = 0;
      typedef std::shared_ptr<Reader> ptr;
    };
    
    class EnvironmentReader : public Settings::Reader {
    public:
      EnvironmentReader(const std::string &variables_prefix);
      virtual boost::optional<std::string> value(const std::string& key) const;
    private:
      std::string variables_prefix;
    };
    
    class CompositeReader : public GuLinux::Settings::Reader {
    public:
      CompositeReader(const std::initializer_list<Reader::ptr> &readers);
      virtual boost::optional<std::string> value(const std::string& key) const;
    private:
      std::list<Reader::ptr> readers;
    };
  }
}

#endif