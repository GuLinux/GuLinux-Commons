#include "../stlutils.h"
#include <sstream>
#include <string>
using namespace std;
using namespace GuLinux;
void assert(bool condition, const std::string &message, const std::string &file, const std::string &function, int line) {
  if(!condition) {
    stringstream msg;
    msg << "assertion failed: " << message << " (" << file << ":" << line << ", " << function << ")" << endl;
    throw runtime_error(msg.str());
  }
}

inline string to_string(const std::string &s) { return s; }
template<typename T> inline ostream &operator<<(ostream &s, pair<T, T> p) {
  s << "{" << p.first << ", " << p.second  << "}";
  return s;
}
template<typename T> string to_string(const vector<T> &v) {
  stringstream s;
  string sep;
  s << "[";
  for(auto value: v) {
    s << sep << value;
    sep = ", ";
  }
  s << "]";
  return s.str();
}
#define COMMON_ASSERT(condition, message) assert(condition, message, __FILE__, __FUNCTION__, __LINE__)
#define ASSERT_EQUALS(a, b) COMMON_ASSERT( a == b ,  string{#a "("} + to_string(a) +  string{")==" #b "("} + to_string(b) + string{")"} )


void test_xy_cycle() {
  vector<pair<int, int>> points;
  foreach_point<int>(5, 4 , [&points](int x, int y) { points.push_back({x, y}); } );
  vector<pair<int, int>> expected;
  for(int i=0; i<4; i++) for(int j=0; j<5; j++) expected.push_back({j, i});
  ASSERT_EQUALS(points, expected);
}



void test_xy_cycle_benchmark() {
  {
    benchmark_scope(plain_for, 1);
    vector<int> result;
    auto lambda = [&result](int x, int y) { result.push_back(x*y); };
    for(int y = 0; y < 1300; y++)
      for(int x=0; x<1200; x++)
        lambda(x, y);
  }
  {
    benchmark_scope(foreach_point_lambda, 1);
    vector<int> result;
    foreach_point<int>(1200, 1300, [&result](int x, int y) { result.push_back(x*y); });
  }
}

int main() {
  try {
    test_xy_cycle();
    test_xy_cycle_benchmark();
  } catch(std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}