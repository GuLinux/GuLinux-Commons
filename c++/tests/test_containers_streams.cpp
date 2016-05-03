#include <gtest/gtest.h>
#include <set>
#include <map>
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

TEST(ContainersStream, transform_to_set) {
  cstream<vector<int>> c{{1, 3, 34, 6, 3}};
  set<int> actual = c.sorted().transform<set<int>>([](int a){ return a; });
  set<int> expected{1, 3, 34, 6};
  ASSERT_EQ(expected, actual);
}


TEST(ContainersStream, transform_to_map) {
  cstream<vector<int>> c{{1, 3, 34, 6, 3}};
  map<int, int> actual = c.sorted().transform<map<int, int>>([](int a){ return make_pair(a+1, a); });
  map<int, int> expected{ {2, 1}, {4, 3}, {35, 34}, {7, 6}};
  ASSERT_EQ(expected, actual);
}


TEST(ContainersStream, remove) {
  cstream<vector<int>> c{{1, 5, 34, 6, 3}};
  vector<int> actual = c.remove([](int a){ return a %2 == 0; });
  vector<int> expected{1, 5, 3};
  ASSERT_EQ(expected, actual);
}



TEST(ContainersStream, filter) {
  cstream<vector<int>> c{{1, 5, 34, 6, 3}};
  vector<int> actual = c.filter([](int a){ return a %2 != 0; });
  vector<int> expected{1, 5, 3};
  ASSERT_EQ(expected, actual);
}

TEST(ContainersStream, accumulate) {
  cstream<vector<int>> c{{1, 5, 34, 6, 3}};
  int actual = c.accumulate();
  ASSERT_EQ(49, actual);
}


TEST(ContainersStream, accumulate_custom) {
  cstream<vector<int>> c{{1, 5, 6}};
  int actual = c.accumulate(3, [](int initial, int current){ return initial*current; });
  ASSERT_EQ(90, actual);
}
