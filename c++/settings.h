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