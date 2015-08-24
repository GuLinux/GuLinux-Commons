#ifndef WOBJECT_SCOPE_H
#define WOBJECT_SCOPE_H

#include <Wt/WObject>
#include <functional>

namespace WtCommons {

class WObjectScope : public Wt::WObject {
public:
  WObjectScope(std::function<void()> f, WObject *parent) : f(f), Wt::WObject(parent) {};
  ~WObjectScope() { f(); }
private:
  std::function<void()> f;
};
}
#endif