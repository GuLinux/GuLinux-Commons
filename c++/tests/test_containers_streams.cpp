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
  
  ASSERT_EQ(foo, c.ref());
}