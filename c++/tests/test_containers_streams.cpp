#include <gtest/gtest.h>
#include "containers_streams.h"
using namespace GuLinux;
using namespace std;
TEST(ContainersStream, acceptsContainerReference) {
  vector<int> foo{1,2,4,6,8,10};
  cstream<vector<int>> c(foo);
  
  ASSERT_EQ(foo, c.ref());
}

TEST(ContainersStream, acceptsContainerMove) {
  vector<int> foo{1,2,4,6,8,10};
  cstream<vector<int>> c(vector<int>{1,2,4,6,8,10});
  vector<int> actual = c;
  ASSERT_EQ(foo, actual);
}

TEST(ContainersStream, sort) {
  cstream<vector<int>> c{{1, 5, 34, 6, 3}};
  vector<int> actual = c.sorted();
  vector<int> expected{1, 3, 5, 6, 34};
  ASSERT_EQ(expected, actual);
}


TEST(ContainersStream, sort_custom_compare) {
  cstream<vector<int>> c{{1, 5, 34, 6, 3}};
  vector<int> actual = c.sorted([](int a, int b){ return a > b; });
  vector<int> expected{34, 6, 5, 3, 1};
  ASSERT_EQ(expected, actual);
}

TEST(ContainersStream, transform) {
  cstream<vector<int>> c{{1, 5, 34, 6, 3}};
  vector<string> actual = c.sorted().transform<vector<string>>([](int a){ stringstream s; s << a; return s.str(); });
  vector<string> expected{"1", "3", "5", "6", "34"};
  ASSERT_EQ(expected, actual);
}