#include <gtest/gtest.h>
#include <set>
#include <map>
#include <unordered_map>
#include <list>
#include "containers_streams.h"
using namespace GuLinux;
using namespace std;
TEST(ContainersStream, acceptsContainerReference) {
  vector<int> foo{1,2,4,6,8,10};
  auto c = make_stream(foo);
  
  ASSERT_EQ(foo, c.get_ref());
}

TEST(ContainersStream, acceptsContainerMove) {
  vector<int> foo{1,2,4,6,8,10};
  auto c = make_stream(vector<int>{1,2,4,6,8,10});
  vector<int> actual = c;
  ASSERT_EQ(foo, actual);
}

TEST(ContainersStream, acceptsConstRef) {
  vector<int> foo{1,2,4,6,8,10};
  auto mk_stream_f = [](const vector<int> &v) { return make_stream_copy(v); };
  auto c = mk_stream_f(vector<int>{1,2,4,6,8,10});
  vector<int> actual = c;
  ASSERT_EQ(foo, actual);
}

TEST(ContainersStream, sort) {
  auto c = make_stream(vector<int>{1, 5, 34, 6, 3});
  vector<int> actual = c.sorted();
  vector<int> expected{1, 3, 5, 6, 34};
  ASSERT_EQ(expected, actual);
}


TEST(ContainersStream, sort_custom_compare) {
  auto c = make_stream(vector<int>{1, 5, 34, 6, 3});
  vector<int> actual = c.sorted([](int a, int b){ return a > b; });
  vector<int> expected{34, 6, 5, 3, 1};
  ASSERT_EQ(expected, actual);
}

TEST(ContainersStream, transform) {
  auto c = make_stream(vector<int>{1, 5, 34, 6, 3});
  vector<string> actual = c.sorted().transform<vector<string>>([](int a){ stringstream s; s << a; return s.str(); });
  vector<string> expected{"1", "3", "5", "6", "34"};
  ASSERT_EQ(expected, actual);
}

TEST(ContainersStream, transform_to_set) {
  auto c = make_stream(vector<int>{1, 3, 34, 6, 3});
  set<int> actual = c.sorted().transform<set<int>>(identity<int>{});
  set<int> expected{1, 3, 34, 6};
  ASSERT_EQ(expected, actual);
}


TEST(ContainersStream, transform_to_map) {
  auto c = make_stream(vector<int>{1, 3, 34, 6, 3});
  map<int, int> actual = c.sorted().transform<map<int, int>>([](int a){ return make_pair(a+1, a); });
  map<int, int> expected{ {2, 1}, {4, 3}, {35, 34}, {7, 6}};
  ASSERT_EQ(expected, actual);
}


TEST(ContainersStream, remove) {
  auto c = make_stream(vector<int>{1, 5, 34, 6, 3});
  vector<int> actual = c.remove([](int a){ return a %2 == 0; });
  vector<int> expected{1, 5, 3};
  ASSERT_EQ(expected, actual);
}



TEST(ContainersStream, erase_on_map) {
  map<string, int> my_map_expected{ {"3", 3}, {"4", 4} };
  map<string, int> actual = make_stream(map<string, int>{ {"2", 2}, {"3", 3}, {"4", 4} }).remove_ms([](const pair<string,int> &p) { return p.second == 2; });
  ASSERT_EQ(my_map_expected, actual);
}


TEST(ContainersStream, erase_on_set) {
  set<int> expected{1, 3, 5};
  set<int> actual = make_stream(set<int>{1, 2, 3, 4, 5}).remove_ms([](int i) { return i%2==0; });
  ASSERT_EQ(expected, actual);
}



TEST(ContainersStream, filter) {
  auto c = make_stream(vector<int>{1, 5, 34, 6, 3});
  vector<int> actual = c.filter([](int a){ return a %2 != 0; });
  vector<int> expected{1, 5, 3};
  ASSERT_EQ(expected, actual);
}


TEST(ContainersStream, filter_map) {
  auto c = make_stream(map<int, string>{ {1, "1"}, {5, "5"}, {34, "34"} } );
  map<int, string> actual = c.filter_ms( [](const pair<int, string> &a){ return a.first %2 != 0; } );
  map<int, string> expected{ {1, "1"}, {5, "5"} };
  ASSERT_EQ(expected, actual);
}

TEST(ContainersStream, filter_unordered_map) {
  auto c = make_stream(unordered_map<int, string>{ {1, "1"}, {5, "5"}, {34, "34"} } );
  unordered_map<int, string> actual = c.filter_ms( [](const pair<int, string> &a){ return a.first %2 != 0; } );
  unordered_map<int, string> expected{ {1, "1"}, {5, "5"} };
  ASSERT_EQ(expected, actual);
}

TEST(ContainersStream, accumulate) {
  auto c = make_stream(vector<int>{1, 5, 34, 6, 3});
  int actual = c.accumulate();
  ASSERT_EQ(49, actual);
}


TEST(ContainersStream, accumulate_custom) {
  auto c = make_stream(vector<int>{1, 5, 6});
  int actual = c.accumulate(3, [](int initial, int current){ return initial*current; });
  ASSERT_EQ(90, actual);
}


TEST(ContainersStream, accumulate_custom2) {
  auto c = make_stream(vector<string>{"a1", "b5", "c6"});
  string actual = c.accumulate(string{}, join_accumulate<string>{" "});
  ASSERT_EQ("a1 b5 c6", actual);
}


