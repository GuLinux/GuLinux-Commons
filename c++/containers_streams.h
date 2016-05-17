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

#ifndef GULINUX_COMMONS_CONTAINERS_STREAMS
#define GULINUX_COMMONS_CONTAINERS_STREAMS
#include <utility>
#include <algorithm>
#include <numeric>
#include <functional>
#include <list>
#include <vector>
#include <iostream>
#include "optional.h"

namespace GuLinux {
  
  
template<typename C> struct container_accumulate {
  typedef typename C::value_type value_type;
  C &operator()(C &dest, const C &source) const {
    std::copy(std::begin(source), std::end(source), std::inserter(dest, std::end(dest) ) );
    return dest;
  }
};

template<typename V> struct identity {
  V operator()(const V &v) const {
    return v;
  }
};


  
template<typename C> class cstream {
public:

  typedef typename C::value_type value_type;
  typedef decltype(C{}.begin()) it_type;

  cstream(C &c) : _container_ref{c} {}
  cstream(C &&c) : __moved{c}, _container_ref{__moved} {}
  C &get_ref() { return _container_ref; }
  C &&get_move() { return std::move(_container_ref); }
  C get() const { return _container_ref; }
  operator C&&() { return get_move(); }
  
  // Operations
  template<typename BinaryFunction = std::less<value_type>> cstream<C> &sorted(const BinaryFunction &op = {}) {
    std::sort(std::begin(_container_ref), std::end(_container_ref), op);
    return *this;
  }
  
  template<typename T, typename UnaryFunction> cstream<T> transform(UnaryFunction transform_f) const {
    T dest;
    std::transform(std::begin(_container_ref), std::end(_container_ref), std::inserter(dest, std::end(dest)), transform_f);
    return {std::move(dest)};
  }
  
  template<typename UnaryFunction> cstream<C> & remove(UnaryFunction remove_f) {
    _container_ref.erase(std::remove_if(std::begin(_container_ref), std::end(_container_ref), remove_f), std::end(_container_ref));
    return *this;
  }
  
  template<typename UnaryFunction> cstream<C> & remove_ms(UnaryFunction remove_f) {
    std::list<it_type> to_remove;
    for(auto it = _container_ref.begin(); it != _container_ref.end(); ++it) {
      if(remove_f(*it))
	to_remove.push_back(it);
    }
    for(auto it: to_remove)
      _container_ref.erase(it);
    return *this;
  }
  
  
  template<typename ValueType> cstream<C> &erase(ValueType v) {
    _container_ref.erase(v);
    return *this;
  }
  
    
  template<typename UnaryFunction> cstream<C> &filter(UnaryFunction filter) {
    return remove([&](const value_type &v) { return ! filter(v); });
  }
  
  template<typename UnaryFunction> cstream<C> cp_filter(UnaryFunction filter) {
    C c;
    std::copy_if(std::begin(_container_ref), std::end(_container_ref), std::inserter(c, std::end(c)), filter);
    return cstream<C>(std::move(c));
  }
  
  template<typename UnaryFunction> cstream<C> &filter_ms(UnaryFunction filter) {
    return remove_ms([&](const value_type &v) { return ! filter(v); });
  }
  
  
  template<typename UnaryFunction = std::plus<value_type>> value_type accumulate(value_type initial = {}, UnaryFunction op = {}) const {
    return std::accumulate(std::begin(_container_ref), std::end(_container_ref), initial, op);
  }
  
  template<typename UnaryFunction> cstream<C> &for_each(UnaryFunction f) {
    std::for_each(std::begin(_container_ref), std::end(_container_ref), f);
    return *this;
  }
  
  template<typename UnaryFunction> bool all(UnaryFunction f) const {
    return std::all_of(std::begin(_container_ref), std::end(_container_ref), f);
  }
  template<typename UnaryFunction> bool any(UnaryFunction f) const {
    return std::any_of(std::begin(_container_ref), std::end(_container_ref), f);
  }
  template<typename UnaryFunction> bool none(UnaryFunction f) const {
    return std::none_of(std::begin(_container_ref), std::end(_container_ref), f);
  }
  
  cstream<C> copy() const {
    C c(_container_ref);
    return cstream<C>{std::move(c)};
  };
  
  std::size_t size() const {
    return _container_ref.size();
  }
  template<typename UnaryFunction>
  std::size_t count(UnaryFunction f) const {
    return std::count_if(std::begin(_container_ref), std::end(_container_ref), f);
  }
  std::size_t count(const value_type &v) const {
    return std::count(std::begin(_container_ref), std::end(_container_ref), v);
  }
  
  std::size_t contains(const value_type &value) const {
    return any([&](const value_type &v){ return value == v; });
  }

  
  value_type min() const {
    return *std::min_element(std::begin(_container_ref), std::end(_container_ref));
  }
  
  value_type max() const {
    return *std::max_element(std::begin(_container_ref), std::end(_container_ref));
  }
  
  double mean() const {
    return static_cast<double>(accumulate()) / static_cast<double>(size());
  }
  
  optional<value_type> first() const {
    if(std::begin(_container_ref) == std::end(_container_ref) )
      return {};
    return { *std::begin(_container_ref) };
  }
  
  template<typename UnaryFunction>
  optional<value_type> first(UnaryFunction f) const {
    auto it = std::find_if( std::begin(_container_ref), std::end(_container_ref), f );
    if(it == std::end(_container_ref))
      return {};
    return { *it };
  }
  
private:
  C __moved;
  C &_container_ref;  
};


template<typename C, typename Add = std::plus<C>> struct join_accumulate {
  C separator;
  Add adder;
  C operator()(C &dest, const C &source) const {
    return dest == C{} ? source : adder(adder(dest, separator), source);
  }
};

template<typename T> cstream<T> make_stream(T &t) {
  return cstream<T>{t};
}
template<typename T> cstream<T> make_stream(T &&t) {
  return cstream<T>{std::forward<T>(t)};
}
template<typename T> cstream<T> make_stream_copy(const T &t) {
  return cstream<T>{T{t}};
}
template<typename N, typename T = N> cstream<std::vector<N>> cpstream(T *t, std::size_t size) {
  std::vector<N> data(size);
  std::copy_n(t, size, data.begin());
  return make_stream(std::move(data));
}


}

#endif

