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
  template<typename Op = std::less<value_type>>
  cstream<C> &sorted(const Op &op = {}) {
    std::sort(std::begin(_container_ref), std::end(_container_ref), op);
    return *this;
  }
  template<typename T, typename Op>
  cstream<T> transform(Op transform_f) {
    T dest;
    std::transform(std::begin(_container_ref), std::end(_container_ref), std::back_inserter(dest), transform_f);
    return {std::move(dest)};
  }
private:
  C __moved;
  C &_container_ref;
};
}

#endif