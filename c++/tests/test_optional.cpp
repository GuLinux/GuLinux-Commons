#include <gtest/gtest.h>
#include <set>
#include <map>
#include <unordered_map>
#include <list>
#include "optional.h"
using namespace GuLinux;
using namespace std;

TEST(Optional, empty) {
  ASSERT_FALSE( (optional<int>{}) );
}

TEST(Optional, constructor_value) {
  auto val = optional<int>{5};
  ASSERT_TRUE( val );
  ASSERT_EQ(5, *val);
}

TEST(Optional, reset) {
  auto val = optional<int>{5};
  val.reset();
  ASSERT_FALSE( val );
}


TEST(Optional, set) {
  auto val = optional<int>{};
  ASSERT_FALSE( val );
  val.set(3);
  ASSERT_TRUE( val );
  ASSERT_EQ(3, *val);
}

TEST(Optional, copy) {
  auto val = optional<int>{5};
  auto val2 = val;
  ASSERT_TRUE(val2);
  ASSERT_EQ(5, *val2);
}