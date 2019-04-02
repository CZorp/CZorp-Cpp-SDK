#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include "types.h"

#include <map>
#include <optional>

struct JsonObject : std::map<JsonString,JsonValue>
{
    using std::map<JsonString,JsonValue>::map;
    static JsonObject fromString(std::string_view const&s);
    std::string toString(JsonFormat jsonFormat = JsonFormat::Compact) const;
    JsonValue take(JsonString const&k);
};

#endif // JSONOBJECT_H
