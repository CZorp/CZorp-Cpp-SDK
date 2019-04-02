#pragma once

#include <string>

struct JsonValue;
struct JsonObject;
struct JsonArray;
typedef std::string JsonString;
typedef double JsonNumber;
enum JsonBool {False=0, True=1};
typedef std::nullptr_t JsonNull;

//Json output string format (default is Compact)
enum class JsonFormat{ Compact,Beautify};
