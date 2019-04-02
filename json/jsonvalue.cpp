#include "jsonvalue.h"

#include <sstream>
#include <functional>
#include <iomanip>
#include <string_view>
#include <cmath>
#include <cstring>

#include <codecvt>

std::string jsonString_escape(std::string const&arg)
{
    std::string ret(1,'\"');
    ret.reserve(arg.size()+2);
    for(size_t i = 0; i < arg.size(); ++i){
        switch(static_cast<unsigned char>(arg[i])) {
        case 0x00: ret += "\\u0000"; break;
        case 0x01: ret += "\\u0001"; break;
        case 0x02: ret += "\\u0002"; break;
        case 0x03: ret += "\\u0003"; break;
        case 0x04: ret += "\\u0004"; break;
        case 0x05: ret += "\\u0005"; break;
        case 0x06: ret += "\\u0006"; break;
        case 0x07: ret += "\\u0007"; break;
        case 0x08: ret += "\\b"; break;
        case 0x09: ret += "\\t"; break;
        case 0x0A: ret += "\\n"; break;
        case 0x0B: ret += "\\u000B"; break;
        case 0x0C: ret += "\\f"; break;
        case 0x0D: ret += "\\r"; break;
        case 0x0E: ret += "\\u000E"; break;
        case 0x0F: ret += "\\u000F"; break;
        case 0x10: ret += "\\u0010"; break;
        case 0x11: ret += "\\u0011"; break;
        case 0x12: ret += "\\u0012"; break;
        case 0x13: ret += "\\u0013"; break;
        case 0x14: ret += "\\u0014"; break;
        case 0x15: ret += "\\u0015"; break;
        case 0x16: ret += "\\u0016"; break;
        case 0x17: ret += "\\u0017"; break;
        case 0x18: ret += "\\u0018"; break;
        case 0x19: ret += "\\u0019"; break;
        case 0x1A: ret += "\\u001A"; break;
        case 0x1B: ret += "\\u001B"; break;
        case 0x1C: ret += "\\u001C"; break;
        case 0x1D: ret += "\\u001D"; break;
        case 0x1E: ret += "\\u001E"; break;
        case 0x1F: ret += "\\u001F"; break;
        case 0x22: ret += "\\\""; break;
        case 0x5C: ret += "\\\\"; break;
        default: ret += arg[i]; break;
        }
    }
    ret += '\"';
    return ret;
}

std::string jsonString_unescape(std::string const&arg)
{
    std::string ret;
    ret.reserve(arg.size());
    for(size_t i = 0; i < arg.size(); ++i){
        if(arg[i] == '\\')
        {
            ++i;
            if(arg.at(i) == 'u'){
                std::string str{arg.at(i+1),arg.at(i+2),arg.at(i+3),
                            arg.at(i+4)};
                i+=4;
                char16_t hex_val;
                std::istringstream iss(str);
                iss >> std::hex >> reinterpret_cast<uint16_t &>(hex_val);
                std::wstring_convert<
                        std::codecvt_utf8_utf16<char16_t>, char16_t> cv16;
                ret += cv16.to_bytes(hex_val);
            }
            else{
                switch(arg[i]) {
                case '\"': ret += '\"'; break;
                case '\\': ret += '\\'; break;
                case '/': ret += '/'; break;
                case 'b': ret += '\b'; break;
                case 'f': ret += '\f'; break;
                case 'n': ret += '\n'; break;
                case 'r': ret += '\r'; break;
                case 't': ret += '\t'; break;
                default:
                    break;
                }
            }
        }
        else
            ret += arg[i];
    }
    ret.shrink_to_fit();
    return ret;
}


