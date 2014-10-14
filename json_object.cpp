#include "json_object.h"
#include <Wt/Json/Object>
#include <Wt/Json/Serializer>
#include <Wt/Json/Parser>
#include <Wt/Json/Array>

using namespace std;

namespace WtCommons {
namespace Json {
template<typename T> class Value {
public:
    Value(void *p) : v(*reinterpret_cast<T*>(p)) {}
    operator T&() const { return v;}
    operator Wt::Json::Value() const { return {v}; }
    void set(const Wt::Json::Value &t) { v = t; }
private:
    T &v;
};

template<typename V, template<typename> class C> class Container {
public:
    Container(void *p) : v(*reinterpret_cast<C<V>*>(p)) {}
    operator C<V>&() const { return v;}
    operator Wt::Json::Value() const { return {v}; }
    void set(const Wt::Json::Value &t) { /* TODO */ }
private:
    C<V> &v;
};

template<typename T> using Vector = vector<T>;

template<> class Value<boost::posix_time::ptime> {
public:
    Value(void *p) : v(*reinterpret_cast<boost::posix_time::ptime*>(p)) {}
    operator boost::posix_time::ptime&() const { return v;}
    operator Wt::Json::Value() const { return {boost::posix_time::to_iso_string(v)}; }
    void set(const Wt::Json::Value &t) { v = boost::posix_time::from_iso_string(t); }
private:
    boost::posix_time::ptime &v;
};
template<> class Value<string> {
public:
    Value(void *p) : v(*reinterpret_cast<string*>(p)) {}
    operator string&() const { return v;}
    operator Wt::Json::Value() const { return {v}; }
    void set(const Wt::Json::Value &t) { v = t.toString().orIfNull(""); }
private:
    string &v;
};

template<> class Value<Object> {
public:
    Value(void *p) : v(*reinterpret_cast<Object*>(p)) {}
    operator Object&() const { return v;}
    operator Wt::Json::Value() const {
        Wt::Json::Value another(Wt::Json::ObjectType);
        Wt::Json::Object &o = another;
        o = {this->v.toWtObject()};
        return another;
    }
    void set(const Wt::Json::Value &t) {v.from(t); }
private:
    Object &v;

};


template<> class Object::Field::Builder<std::string> {
public:
    static Object::Field asField(std::string &s) { return {&s, Object::Field::String}; }
};
template<> class Object::Field::Builder<int> {
public:
    static Object::Field asField(int &i) { return {&i, Object::Field::Int}; }
};
template<> class Object::Field::Builder<long long> {
public:
    static Object::Field asField(long long &i) { return {&i, Object::Field::LongLong}; }
};
template<> class Object::Field::Builder<boost::posix_time::ptime> {
public:
    static Object::Field asField(boost::posix_time::ptime &v) { return {&v, Object::Field::DateTime}; }
};
template<> class Object::Field::Builder<WtCommons::Json::Object> {
public:
    static Object::Field asField(WtCommons::Json::Object &v) { return {&v, Object::Field::Object}; }
};
template<> class Object::Field::ContainerBuilder<int, Vector> {
public:
    static Object::Field asField(std::vector<int> &v) { return {&v, Object::Field::Vector, Object::Field::Int}; }
};


string Object::toJson() const {
    return Wt::Json::serialize(toWtObject() );
}


template Object &Object::addField(const std::string &, std::string &field);
template Object &Object::addField(const std::string &, int &field);
template Object &Object::addField(const std::string &, long long &field);
template Object &Object::addField(const std::string &, boost::posix_time::ptime &field);
template Object &Object::addField(const std::string &, Object &field);
template Object &Object::addField<int, Vector>(const std::string &, std::vector<int> &field);


template<typename T> void toValue(Wt::Json::Value &v, void *p) {
    v = Value<T>(p);
}

template<typename T> void fromValue(Wt::Json::Value &v, void *p) {
  Value<T>(p).set(v);
}

template<typename V, template<typename> class C> void toContainer(Wt::Json::Value &v, void *p) {
    Wt::Json::Value _v(Wt::Json::ArrayType);
    Wt::Json::Array &a = _v;
    C<V>  _container = Container<V, C>{p};
    std::copy(begin(_container), end(_container), back_inserter(a));
    v = _v;
}


struct Mapping {
    typedef function<void(Wt::Json::Value &, void *)> Exporter;
    typedef function<void(Wt::Json::Value &, void *)> Importer;

    Exporter exporter;
    Importer importer;
};


#ifdef IN_IDE_PARSER
static map<Object::Field::Type, Mapping> mappings;
#else
static map<Object::Field::Type, Mapping> mappings
{
  {{Object::Field::Vector, Object::Field::Int}, {toContainer<int, Vector>}},
  {{Object::Field::Object, Object::Field::Null}, {toValue<Object>, fromValue<Object>}},
  {{Object::Field::String, Object::Field::Null}, {toValue<string>, fromValue<string>}},
  {{Object::Field::Int, Object::Field::Null}, {toValue<int>, fromValue<int>}},
  {{Object::Field::LongLong, Object::Field::Null}, {toValue<long long>, fromValue<long long>}},
  {{Object::Field::DateTime, Object::Field::Null}, {toValue<boost::posix_time::ptime>, fromValue<boost::posix_time::ptime>}},
};
#endif

Wt::Json::Object Object::toWtObject() const {
    Wt::Json::Object wtObject;
    for(auto v: fields) {
        Wt::Json::Value value;
        mappings[v.type].exporter(value, v.p);
        wtObject[v.label] = value;
    }
    return wtObject;
}


void Object::fromJson(const std::string &jsonString) {
    Wt::Json::Object wtObject;
    Wt::Json::parse(jsonString, wtObject);
    from(wtObject);
}

void Object::from(const Wt::Json::Object &object) {
    for(auto field: fields) {
        auto value = object.at(field.label);
        mappings[field.type].importer(value, field.p);
    }
}

} // namespace Json
} // namespace WtCommons
