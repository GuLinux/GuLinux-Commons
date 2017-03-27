/*
 * Copyright (C) 2016  Marco Gulino <marco@gulinux.net>
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

#ifndef WIN32

#ifndef _STACKTRACE_H_
#define _STACKTRACE_H_

#include <ostream>
#include <string>
#include <vector>

namespace GuLinux {
namespace Backtrace {
struct Frame {
  void *address;
  std::string symbol;
  std::string file() const;
  std::string function() const;
  typedef std::vector<Frame> vector;
};
Frame::vector backtrace(uint32_t max_frames = 50, uint32_t skip = 0);

}
}

std::ostream &operator<<(std::ostream &o, const GuLinux::Backtrace::Frame &frame);
std::ostream &operator<<(std::ostream &o, const GuLinux::Backtrace::Frame::vector &frames);

#endif // _STACKTRACE_H_
#endif // WIN32
