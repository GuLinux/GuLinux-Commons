#ifndef WTCOMMONS_JSON_OBJECT_H
#define WTCOMMONS_JSON_OBJECT_H

#include <memory>
#include <map>
#include <boost/date_time.hpp>
#include <Wt/Json/Value>
#include <Wt/Json/Object>
#include <Wt/WDateTime>

namespace Wt {
namespace Json {
class Object;
}
}

namespace WtCommons {
namespace Json {
template<typename T> using Vector = std::vector<T>;


template<typename T> class Value {
public:
  virtual T& value(void *p) const { return convert_pointer(p); }
  virtual Wt::Json::Value json(void *p) const { Wt::Json::Value v = {value(p)}; return v; }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { convert_pointer(p) = v; }
protected:
  virtual T &convert_pointer(void *p) const { return *reinterpret_cast<T*>(p); }
};



class Object
{
public:
  
  struct Field {
    // TODO: memory cleanup
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
      template<typename T> Wt::Json::Value json() const { return converter<T>()->json(p); }
      template<typename T> void fromJson(const Wt::Json::Value &v) const { return converter<T>()->fromJson(p, v); }
    
    template<typename T> Value<T> *converter() const {
      return reinterpret_cast<Value<T>*>(valueConverter);
    }
  };

    template<typename T, template<typename> class C> Object &addField(const std::string &name, C<T> &f) {
      return push_field(Field::ContainerBuilder<T, C>::asField(f), name, nullptr);
    }

    template<typename T> Object &addField(const std::string &name, T &f, Value<T> *converter = new Value<T>() ) {
      return push_field(Field::Builder<T>::asField(f), name, reinterpret_cast<void*>(converter));
    }
    Object &push_field(Field field, const std::string &name, void *valueConverter) {
      field.label = name;
      field.valueConverter = (valueConverter);
      fields.push_back(field);
      return *this;
    }

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
template<> class Object::Field::ContainerBuilder<int, Vector> {
public:
    static Object::Field asField(std::vector<int> &v) { return {&v, Object::Field::Vector, Object::Field::Int}; }
};


template<> class Value<std::string> {
public:
  virtual std::string& value(void *p) const { return convert_pointer(p); }
  virtual Wt::Json::Value json(void *p) const { Wt::Json::Value v = {Wt::WString::fromUTF8(value(p))}; return v; }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { convert_pointer(p) = v.toString().orIfNull(""); }

protected:
  virtual std::string &convert_pointer(void *p) const { return *reinterpret_cast<std::string*>(p); }
};
template<> class Value<Object> {
public:
  virtual Object& value(void *p) const { return convert_pointer(p); }
  virtual Wt::Json::Value json(void *p) const { 
    Wt::Json::Value v{Wt::Json::ObjectType};
    Wt::Json::Object &o = v;
    o = value(p).toWtObject();
    return v;
  }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { convert_pointer(p).from(v); }
protected:
  virtual Object &convert_pointer(void *p) const { return *reinterpret_cast<Object*>(p); }
};
template<> class Value<boost::posix_time::ptime> {
public:
  virtual boost::posix_time::ptime& value(void *p) const { return convert_pointer(p); }
  virtual Wt::Json::Value json(void *p) const { return {static_cast<long long>(Wt::WDateTime::fromPosixTime(value(p)).toTime_t())}; }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { long long timeT = v; convert_pointer(p) = boost::posix_time::from_time_t(timeT); }

protected:
  virtual boost::posix_time::ptime &convert_pointer(void *p) const { return *reinterpret_cast<boost::posix_time::ptime*>(p); }
};

class PosixTimeValue : public Value<boost::posix_time::ptime> {
public:
  virtual Wt::Json::Value json(void *p) const { return {boost::posix_time::to_iso_string(value(p))}; }
  virtual void fromJson(void *p, const Wt::Json::Value &v) const { convert_pointer(p) = boost::posix_time::from_iso_string(v.toString().orIfNull("")); }
};

} // namespace Json
} // namespace WtCommons

#endif // WTCOMMONS_JSON_OBJECT_H