TEST(ContainersStream, foreach) {
  auto c = make_stream(vector<int>{1, 5, 6});
  int sum = 0;
  c.for_each([&](int i) {sum += i; });
  ASSERT_EQ(12, sum);
}


TEST(ContainersStream, all_any_none_of) {
  auto c = make_stream(vector<int>{1, 5, 7});
  ASSERT_TRUE(c.all([](int i){ return i%2 == 1; }));
  ASSERT_FALSE(c.all([](int i){ return i < 7; }));
  
  ASSERT_TRUE(c.any([](int i){ return i < 7; }));
  ASSERT_FALSE(c.any([](int i){ return i < 1; }));
  
  ASSERT_TRUE(c.none([](int i){ return i < 1; }));
  ASSERT_FALSE(c.none([](int i){ return i < 7; }));
}


TEST(ContainersStream, count_and_size) {
  auto c = make_stream(vector<int>{1, 5, 6});
  ASSERT_EQ(3, c.size());
  ASSERT_EQ(2, c.count([](int i) {return i < 6; }));
  ASSERT_EQ(1, c.count(5));
  ASSERT_TRUE( c.contains(5) );
  ASSERT_FALSE( c.contains(4) );
}


TEST(ContainersStream, retval) {
  vector<int> expected{1, 5, 6};
  struct X {
    vector<int> y() const { return make_stream(vector<int>{1, 5, 6}); }
  };
  X x;
  ASSERT_EQ(expected, x.y());
}


TEST(ContainersStream, flatten) {
  auto c = make_stream(vector<vector<int>>{ {1,2,3}, {4,5,6}, {7,8,9} });
  vector<int> expected{1,2,3,4,5,6,7,8,9};
  auto actual = c.accumulate({}, container_accumulate<vector<int>>{});
  ASSERT_EQ(expected, actual);
}

TEST(ContainersStream, copy) {
  vector<int> v{1,2,3};
  vector<int> expected{1,2,3};
  auto c = make_stream(v);
  expected[1]++; v[1]++;
  ASSERT_EQ(expected, c.get_ref());
  auto c_copy = c.copy();
  v[1]++;
  ASSERT_EQ(expected, c_copy.get_ref());
}

TEST(ContainersStream, min_max_mean) {
  auto stream = make_stream(vector<int>{5, 3, 6, 1, 7, 3, 5, 2, 9, 0, 20, 2});
  ASSERT_EQ(0, stream.min());
  ASSERT_EQ(20, stream.max());
  ASSERT_EQ(5.25, stream.mean());
}

TEST(ContainersStream, first) {
  ASSERT_TRUE( make_stream<vector<int>>({5, 2, 3}).first() );
  ASSERT_EQ(5, *make_stream<vector<int>>({5, 2, 3}).first() );
  ASSERT_FALSE( make_stream<vector<int>>({}).first() );
  ASSERT_FALSE( make_stream<list<int>>({}).first() );
  ASSERT_EQ(5, *make_stream<list<int>>({5, 2, 3}).first() );
}

TEST(ContainersStream, first_with_predicate) {
  auto is_even = [](int n) { return n%2 == 0; };
  auto gt_5 = [](int n) { return n > 5; };
  ASSERT_TRUE( make_stream<vector<int>>({5, 2, 3}).first( is_even ) );
  ASSERT_EQ(2, *make_stream<vector<int>>({5, 2, 3}).first( is_even ) );
  
  ASSERT_FALSE( make_stream<vector<int>>({5, 2, 3}).first(gt_5) );
  ASSERT_FALSE( make_stream<list<int>>({5, 2, 3}).first(gt_5) );
  
  ASSERT_FALSE( make_stream<vector<int>>({}).first(is_even) );
  ASSERT_FALSE( make_stream<list<int>>({}).first(is_even) );
  
  ASSERT_EQ(2, *make_stream<list<int>>({5, 2, 3}).first(is_even) );
}


TEST(ContainersStream, copy_from_array) {
  const char *data[] = {"hello", "world", "1", "2", "3"};
  ASSERT_EQ( (vector<string>{"hello", "world", "1", "2", "3"}),( cpstream<string>(data, 5).get() ) );
}


TEST(ContainersStream, typical_usage) {
  vector<int> v{1,2,3,4,5};
  list<pair<int, string>> expected{ {2, "1c"}, {3, "2c"}, {6, "5c"} };
  list<pair<int, string>> actual = make_stream(v)
    .transform<map<string, int>>([](int i){ stringstream s; s << i; return make_pair(s.str(), i); })
    .erase("3")
    .transform<list<pair<int, string>>>([](const pair<string, int> &p){ return make_pair(p.second, p.first); })
    .remove([](const pair<int, string> &p){ return p.first == 4; })
    .for_each([](pair<int, string> &p) { p.first++; p.second += "c"; });
    ;
  ASSERT_EQ(expected, actual);
}



#define VECTOR_SIZE 10000000
TEST(BenchmarkContainersStream, ref_constructor) {
  vector<long> v(VECTOR_SIZE);
  cstream<vector<long>> c{v};
}

TEST(BenchmarkContainersStream, _helper) {
  vector<long> v(VECTOR_SIZE);
  auto c = make_stream(v);
}


TEST(BenchmarkContainersStream, move_constructor) {
  cstream<vector<long>> c{vector<long>(VECTOR_SIZE)};
}

TEST(BenchmarkContainersStream, move_helper) {
 auto c = make_stream(vector<long>(VECTOR_SIZE));
}