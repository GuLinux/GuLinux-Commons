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
    typedef function<void(Wt::Json::Value &, void *)> Exporter;
    static map<Field::Type, Exporter> exporters {
        {Field::String, [](Wt::Json::Value &v, void *p){v = Wt::WString::fromUTF8(Value<string>(p)); } },
        {Field::Int, [](Wt::Json::Value &v, void *p){v = {Value<int>(p)}; } },
        {Field::LongLong, [](Wt::Json::Value &v, void *p){v = {Value<long long>(p)}; } },
        {Field::DateTime, [](Wt::Json::Value &v, void *p){v = {boost::posix_time::to_iso_string(Value<boost::posix_time::ptime>(p))}; } },
        {Field::Object, [](Wt::Json::Value &v, void *p){
                Wt::Json::Value another(Wt::Json::ObjectType);
                Object &t = Value<Object>(p);
                Wt::Json::Object &o = another;
                o = {t.toWtObject()};
                v = another;
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
