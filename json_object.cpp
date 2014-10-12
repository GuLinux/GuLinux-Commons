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
    void set(const T &t) { v = t; }
private:
    T &v;
};

template<> class Value<boost::posix_time::ptime> {
public:
    Value(void *p) : v(*reinterpret_cast<boost::posix_time::ptime*>(p)) {}
    operator boost::posix_time::ptime&() const { return v;}
    operator Wt::Json::Value() const { return {boost::posix_time::to_iso_string(v)}; }
    void set(const boost::posix_time::ptime &t) { v = t; }
private:
    boost::posix_time::ptime &v;
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
    void set(const Object &t) { v = t; }
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
template<> class Object::Field::Builder<std::vector<int>> {
public:
    static Object::Field asField(std::vector<int> &v) { return {&v, Object::Field::Vector, Object::Field::Int}; }
};


string Object::toJson() const {
    return Wt::Json::serialize(toWtObject() );
}


template<typename T>
Object &Object::addField(const std::string &name, T &field) {
    fields[name] = Field::Builder<T>::asField(field);
    return *this;
}


template Object &Object::addField(const std::string &, std::string &field);
template Object &Object::addField(const std::string &, int &field);
template Object &Object::addField(const std::string &, long long &field);
template Object &Object::addField(const std::string &, boost::posix_time::ptime &field);
template Object &Object::addField(const std::string &, Object &field);
template Object &Object::addField(const std::string &, std::vector<int> &field);


template<typename T> void toValue(Wt::Json::Value &v, void *p) {
    v = Value<T>(p);
}


Wt::Json::Object Object::toWtObject() const {
    typedef function<void(Wt::Json::Value &, void *)> Exporter;
    static map<Field::Type, Exporter> exporters {
        {Field::String, toValue<std::string> },
        {Field::Int, toValue<int> },
        {Field::LongLong, toValue<long long> },
        {Field::DateTime, toValue<boost::posix_time::ptime> },
        {Field::Object, toValue<Object> },
        {Field::Vector, [](Wt::Json::Value &v, void *p){
                Wt::Json::Value _v(Wt::Json::ArrayType);
                Wt::Json::Array &a = _v;
                std::vector<int> _vector = Value<std::vector<int>>{p};
                std::copy(begin(_vector), end(_vector), back_inserter(a));
                v = _v;
            } },
    };

    Wt::Json::Object wtObject;
    for(auto v: fields) {
        Wt::Json::Value value;
        exporters[v.second.type](value, v.second.p);
        wtObject[v.first] = value;
    }
    return wtObject;
}


void Object::fromJson(const std::string &jsonString) {
    Wt::Json::Object wtObject;
    Wt::Json::parse(jsonString, wtObject);
    from(wtObject);
}

void Object::from(const Wt::Json::Object &object) {
    typedef function<void(Wt::Json::Value &, void *)> Importer;

    static map<Field::Type, Importer> importers {
        {Field::String, [](Wt::Json::Value &v, void *p) { Value<string>(p).set(v); } },
        {Field::Int, [](Wt::Json::Value &v, void *p) { Value<int>(p).set(v); } },
        {Field::LongLong, [](Wt::Json::Value &v, void *p) { Value<long long>(p).set(v); } },
        {Field::DateTime, [](Wt::Json::Value &v, void *p) { Value<boost::posix_time::ptime>(p).set(boost::posix_time::from_iso_string(v)); } },
        {Field::Object, [](Wt::Json::Value &v, void *p) { Object &object = Value<Object>(p); object.from(v); } },
    };

    for(auto field: fields) {
        auto value = object.at(field.first);
        importers[field.second.type](value, field.second.p);
    }
}

} // namespace Json
} // namespace WtCommons
