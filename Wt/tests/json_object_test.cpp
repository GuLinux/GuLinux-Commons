#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#ifdef IN_IDE_PARSER
#define BOOST_CHECK_WITH_ARGS_IMPL(...) (void) __VA_ARGS__
#endif // IN_IDE_PARSER

#include "json_object.h"

#include <Wt/Json/Object>
#include <Wt/Json/Serializer>
#include <Wt/Json/Parser>

using namespace std;

class AnObject : public WtCommons::Json::Object {
public:
    AnObject() {
        addField<string>("string", _string).addField<int>("number", _number);
    }
    AnObject(int a_number, const string &a_string) : _number(a_number), _string(a_string) {
        addField<string>("string", _string).addField<int>("number", _number);
    }
    std::string _string;
    int _number;
};


class AnotherObject : public WtCommons::Json::Object {
public:
    AnotherObject() {
        addField<string>("another_string", _string).addField<int>("another_number", _number).addField<WtCommons::Json::Object>("an_object", _anObject);
    }
    AnotherObject(int a_number, const string &a_string, const AnObject &anObject) : _number(a_number), _string(a_string), _anObject(anObject) {
        addField<string>("another_string", _string).addField<int>("another_number", _number).addField<WtCommons::Json::Object>("an_object", _anObject);
    }
    std::string _string;
    int _number;
    AnObject _anObject;
};

BOOST_AUTO_TEST_CASE(TestFieldWithString) {
   std::string foo("fuffa");
   WtCommons::Json::Object::Field field;
   field.p = &foo;
   field.valueConverter = (new WtCommons::Json::Value<std::string>);
   BOOST_REQUIRE_EQUAL(foo, field.value<std::string>());
}
BOOST_AUTO_TEST_CASE(TestConstruction) {
    AnObject anObject(5, "3");

    Wt::Json::Object o;
    Wt::Json::parse(R"({"number" : 5, "string" : "3" })", o);

    BOOST_REQUIRE_EQUAL(Wt::Json::serialize(o), anObject.toJson());
}

BOOST_AUTO_TEST_CASE(TestUnixTime) {
  struct TimeObject : public WtCommons::Json::Object {
    TimeObject() { addField<boost::posix_time::ptime>("aTime", time); }
    boost::posix_time::ptime time;
  } timeObject;
  timeObject.time = boost::posix_time::from_iso_string("20141012T123323");
  Wt::Json::Object o;
  Wt::Json::parse(R"({"aTime" : 1413117203 })", o);

  BOOST_REQUIRE_EQUAL(Wt::Json::serialize(o), timeObject.toJson());
  
  timeObject.time = {};
  timeObject.fromJson(R"({"aTime" : 1413117203 })");
  BOOST_REQUIRE_EQUAL(boost::posix_time::from_iso_string("20141012T123323"), timeObject.time);
}

BOOST_AUTO_TEST_CASE(TestUnixTimeAsPosixString) {
  struct TimeObject : public WtCommons::Json::Object {
    TimeObject() { addField<boost::posix_time::ptime>("aTime", time, new WtCommons::Json::PosixTimeValue); }
    boost::posix_time::ptime time;
  } timeObject;
  timeObject.time = boost::posix_time::from_iso_string("20141012T123323");
  Wt::Json::Object o;
  Wt::Json::parse(R"({"aTime" : "20141012T123323" })", o);

  BOOST_REQUIRE_EQUAL(Wt::Json::serialize(o), timeObject.toJson());
  
  timeObject.time = {};
  timeObject.fromJson(R"({"aTime" : "20141012T123323" })");
  BOOST_REQUIRE_EQUAL(boost::posix_time::from_iso_string("20141012T123323"), timeObject.time);

}

/*

BOOST_AUTO_TEST_CASE(TestConstructionWithArray) {
    AnObjectWithAnArray<int> anObject;
    anObject._number = 4;
    anObject.anArray.push_back(5);
    anObject.anArray.push_back(6);
    anObject.anArray.push_back(9);

    Wt::Json::Object o;
    Wt::Json::parse(R"({"number" : 4, "an-array" : [5, 6, 9] })", o);

    BOOST_REQUIRE_EQUAL(Wt::Json::serialize(o), anObject.toJson());
}
/*
BOOST_AUTO_TEST_CASE(TestConstructionWithArrayOfObjects) {
    AnObjectWithAnArray<std::shared_ptr<AnObject>> anObject;
    anObject._number = 4;
    anObject.anArray.push_back(std::make_shared<AnObject>(3, "5"));
    anObject.anArray.push_back(std::make_shared<AnObject>(5, "7"));
    anObject.anArray.push_back(std::make_shared<AnObject>(7, "9"));

    Wt::Json::Object o;
    Wt::Json::parse(R"({"number" : 4, "an-array" : [5, 6, 9] })", o);

    BOOST_REQUIRE_EQUAL(Wt::Json::serialize(o), anObject.toJson());
}
*/

