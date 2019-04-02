#include "czorp.h"

#include <chrono>
#include <sstream>
#include <fstream>
#include <cstring>
#include <iomanip>

//c++17
#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

//3rd party
#include <curl/curl.h>
#include <openssl/hmac.h>
#include "json/json.h"


//SET THESE VALUES
constexpr auto APP_ID ="";
constexpr auto APP_SECRET ="";
constexpr auto TOKEN_FILEPATH = "token.txt";
constexpr auto CZORP_V1_URL = "https://api.czorp.com/v1";

//DO NOT CHANGE THESE VALUES
#include <mutex>
#include <shared_mutex>
std::shared_mutex m_token_sm;
std::string m_token = "";
std::string token(){
    std::shared_lock lock(m_token_sm);
    return m_token;
}
std::string token(std::string const&v){
    std::scoped_lock lock(m_token_sm);
    return m_token = v;
}

//HELPER FUNCTIONS
std::string httpDate(){
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::gmtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S %Z");
    return oss.str();
}
std::string base64Encode(std::string const&str){
    constexpr auto charSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                             "abcdefghijklmnopqrstuvwxyz"
                             "0123456789+/";
    unsigned char const*p = reinterpret_cast<unsigned char const*>(str.data());
    unsigned char const*const pEnd = p+str.size()-str.size()%3;
    std::string r((str.size() + 2)/3 * 4,'=');
    char *rPtr = r.data();

    while(p != pEnd)
    {
        uint32_t const n = *p << 16 | *(p+1) << 8 | *(p+2);
        p += 3;
        *rPtr++ = charSet[n >> 18         ];
        *rPtr++ = charSet[n >> 12   & 0x3F];
        *rPtr++ = charSet[n >> 6    & 0x3F];
        *rPtr++ = charSet[n         & 0x3F];
    }
    switch (str.size()%3) {
    case 1:
        *rPtr++ = charSet[*p >> 2         ];
        *rPtr++ = charSet[*p << 4   & 0x3F];
        break;
    case 2:
    {
        uint16_t n = *p << 8 | *(p+1);
        *rPtr++ = charSet[n >> 10         ];
        *rPtr++ = charSet[n >> 4    & 0x3F];
        *rPtr++ = charSet[n << 2    & 0x3F];
        break;
    }
    default:
        break;
    }
    return r;
}
size_t cb(char *p, size_t s, size_t n, void *u){
    static_cast<std::string*>(u)->append(p,s*n);
    return s*n;
}
std::optional<std::string> POST(std::string const&path, JsonObject const&obj,
                                std::string &response){
    std::unique_ptr<CURL,decltype(&curl_easy_cleanup)> curl(curl_easy_init(),
                                                            curl_easy_cleanup);
    std::unique_ptr<curl_slist,decltype(&curl_slist_free_all)>
            slist(curl_slist_append(nullptr,"Content-Type: application/json"),
                  curl_slist_free_all);
    if(!curl)
        return "cURL error: curl_easy_init failed";
    std::string url = CZORP_V1_URL + path;
    std::string obj_serialized = obj.toString();
    curl_easy_setopt(curl.get(),CURLOPT_URL,url.data());
    curl_easy_setopt(curl.get(),CURLOPT_HTTPHEADER,slist.get());
    curl_easy_setopt(curl.get(),CURLOPT_POSTFIELDS,obj_serialized.data());
    curl_easy_setopt(curl.get(),CURLOPT_WRITEFUNCTION,cb);
    curl_easy_setopt(curl.get(),CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl.get(),CURLOPT_WRITEDATA,
                     static_cast<void*>(&response));

    CURLcode res = curl_easy_perform(curl.get());
    if(res != CURLE_OK)
        return std::string("cURL error: curl_easy_perform error: ")
                +curl_easy_strerror(res);
    return std::nullopt;
}
size_t statusCode(std::string const&response){
    std::string bluff;
    size_t ret;
    std::istringstream iss(response.substr(0,50));
    iss >> bluff >> ret;
    return ret;
}
std::optional<std::string> HMAC_SHA256_sign(std::string const&data,
                                            std::string const&key,
                                            std::string &signature)
{
    signature.resize(EVP_MAX_MD_SIZE);
    unsigned int signature_len(signature.size());
    if(HMAC(EVP_sha256(),
            reinterpret_cast<void const*>(key.data()),key.size(),
            reinterpret_cast<unsigned char const*>(data.data()),data.size(),
            reinterpret_cast<unsigned char*>(signature.data()),&signature_len)
            == nullptr)
        return "OpenSSL error: HMAC signing failed";
    signature.resize(signature_len);
    return std::nullopt;
}
std::optional<std::string> verify(std::string const&data, std::string const&key,
                                  std::string const&signature)
{
    std::string calculated_signature;
    if(auto v = HMAC_SHA256_sign(data,key,calculated_signature))
        return v;
    else if(base64Encode(calculated_signature) != signature)
        return "Response corrupted. Signature DID NOT VERIFY.";
    return std::nullopt;
}
std::optional<std::string> sign(JsonObject const&o, std::string &signature){
    if(auto v = HMAC_SHA256_sign(o.toString(),APP_SECRET,signature))
        return v;
    signature = base64Encode(signature);
    return std::nullopt;
}
JsonObject responseBody(std::string const&response){
    return JsonObject::fromString(response.substr(response.find("\r\n\r\n")+4));
}
//CZORP functions
std::optional<std::string> CZORP_init()
{
    //cURL init
    curl_global_init(CURL_GLOBAL_ALL);
    //check if APP_ID,APP_SECRET,TOKEN_FILEPATH set inside cpp file
    if(!std::strlen(APP_ID))
        return "APP_ID empty. Please set APP_ID in "
                + std::string(__FILE__)+".";
    else if(!std::strlen(APP_SECRET))
        return "APP_SECRET empty. Please set APP_SECRET in "
                + std::string(__FILE__)+".";
    else if(!std::strlen(TOKEN_FILEPATH))
        return "TOKEN_FILEPATH empty. Please set TOKEN_FILEPATH in "
                + std::string(__FILE__)+".";
    //read token from file
    if(fs::exists(TOKEN_FILEPATH)){
        std::string tok;
        std::ifstream fis(TOKEN_FILEPATH);
        fis >> tok;
        token(tok);
    }
    return std::nullopt;
}
void CZORP_cleanup()
{
    //cURL cleanup
    curl_global_cleanup();
    //write token to file
    if(!token().empty()){
        std::string tok = token();
        std::ofstream fos(TOKEN_FILEPATH);
        fos << tok;
    }
}
std::optional<std::string> CZORP_signup(std::string const&email)
{
    //build request
    JsonObject request{
        {"app_id",APP_ID},
        {"date",httpDate()},
        {"email",email}
    };
    //sign request
    std::string request_signature;
    if(auto v = sign(request,request_signature))
        return v;
    request.emplace("signature",request_signature);
    //perform blocking POST and get response
    std::string response;
    if(auto v = POST("/signup",request,response))
        return v;
    //parse response body
    JsonObject response_body = responseBody(response);
    //return error_string if error
    if(statusCode(response) != 200)
        return response_body.at("error_string");
    //verify response
    std::string response_signature = response_body.take("signature");
    if(auto v = verify(response_body.toString(),APP_SECRET,response_signature))
        return v;
    return std::nullopt;
}
std::optional<std::string> CZORP_signin(std::string const&email,
                                        std::string const&password)
{
    //build request
    JsonObject request{
        {"app_id",APP_ID},
        {"date",httpDate()},
        {"email",email},
        {"password",base64Encode(password)}
    };
    //sign request
    std::string request_signature;
    if(auto v = sign(request,request_signature))
        return v;
    request.emplace("signature",request_signature);
    //perform blocking POST and get response
    std::string response;
    if(auto v = POST("/signin",request,response))
        return v;
    //parse response body
    JsonObject response_body = responseBody(response);
    //return error_string if error
    if(statusCode(response) != 200)
        return response_body.at("error_string");
    //verify response
    std::string response_signature = response_body.take("signature");
    if(auto v = verify(response_body.toString(),APP_SECRET,response_signature))
        return v;
    //process response
    token(response_body.at("token"));
    return std::nullopt;
}
std::optional<std::string> CZORP_token()
{
    if(token().empty())
        return "Can not perform CZORP_token without a valid session";
    //build request
    JsonObject request{
        {"app_id",APP_ID},
        {"date",httpDate()},
        {"token",token()}
    };
    //sign request
    std::string request_signature;
    if(auto v = sign(request,request_signature))
        return v;
    request.emplace("signature",request_signature);
    //perform blocking POST and get response
    std::string response;
    if(auto v = POST("/token",request,response))
        return v;
    //parse response body
    JsonObject response_body = responseBody(response);
    //return error_string if error
    if(statusCode(response) != 200)
        return response_body.at("error_string");
    //verify response
    std::string response_signature = response_body.take("signature");
    if(auto v = verify(response_body.toString(),APP_SECRET,response_signature))
        return v;
    token(response_body.at("token"));
    return std::nullopt;
}
std::optional<std::string> CZORP_password_reset(std::string const&email)
{
    //build request
    JsonObject request{
        {"app_id",APP_ID},
        {"date",httpDate()},
        {"email",email},
    };
    //sign request
    std::string request_signature;
    if(auto v = sign(request,request_signature))
        return v;
    request.emplace("signature",request_signature);
    //perform blocking POST and get response
    std::string response;
    if(auto v = POST("/password-reset",request,response))
        return v;
    //parse response body
    JsonObject response_body = responseBody(response);
    //return error_string if error
    if(statusCode(response) != 200)
        return response_body.at("error_string");
    //verify response
    std::string response_signature = response_body.take("signature");
    if(auto v = verify(response_body.toString(),APP_SECRET,response_signature))
        return v;
    return std::nullopt;
}
std::optional<std::string> CZORP_signout()
{
    if(token().empty())
        return "Can not perform CZORP_signout without a valid session";
    //build request
    JsonObject request{
        {"app_id",APP_ID},
        {"date",httpDate()},
        {"token",token()}
    };
    //sign request
    std::string request_signature;
    if(auto v = sign(request,request_signature))
        return v;
    request.emplace("signature",request_signature);
    //perform blocking POST and get response
    std::string response;
    if(auto v = POST("/signout",request,response))
        return v;
    //parse response body
    JsonObject response_body = responseBody(response);
    //return error_string if error
    if(statusCode(response) != 200)
        return response_body.at("error_string");
    //verify response
    std::string response_signature = response_body.take("signature");
    if(auto v = verify(response_body.toString(),APP_SECRET,response_signature))
        return v;
    //clear m_token
    token("");
    //erase file at m_token_filepath
    fs::remove(TOKEN_FILEPATH);
    return std::nullopt;
}
