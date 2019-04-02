#ifndef JSONVALUE_H
#define JSONVALUE_H

#include "types.h"
#include "jsonobject.h"
#include "jsonarray.h"

#include <variant>
#include <optional>

struct JsonValue : std::variant<JsonObject,JsonArray,JsonString,JsonNumber,
        JsonBool,JsonNull>
{
private:
    template<typename _T, typename ...Args>
    static constexpr bool is_one_of = (std::is_same<_T,Args>::value || ...);
public:
    using std::variant<JsonObject,JsonArray,JsonString,JsonNumber,
    JsonBool,JsonNull>::variant;
    static std::optional<JsonValue> fromString(std::string_view const&str);
    std::string toString(JsonFormat jsonFormat = JsonFormat::Compact) const;

    template<typename _T, typename std::enable_if_t<is_one_of<_T,JsonObject,
      JsonArray,JsonString,JsonNumber,JsonBool,JsonNull>>* = nullptr>
    operator _T() const {
        return std::get<_T>(*this);
    }
    template<typename _T, typename std::enable_if_t<is_one_of<_T,JsonObject,
      JsonArray,JsonString,JsonNumber,JsonBool,JsonNull>>* = nullptr>
    operator _T&() {
        return std::get<_T>(*this);
    }

    bool operator ==(char const*o) const{
        return std::get<JsonString>(*this) == o;
    }
    bool operator !=(char const*o) const{
        return std::get<JsonString>(*this) != o;
    }
    bool operator !=(JsonString const&o) const{
        return std::get<JsonString>(*this) != o;
    }

    JsonValue &at(JsonString const&k){
        return std::get<JsonObject>(*this).at(k);
    }
    JsonValue at(JsonString const&k) const{
        return std::get<JsonObject>(*this).at(k);
    }
    JsonValue &at(size_t const&k){
        return std::get<JsonArray>(*this).at(k);
    }
    JsonValue at(size_t const&k) const{
        return std::get<JsonArray>(*this).at(k);
    }
};

#endif // JSONVALUE_H
