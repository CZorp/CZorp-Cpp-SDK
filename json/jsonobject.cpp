#include "jsonobject.h"

#include "jsonvalue.h"
#include "jsonarray.h"

#include <sstream>

JsonObject JsonObject::fromString(std::string_view const&s)
{
    if(auto ret = JsonValue::fromString(s))
    {
        if(auto pval = std::get_if<JsonObject>(&ret.value()))
            return *pval;
        else
            throw std::runtime_error("JSON error: fromString error: "
                                     "String is not a JsonObject");
    }
    else
        throw std::runtime_error("JSON error: fromString error: "
                                 "Syntax error inside string");
}
std::string JsonObject::toString(JsonFormat jsonFormat) const
{
    if(size() == 0)
        return "{}";
    else if(jsonFormat == JsonFormat::Beautify)
    {
        std::ostringstream oss;
        auto citer = cbegin();
        oss << "\"" << citer->first << "\":"
            << citer->second.toString(JsonFormat::Beautify);
        ++citer;
        for(;citer != cend(); ++citer)
            oss << ",\n\"" << citer->first << "\":"
                << citer->second.toString(JsonFormat::Beautify);
        return "{\n" + oss.str() + "\n}";
    }
    else if(jsonFormat == JsonFormat::Compact)
    {
        std::ostringstream oss;
        auto citer = cbegin();
        oss << "\"" << citer->first << "\":"
            << citer->second.toString(JsonFormat::Compact);
        ++citer;
        for(;citer != cend(); ++citer)
            oss << ",\"" << citer->first << "\":"
                << citer->second.toString(JsonFormat::Compact);
        return "{" + oss.str() + "}";
    }
    return std::string();
}
JsonValue JsonObject::take(JsonString const&k){
    JsonValue v = std::move(at(k));
    erase(k);
    return v;
}
