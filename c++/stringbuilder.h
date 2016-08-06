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
 *
 */
#ifndef CPP_STRINGBUILDER
#define CPP_STRINGBUILDER
#include <sstream>

namespace GuLinux {
class stringbuilder {
public:
    template<typename T> stringbuilder &operator<<(const T &t) { return append(t); }
    template<typename T> stringbuilder &append(const T &t) { _s << t; return *this; }
    operator std::string() const { return str(); }
    std::string str() const { return _s.str(); }
    
private:
    std::ostringstream _s;
};
}

#endif // CPP_STRINGBUILDER

