#ifndef WTCOMMONS_JSON_OBJECT_H
#define WTCOMMONS_JSON_OBJECT_H

#include <memory>
#include <map>
#include <boost/date_time.hpp>
#include <Wt/Json/Value>
#include <Wt/Json/Object>
#include <Wt/Json/Array>
#include <Wt/Json/Serializer>
#include <Wt/WDateTime>

namespace Wt {
namespace Json {
class Object;
}
}

namespace WtCommons {
namespace Json {
template<typename T> using Vector = std::vector<T>;


template<typename T> class ValueBase {
public:
  virtual Wt::Json::Value json(const T &t) const = 0;
  virtual Wt::Json::Value jsonP(void *p) { return json(value(p)); }
  virtual T& value(void *p) const { return convert_pointer(p); }
protected:
  virtual T &convert_pointer(void *p) const { return *reinterpret_cast<T*>(p); }
};

template<typename T> class Value : public ValueBase<T> {
public:
  virtual Wt::Json::Value json(const T &t) const { Wt::Json::Value v = {t}; return v; }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { ValueBase<T>::convert_pointer(p) = v; }
};

class Object
{
public:
  virtual ~Object();
  struct Field {
      enum Types {Null = 0x0, String = 0x1, Int = 0x2, LongLong = 0x3, DateTime = 0x4, Object = 0x10, Vector = 0x20};
      void *p;
      struct Type {
	Types element;
	Types nested;
	bool operator<(const Type &o) const { return element*100+nested < o.element*100 + o.nested; }
      };
      Type type;
      std::string label;
      void *valueConverter;
      template<typename T> struct Builder { static Field asField(T &t); };
      template<typename T, template<typename> class C> struct ContainerBuilder {
	  static Field asField(C<T> &t);
      };
      
      template<typename T> T &value() const { return converter<T>()->value(p); }
      template<typename T> Wt::Json::Value json() const { return converter<T>()->jsonP(p); }
      template<typename T> void fromJson(const Wt::Json::Value &v) const { return converter<T>()->fromJson(p, v); }
    
    template<typename T> Value<T> *converter() const {
      return reinterpret_cast<Value<T>*>(valueConverter);
    }
  };

     template<typename T, template<typename> class C> Object &addField(const std::string &name, C<T> &f) {
     }

    template<typename T> Object &addField(const std::string &name, T &f, Value<T> *converter = new Value<T>() ) {
      auto &type = typeid(*converter);
      static std::map<std::size_t, void *> convertersCache;
      if(convertersCache.count(type.hash_code())) {
	delete converter;
	converter = reinterpret_cast<Value<T>*>(convertersCache.at(type.hash_code()));
      } else {
	convertersCache[type.hash_code()] = converter;
      }
      return push_field(Field::Builder<T>::asField(f), name, reinterpret_cast<void*>(converter));
    }
    Object &push_field(Field field, const std::string &name, void *valueConverter) {
      field.label = name;
      field.valueConverter = valueConverter;
      fields.push_back(field);
      return *this;
    }
    
    virtual void add_to_json(Wt::Json::Object &object) const {}

    std::string toJson() const;
    void fromJson(const std::string &jsonString);
    Wt::Json::Object toWtObject() const;
    void from(const Wt::Json::Object &);
private:
    std::vector<Field> fields;
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


template<> class Value<std::string> : public ValueBase<std::string> {
public:
  virtual Wt::Json::Value json(const std::string &s) const { Wt::Json::Value v = {Wt::WString::fromUTF8(s)}; return v; }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { convert_pointer(p) = v.toString().orIfNull(""); }
};
template<> class Value<Object> : public ValueBase<Object> {
public:
  virtual Wt::Json::Value json(const Object &obj) const { 
    Wt::Json::Value v{Wt::Json::ObjectType};
    Wt::Json::Object &o = v;
    o = obj.toWtObject();
    return v;
  }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { convert_pointer(p).from(v); }
};

template<> class Value<boost::posix_time::ptime> : public ValueBase<boost::posix_time::ptime> {
public:
  virtual Wt::Json::Value json(const boost::posix_time::ptime &p) const { return {static_cast<long long>(Wt::WDateTime::fromPosixTime(p).toTime_t())}; }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { long long timeT = v; convert_pointer(p) = boost::posix_time::from_time_t(timeT); }
};

class PosixTimeValue : public Value<boost::posix_time::ptime> {
public:
  virtual Wt::Json::Value json(const boost::posix_time::ptime &p) const { return {boost::posix_time::to_iso_string(p)}; }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { convert_pointer(p) = boost::posix_time::from_iso_string(v.toString().orIfNull("")); }
};


template<class T, template<typename> class P> 
class PointerObjectConverter : public WtCommons::Json::ValueBase<P<T>> {
public:
    virtual Wt::Json::Value json(const P<T>& t) const {
      return WtCommons::Json::Value<WtCommons::Json::Object>().json(*t);
    }
    virtual void fromJson(void* p, const Wt::Json::Value& v) const {}
};

using ObjectValue = WtCommons::Json::Value<WtCommons::Json::Object>;


template<typename V, template<typename> class C, class Cv = Value<V>>
class Array {
public:
  Array(C<V> &c) : c(c) {}
  Wt::Json::Array toWtArray() const {
    Wt::Json::Array a;
    std::transform(std::begin(c), std::end(c), back_inserter(a), [](V &v){ return Cv().json(v); });
    return a;
  }
  
  Wt::Json::Value toWtValue() const {
    Wt::Json::Value v(Wt::Json::ArrayType);
    Wt::Json::Array &a = v;
    a = toWtArray();
    return v;
  }
  
  std::string toJson() const {
    return Wt::Json::serialize(toWtArray());
  }
  
private:
  C<V> &c;
};

} // namespace Json
} // namespace WtCommons

#endif // WTCOMMONS_JSON_OBJECT_H
