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
#ifndef GULINUX_STL_UTILS_H
#define GULINUX_STL_UTILS_H
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>
#include <ratio>
#include <numeric>


namespace GuLinux {
class Scope {
public:
  Scope(const std::function<void()> &onExit) : onExit(onExit) {}
  ~Scope() { onExit(); }
private:
  std::function<void()> onExit;
};

template<typename T>
void foreach_point(T x, T y, std::function<void(T, T)> onEach) {
  for(T _y=0; _y<y; _y++) for(T _x=0; _x < x; _x++) onEach(_x, _y);
}

class benchmark : public std::enable_shared_from_this<benchmark>{
public:
  typedef std::shared_ptr<benchmark> ptr;
  typedef std::function<void(const std::string &)> PrintF;
  benchmark(const std::string &name, PrintF print_function)
   : name(name),
   print_every(1), 
   print_function(print_function), 
   started{std::chrono::steady_clock::now()},
   unit_ratio{1},
   unit_suffix{"s"}
  {
  }
  ~benchmark() {
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>( std::chrono::steady_clock::now() - started);
    static std::map<std::string, std::vector<double>> timers;
    timers[name].push_back(elapsed.count());
    if(timers[name].size() >= print_every) {
      double elapsed_secs = std::accumulate(std::begin(timers[name]), std::end(timers[name]), 0.)/timers[name].size();
      print_function(elapsed_str(timers[name].size(), elapsed_secs));
      timers[name].clear();
    }
  }
  std::string elapsed_str(std::size_t calls_count, double secs) const {
    std::stringstream s;
    s << "benchmark " << name;
    if(print_every > 1)
      s << " (avg of " << calls_count << " calls)";
    s << ": " << std::fixed << std::setprecision(6) << secs/unit_ratio << unit_suffix;
    return s.str();
  }
  
  ptr every(size_t print_every) {
    this->print_every = print_every;
    return shared_from_this();
  }
  ptr ms() {
    unit_ratio = 1./1000.;
    unit_suffix = "ms";
    return shared_from_this();
  }
  ptr us() {
    unit_ratio = 1./1000000.;
    unit_suffix = "us";
    return shared_from_this();
  }
  ptr s() {
    unit_ratio = 1;
    unit_suffix = "s";
    return shared_from_this();
  }
private:
  const std::string name;
  size_t print_every;
  PrintF print_function;
  std::string unit_suffix;
  double unit_ratio;
  std::chrono::steady_clock::time_point started;
};
}
#define CUR_POS std::string{__FUNCTION__} + " " + std::string{__FILE__} + " "
#define CREATE_BENCHMARK_BASE(name, out_function) \
  auto name = make_shared<GuLinux::benchmark>(#name, out_function)
  

#define BENCH(name) CREATE_BENCHMARK_BASE(name, [](const std::string &s){ std::cerr << s << std::endl; })
#define BENCH_END(name) name.reset();

#endif
