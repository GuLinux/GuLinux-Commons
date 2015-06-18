#define BOOST_TEST_MODULE WtUtilsTest
#include <boost/test/unit_test.hpp>

#ifdef IN_IDE_PARSER
#define BOOST_CHECK_WITH_ARGS_IMPL(...) (void) __VA_ARGS__
#endif // IN_IDE_PARSER

#include "Wt-Commons/wt_utils.h"

using namespace std;

BOOST_AUTO_TEST_CASE(TestWStringLiteral) {
  auto str = "hello"_ws;
  auto expected = Wt::WString{"hello"};
  BOOST_REQUIRE_EQUAL( typeid(str).name(), typeid(expected).name() );
  BOOST_REQUIRE_EQUAL(str, expected);
}
BOOST_AUTO_TEST_CASE(TestLiteralTranslation) {
  auto str = "untranslated_hello"_wtr;
  auto expected = Wt::WString{"??untranslated_hello??"};
  BOOST_REQUIRE_EQUAL( typeid(str).name(), typeid(expected).name() );
  BOOST_REQUIRE_EQUAL(str, expected);
}

BOOST_AUTO_TEST_CASE(TestWStringArgs) {
  BOOST_REQUIRE_EQUAL( "string with {1} {2} {3} {4}"_ws % 1 % 22 % 55 % "abc", "string with 1 22 55 abc");
}

BOOST_AUTO_TEST_CASE(TestStringFormatter) {
  std::string actual = "string with %d %s"_fmt % 1 % "abc";
  BOOST_REQUIRE_EQUAL(actual, std::string{"string with 1 abc"});
}

BOOST_AUTO_TEST_CASE(TestWStringFormatter) {
  Wt::WString actual = "string with %d %s"_wfmt % 1 % "abc";
  BOOST_REQUIRE_EQUAL(actual, Wt::WString{"string with 1 abc"});
}