#include "json_object.h"
#include <Wt/Json/Object>
#include <Wt/Json/Serializer>
#include <Wt/Json/Parser>

using namespace std;

namespace WtCommons {
namespace Json {
template<typename T> class Value {
public:
    Value(void *p) : v(*reinterpret_cast<T*>(p)) {}
    operator T&() const { return v;}
    void set(const T &t) {
        v = t;
    }

private:
    T &v;
};

string Object::toJson() const {
    return Wt::Json::serialize(toWtObject() );
}

Wt::Json::Object Object::toWtObject() const {
    Wt::Json::Object wtObject;
    for(auto v: fields) {
        Wt::Json::Value value;
        switch(v.second.type) {
        case Field::String:
            //value = {*reinterpret_cast<string*>(v.second.p)};
            value = {Wt::WString::fromUTF8(Value<string>(v.second.p))};
            break;
        case Field::Int:
            value = {Value<int>(v.second.p)};
            break;
        case Field::LongLong:
            value = {Value<long long>(v.second.p)};
            break;
        case Field::DateTime: // TODO: Parameters for format choosing
            value = {boost::posix_time::to_iso_string(Value<boost::posix_time::ptime>(v.second.p))};
            break;
        case Field::Object:
            Wt::Json::Value another(Wt::Json::ObjectType);
            Object &t = Value<Object>(v.second.p);
            Wt::Json::Object &o = another;
            o = {t.toWtObject()};
            value = another;
            break;
        }

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
    for(auto field: fields) {
        auto value = object.at(field.first);
        switch(field.second.type) {
        case Field::String:
            Value<string>(field.second.p).set(value);
            break;
        case Field::Int:
            Value<int>(field.second.p).set(value);
            break;
        case Field::LongLong:
            Value<long long>(field.second.p).set(value);
            break;
        case Field::DateTime:
            Value<boost::posix_time::ptime>(field.second.p).set(boost::posix_time::from_iso_string(value));
            break;
        case Field::Object:
            Object &object = Value<Object>(field.second.p);
            object.from(value);
        }
    }
}

} // namespace Json
} // namespace WtCommons
