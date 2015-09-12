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

namespace GuLinux {
class Scope {
public:
  Scope(const std::function<void()> &onExit) : onExit(onExit) {}
  ~Scope() { onExit(); }
private:
  std::function<void()> onExit;
};




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
#define benchmark_scope(name) GuLinux::benchmark name{#name, GuLinux::benchmark::benchmark_stream(std::cerr )};
#define benchmark_start(name) auto name = new GuLinux::benchmark{#name, GuLinux::benchmark::benchmark_stream(std::cerr )};
#define benchmark_end(name) delete name;

}

#endif
