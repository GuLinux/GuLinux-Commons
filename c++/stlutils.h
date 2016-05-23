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

class benchmark {
public:
  typedef std::function<void(const std::string &, int, double)> BenchmarkCall;
  benchmark(const std::string &name, BenchmarkCall benchmark_f, int print_every = 20) 
    : name(name), print_every(print_every), benchmark_f(benchmark_f), started{std::chrono::steady_clock::now()} {}
  ~benchmark() {
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>( std::chrono::steady_clock::now() - started);
    static std::map<std::string, std::vector<double>> timers;
    timers[name].push_back(elapsed.count());
    if(timers[name].size() >= print_every) {
      benchmark_f(name, timers[name].size(), std::accumulate(std::begin(timers[name]), std::end(timers[name]), 0.)/timers[name].size());
      timers[name].clear();
    }
  }
  template<typename T>
  static BenchmarkCall benchmark_stream(T &stream, const std::string &prefix = {}) { 
    return [&](const std::string &name, int elements, double elapsed){ stream << "benchmark " << prefix << name << " (avg): " << std::fixed << std::setprecision(6) << elapsed*1000. << " ms" << std::endl; };
  }
private:
  const std::string name;
  const int print_every;
  BenchmarkCall benchmark_f;
  std::chrono::steady_clock::time_point started;
};
#define CUR_POS std::string{__FUNCTION__} + " " + std::string{__FILE__} + " "
#define benchmark_scope(name, ...) GuLinux::benchmark name{#name, GuLinux::benchmark::benchmark_stream(std::cerr ), __VA_ARGS__};
#define benchmark_start(name, ...) auto name = new GuLinux::benchmark{#name, GuLinux::benchmark::benchmark_stream(std::cerr ), __VA_ARGS__};
#define benchmark_end(name) delete name;

}

#endif
