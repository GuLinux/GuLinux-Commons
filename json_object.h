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

template<typename T> class FieldBuilder;
class Object
{
    struct Field {
        enum Type {String, Int, LongLong, DateTime, Object, Vector, Null};
        void *p;
        Type type;
        Type arrayType;
        template<typename T> struct Builder {
            static Field build(T &t);
        };
    };

public:
    template<typename T>
    Object &addField(const std::string &name, T &field);

    std::string toJson() const;
    void fromJson(const std::string &jsonString);
    Wt::Json::Object toWtObject() const;
private:
    std::map<std::string, Field> fields;
    template<typename T> friend class FieldBuilder;
    void from(const Wt::Json::Object &);
};

} // namespace Json
} // namespace WtCommons

#endif // WTCOMMONS_JSON_OBJECT_H
