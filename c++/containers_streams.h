#pragma once
#ifndef GULINUX_COMMONS_CONTAINERS_STREAMS
#define GULINUX_COMMONS_CONTAINERS_STREAMS
/*
 * Copyright (C) 2016 Marco Gulino <marco@gulinux.net>
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
#include <utility>
#include <algorithm>
#include <functional>
namespace GuLinux {
template<typename C>
class cstream {
public:
  typedef typename C::value_type value_type;
  cstream(C &c) : _container_ref{c} {}
  cstream(C &&c) : __moved{c}, _container_ref{__moved} {}
  C &ref() { return _container_ref; }
  operator C&&() { return std::move(_container_ref); }
  
  // Operations
  template<typename BinaryFunction = std::less<value_type>>
  cstream<C> &sorted(const BinaryFunction &op = {}) {
    std::sort(std::begin(_container_ref), std::end(_container_ref), op);
    return *this;
  }
  
  template<typename T, typename UnaryFunction>
  cstream<T> transform(UnaryFunction transform_f) const {
    T dest;
    std::transform(std::begin(_container_ref), std::end(_container_ref), std::inserter(dest, std::end(dest)), transform_f);
    return {std::move(dest)};
  }
  
  template<typename UnaryFunction>
  cstream<C> &remove(UnaryFunction remove_f) {
    _container_ref.erase(std::remove_if(std::begin(_container_ref), std::end(_container_ref), remove_f), std::end(_container_ref));
    return *this;
  }
  
    
  template<typename UnaryFunction>
  cstream<C> &filter(UnaryFunction filter) {
    return remove([&](const value_type &v) { return ! filter(v); });
  }
  
  
  template<typename UnaryFunction = std::plus<value_type>>
  value_type accumulate(value_type initial = {}, UnaryFunction op = {}) const {
    return std::accumulate(std::begin(_container_ref), std::end(_container_ref), initial, op);
  }
  
  template<typename UnaryFunction>
  cstream<C> &for_each(UnaryFunction f) {
    std::for_each(std::begin(_container_ref), std::end(_container_ref), f);
    return *this;
  }
  
  template<typename UnaryFunction>
  bool all(UnaryFunction f) const {
    return std::all_of(std::begin(_container_ref), std::end(_container_ref), f);
  }
  template<typename UnaryFunction>
  bool any(UnaryFunction f) const {
    return std::any_of(std::begin(_container_ref), std::end(_container_ref), f);
  }
  template<typename UnaryFunction>
  bool none(UnaryFunction f) const {
    return std::none_of(std::begin(_container_ref), std::end(_container_ref), f);
  }
  
  std::size_t size() const {
    return _container_ref.size();
  }
  template<typename UnaryFunction>
  std::size_t count(UnaryFunction f) const {
    return std::count_if(std::begin(_container_ref), std::end(_container_ref), f);
  }
private:
  C __moved;
  C &_container_ref;
  
  template<typename T> auto get_inserter(T &t) -> decltype(t.push_back({}), std::back_inserter<T>(t)) {
    return std::back_inserter(t);
  }
  template<typename T> auto get_inserter(T &t) -> decltype(t.insert({}), std::inserter<T>(t, t.end())) {
    return std::inserter(t, t.end());
  }
  
};
}

#endif