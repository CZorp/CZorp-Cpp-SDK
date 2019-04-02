#ifndef JSONARRAY_H
#define JSONARRAY_H

#include "types.h"

#include <vector>
#include <optional>
#include <string_view>

struct JsonArray : std::vector<JsonValue>
{
    using std::vector<JsonValue>::vector;
    static std::optional<JsonArray> fromString(std::string_view const&str);
    std::string toString(JsonFormat jsonFormat = JsonFormat::Compact) const;
};

#endif // JSONARRAY_H