BOOST_AUTO_TEST_CASE(TestNestedObjectConstruction) {
    AnObject anObject(5, "3");
    AnotherObject anotherObject(233, "123", anObject);

    Wt::Json::Object o;
    Wt::Json::parse(R"({
                    "an_object" : {
                        "number" : 5,
                        "string" : "3"
                    },
                    "another_number" : 233,
                    "another_string" : "123"
                })", o);

    BOOST_REQUIRE_EQUAL(Wt::Json::serialize(o), anotherObject.toJson());
}

BOOST_AUTO_TEST_CASE(Parsing) {
    AnObject anObject;

    anObject.fromJson(R"({"number" : 5, "string" : "3" })");

    BOOST_REQUIRE_EQUAL(5, anObject._number);
    BOOST_REQUIRE_EQUAL("3", anObject._string);
}

BOOST_AUTO_TEST_CASE(ParsingNestes) {
    AnotherObject anObject;

    anObject.fromJson(R"({
                      "an_object" : {
                          "number" : 5,
                          "string" : "3"
                      },
                      "another_number" : 233,
                      "another_string" : "123"
                  })");

    BOOST_REQUIRE_EQUAL(233, anObject._number);
    BOOST_REQUIRE_EQUAL("123", anObject._string);
    BOOST_REQUIRE_EQUAL("3", anObject._anObject._string);
    BOOST_REQUIRE_EQUAL(5, anObject._anObject._number);
}

BOOST_AUTO_TEST_CASE(TestArray) {
  std::vector<int> anArray {1, 2, 5, 69, 42 };
  WtCommons::Json::Array<int, WtCommons::Json::Vector> a(anArray);
  Wt::Json::Array expected;
  for(auto i: anArray)
    expected.push_back({i});
  BOOST_REQUIRE_EQUAL(Wt::Json::serialize(expected), a.toJson());
}
BOOST_AUTO_TEST_CASE(TestArraySyncronization) {
  std::vector<int> anArray {1, 2, 5, 69, 42 };
  WtCommons::Json::Array<int, WtCommons::Json::Vector> a(anArray);
  Wt::Json::Array expected;
  for(auto i: anArray)
    expected.push_back({i});
  BOOST_REQUIRE_EQUAL(Wt::Json::serialize(expected), a.toJson());
  anArray.push_back(999);
  expected.push_back(999);
  BOOST_REQUIRE_EQUAL(Wt::Json::serialize(expected), a.toJson());
}


using AnObjectConverter = WtCommons::Json::Value<WtCommons::Json::Object>;
BOOST_AUTO_TEST_CASE(TestArrayOfObjects) {
  std::vector<AnObject> anArray;
  AnObject first(12, "first");
  AnObject second(13, "second");
  anArray.push_back(first);
  anArray.push_back(second);
  WtCommons::Json::Array<AnObject, WtCommons::Json::Vector, AnObjectConverter> a(anArray);
  Wt::Json::Array expected;
  for(auto i: anArray) {
    Wt::Json::Value v(Wt::Json::ObjectType);
    Wt::Json::Object &o = v;
    o = i.toWtObject();
    expected.push_back(v);
  }
  BOOST_REQUIRE_EQUAL(Wt::Json::serialize(expected), a.toJson());
}

using AnObjectPointerConverter = WtCommons::Json::PointerObjectConverter<AnObject, std::shared_ptr>;

BOOST_AUTO_TEST_CASE(TestArrayOfObjectPointers) {
  std::vector<std::shared_ptr<AnObject>> anArray;
  auto first = make_shared<AnObject>(12, "first");
  auto second = make_shared<AnObject>(13, "second");
  anArray.push_back(first);
  anArray.push_back(second);
  WtCommons::Json::Array<std::shared_ptr<AnObject>, WtCommons::Json::Vector, AnObjectPointerConverter> a(anArray);
  Wt::Json::Array expected;
  for(auto i: anArray) {
    Wt::Json::Value v(Wt::Json::ObjectType);
    Wt::Json::Object &o = v;
    o = i->toWtObject();
    expected.push_back(v);
  }
  BOOST_REQUIRE_EQUAL(Wt::Json::serialize(expected), a.toJson());
}


template<typename T>
class AnObjectWithAnArray : public WtCommons::Json::Object {
public:
    AnObjectWithAnArray() {
        addField<int>("number", _number);
        addField<T, WtCommons::Json::Vector>("an-array", anArray);
    }
    WtCommons::Json::Vector<T> anArray;
    int _number;
};


BOOST_AUTO_TEST_CASE(TestArrayOfObjectPointersInsideAnObject) {
  auto first = make_shared<AnObject>(12, "first");
  auto second = make_shared<AnObject>(13, "second");
  AnObjectWithAnArray<std::shared_ptr<AnObject>> aComplexObject;
  aComplexObject._number = 4;
  aComplexObject.anArray.push_back(first);
  aComplexObject.anArray.push_back(second);
}
