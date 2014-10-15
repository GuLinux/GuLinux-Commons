#ifndef WTCOMMONS_JSON_OBJECT_H
#define WTCOMMONS_JSON_OBJECT_H

#include <memory>
#include <map>
#include <boost/date_time.hpp>

namespace Wt {
namespace Json {
class Value;
class Object;
}
}

namespace WtCommons {
namespace Json {
template<typename T> using Vector = std::vector<T>;
template<class T> class default_create {
public:
  operator T() const { return T{}; }
};
class Object
{
public:
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
        template<typename T> struct Builder {
            static Field asField(T &t);
        };
        template<typename T, template<typename> class C> struct ContainerBuilder {
            static Field asField(C<T> &t);
        };
    };

    template<typename T, template<typename> class C, typename Creator = default_create<T>> Object &addField(const std::string &name, C<T> &f, Creator creator = {}) {
      return push_field(Field::ContainerBuilder<T, C>::asField(f), name);
    }
    template<typename T, typename Creator = default_create<T>> Object &addField(const std::string &name, T &f, Creator creator = {}) {
      return push_field(Field::Builder<T>::asField(f), name);
    }
    Object &push_field(Field field, const std::string &name) {
      field.label = name;
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


} // namespace Json
} // namespace WtCommons

#endif // WTCOMMONS_JSON_OBJECT_H
