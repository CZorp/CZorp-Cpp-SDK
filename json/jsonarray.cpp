#include "jsonarray.h"

#include "jsonvalue.h"
#include "jsonobject.h"

#include <sstream>

std::optional<JsonArray> JsonArray::fromString(std::string_view const&str)
{
    if(auto ret = JsonValue::fromString(str))
    {
        if(auto pval = std::get_if<JsonArray>(&ret.value()))
            return *pval;
        else
            return std::nullopt;
    }
    else
        return std::nullopt;
}
std::string JsonArray::toString(JsonFormat jsonFormat) const
{
    if(size() == 0)
        return "[]";
    else if(jsonFormat == JsonFormat::Beautify)
    {
        std::ostringstream oss;
        auto citer = cbegin();
        oss << citer->toString(JsonFormat::Beautify);
        ++citer;
        for(; citer != cend(); ++citer)
            oss << ",\n" << citer->toString(JsonFormat::Beautify);
        return "[\n" + oss.str() + "\n]";
    }
    else if(jsonFormat == JsonFormat::Compact)
    {
        std::ostringstream oss;
        auto citer = cbegin();
        oss << citer->toString(JsonFormat::Compact);
        ++citer;
        for(; citer != cend(); ++citer)
            oss << "," << citer->toString(JsonFormat::Compact);
        return "[" + oss.str() + "]";
    }
    return std::string();
}
