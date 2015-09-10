#ifndef GULINUX_STL_UTILS_H
#define GULINUX_STL_UTILS_H
#include <functional>

class Scope {
public:
  Scope(const std::function<void()> &onExit) : onExit(onExit) {}
  ~Scope() { onExit(); }
private:
  std::function<void()> onExit;
};

#endif
