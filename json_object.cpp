#include "json_object.h"
#include <Wt/Json/Object>
#include <Wt/Json/Serializer>
#include <Wt/Json/Parser>
#include <Wt/Json/Array>

using namespace std;

namespace WtCommons {
namespace Json {
template<typename V, template<typename> class C> class Container {
public:
    Container(void *p) : v(*reinterpret_cast<C<V>*>(p)) {}
    operator C<V>&() const { return v;}
    operator Wt::Json::Value() const { return {v}; }
    void set(const Wt::Json::Value &t) { /* TODO */ }
private:
    C<V> &v;
};

string Object::toJson() const {
    return Wt::Json::serialize(toWtObject() );
}

template<typename T> void toValueField(Wt::Json::Value &v, const Object::Field &field) {
    v = field.json<T>();
}

template<typename T> void fromValueField(Wt::Json::Value &v, const Object::Field &field) {
  field.fromJson<T>(v);
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
  {{Object::Field::Object, Object::Field::Null}, {toValueField<Object>, fromValueField<Object>}},
  {{Object::Field::String, Object::Field::Null}, {toValueField<string>, fromValueField<string>}},
  {{Object::Field::Int, Object::Field::Null}, {toValueField<int>, fromValueField<int>}},
  {{Object::Field::LongLong, Object::Field::Null}, {toValueField<long long>, fromValueField<long long>}},
  {{Object::Field::DateTime, Object::Field::Null}, {toValueField<boost::posix_time::ptime>, fromValueField<boost::posix_time::ptime>}},
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
