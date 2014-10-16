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


string Object::toJson() const {
    return Wt::Json::serialize(toWtObject() );
}



template<typename T> void toValue(Wt::Json::Value &v, const Object::Field &field) {
    v = Value<T>(field.p);
}

template<typename T> void toValueField(Wt::Json::Value &v, const Object::Field &field) {
    v = {field.value<T>()};
}

template<typename T> void fromValue(Wt::Json::Value &v, const Object::Field &field) {
  Value<T>(field.p).set(v);
}

template<typename V, template<typename> class C> void toContainer(Wt::Json::Value &v, const Object::Field &field) {
    Wt::Json::Value _v(Wt::Json::ArrayType);
    Wt::Json::Array &a = _v;
    C<V>  _container = Container<V, C>{field.p};
    std::copy(begin(_container), end(_container), back_inserter(a));
    v = _v;
}


struct Mapping {
    typedef function<void(Wt::Json::Value &, const Object::Field&)> Exporter;
    typedef function<void(Wt::Json::Value &, const Object::Field&)> Importer;

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
  {{Object::Field::String, Object::Field::Null}, {toValueField<string>, fromValue<string>}},
  {{Object::Field::Int, Object::Field::Null}, {toValue<int>, fromValue<int>}},
  {{Object::Field::LongLong, Object::Field::Null}, {toValue<long long>, fromValue<long long>}},
  {{Object::Field::DateTime, Object::Field::Null}, {toValue<boost::posix_time::ptime>, fromValue<boost::posix_time::ptime>}},
};
#endif

Wt::Json::Object Object::toWtObject() const {
    Wt::Json::Object wtObject;
    for(auto v: fields) {
        Wt::Json::Value value;
        mappings[v.type].exporter(value, v);
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
        mappings[field.type].importer(value, field);
    }
}

} // namespace Json
} // namespace WtCommons