struct StringView : std::string_view
{
    using std::string_view::npos;
    using std::string_view::string_view;
    using std::string_view::const_iterator;
    void remove_prefix_spaces()
    {
        size_t __p = find_first_not_of(" \f\n\r\t\v");
        return remove_prefix(__p != StringView::npos ? __p : size());
    }
    void remove_suffix_spaces()
    {
        size_t __p = find_last_not_of(" \f\n\r\t\v");
        return remove_suffix(__p != StringView::npos ? size()-(__p+1) : size());
    }
    bool starts_with(char const*opening)
    {
        return (substr(0,std::min(std::strlen(opening),size())) == opening);
    }
    bool pass_if_starts_with(char const*opening)
    {
        if(starts_with(opening))
        {
            remove_prefix(std::strlen(opening));
            return true;
        }
        else
            return false;
    }
    bool starts_with(char opening)
    {
        if(empty())
            return false;
        else
            return (front() == opening);
    }
    bool pass_if_starts_with(char opening)
    {
        if(starts_with(opening))
        {
            remove_prefix(1);
            return true;
        }
        else
            return false;
    }
    std::optional<JsonNull> passJsonNull()
    {
        if(pass_if_starts_with("null"))
            return nullptr;
        else
            return std::nullopt;
    }
    std::optional<JsonBool> passJsonBool()
    {
        if(pass_if_starts_with("true"))
            return JsonBool(True);
        else if(pass_if_starts_with("false"))
            return JsonBool(False);
        else
            return std::nullopt;
    }
    std::optional<JsonNumber> passJsonNumber()
    {
        char *e;
        JsonNumber num = std::strtod(data(),&e);
        if((num != HUGE_VAL) && (e != data()))
        {
            remove_prefix(e-data());
            return num;
        }
        else
            return std::nullopt;
    }
    std::optional<JsonString> passJsonString()
    {
        if(!starts_with('\"'))
            return std::nullopt;
        //dont allow quotation mark " inside, but escaped quotation marks \"
        for(const_iterator citer = cbegin()+1; citer != cend(); ++citer)
            if(*citer == '\"')
            {
                if(*(citer-1) == '\\')
                    continue;
                JsonString str(cbegin()+1,citer);
                remove_prefix(citer-cbegin()+1);
                return str;
            }
        return std::nullopt;
    }
    std::optional<JsonArray> passJsonArray()
    {
        if(!pass_if_starts_with('['))
            return std::nullopt;
        JsonArray ret;
        remove_prefix_spaces();
        if(pass_if_starts_with(']'))
            return ret;
        while(true)
        {
            remove_prefix_spaces();
            if(auto v = passJsonNull())
                ret.push_back(v.value());
            else if(auto v = passJsonBool())
                ret.push_back(v.value());
            else if(auto v = passJsonNumber())
                ret.push_back(v.value());
            else if(auto v = passJsonString())
                ret.push_back(v.value());
            else if(auto v = passJsonArray())
                ret.push_back(v.value());
            else if(auto v = passJsonObject())
                ret.push_back(v.value());
            else
                return std::nullopt;

            //post process COMMA EOF
            remove_prefix_spaces();
            if(pass_if_starts_with(','))
                continue;
            else if(pass_if_starts_with(']'))
                return ret;
            else
                return std::nullopt;
        }
        return ret;
    }
    std::optional<JsonObject> passJsonObject()
    {
        if(!pass_if_starts_with('{'))
            return std::nullopt;
        JsonObject ret;
        remove_prefix_spaces();
        if(pass_if_starts_with('}'))
            return ret;
        while(true)
        {
            JsonString key;
            remove_prefix_spaces();
            if(auto v = passJsonString())
                key = v.value();
            else
                return std::nullopt;

            remove_prefix_spaces();
            if(pass_if_starts_with(':'))
                ;
            else
                return std::nullopt;

            remove_prefix_spaces();
            if(auto v = passJsonNull())
                ret.emplace(std::move(key),std::move(v.value()));
            else if(auto v = passJsonBool())
                ret.emplace(std::move(key),std::move(v.value()));
            else if(auto v = passJsonNumber())
                ret.emplace(std::move(key),std::move(v.value()));
            else if(auto v = passJsonString())
                ret.emplace(std::move(key),std::move(v.value()));
            else if(auto v = passJsonArray())
                ret.emplace(std::move(key),std::move(v.value()));
            else if(auto v = passJsonObject())
                ret.emplace(std::move(key),std::move(v.value()));
            else
                return std::nullopt;

            //post process COMMA EOF
            remove_prefix_spaces();
            if(pass_if_starts_with(','))
                continue;
            else if(pass_if_starts_with('}'))
                return ret;
            else
                return std::nullopt;
        }
        return ret;
    }
};


std::optional<JsonValue> JsonValue::fromString(std::string_view const&__str)
{
    StringView view(__str.data(),__str.size());

    view.remove_prefix_spaces();
    view.remove_suffix_spaces();
    if(auto v = view.passJsonNull())
    { if(view.empty()) return v.value(); else return std::nullopt;}
    else if(auto v = view.passJsonBool())
    { if(view.empty()) return v.value(); else return std::nullopt;}
    else if(auto v = view.passJsonNumber())
    { if(view.empty()) return v.value(); else return std::nullopt;}
    else if(auto v = view.passJsonString())
    { if(view.empty()) return jsonString_unescape(v.value());
      else return std::nullopt;}
    else if(auto v = view.passJsonArray())
    { if(view.empty()) return v.value(); else return std::nullopt;}
    else if(auto v = view.passJsonObject())
    { if(view.empty()) return v.value(); else return std::nullopt;}
    else
        return std::nullopt;
}
std::string JsonValue::toString(JsonFormat jsonFormat) const
{
    if(auto pval = std::get_if<JsonObject>(this))
        return pval->toString(jsonFormat);
    else if(auto pval = std::get_if<JsonArray>(this))
        return pval->toString(jsonFormat);
    else if(auto pval = std::get_if<JsonString>(this))
        return jsonString_escape(*pval);
    else if(auto pval = std::get_if<JsonNumber>(this))
    {
        std::ostringstream oss;
        oss << std::setprecision(std::numeric_limits<double>::digits10 + 1)
            << std::noshowpoint << *pval;
        return oss.str();
    }
    else if(auto pval = std::get_if<JsonBool>(this))
        return *pval ? "true" : "false";
    else if(std::get_if<JsonNull>(this))
        return "null";
    throw std::runtime_error("JsonValue::toString failed");
    return "";
}
