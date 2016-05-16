/*
 * Copied by https://github.com/GuLinux/GuLinux-Commons/blob/master/c%2B%2B/containers_streams.h
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

#ifndef GULINUX_COMMONS_OPTIONAL
#define GULINUX_COMMONS_OPTIONAL

namespace GuLinux {
  
template<typename T>
class optional {
public:
  optional() {}
  optional(const T &value) { set(value); }
  void reset() { is_set = false; }
  void set(const T &value) { this->value = value; is_set = true; }
  operator bool() const { return is_set; }
  T operator*() const { return value; }
  T &operator*() { return value; }
private:
  bool is_set = false;
  T value;
};
}

#endif