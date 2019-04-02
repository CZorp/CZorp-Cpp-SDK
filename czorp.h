/*
 * CZorp-Cpp-SDK 1.0.04 for CZorp API v1
 * © 2019 czorp.com
 *
 * This SDK uses following 3rd party libraries
 *  - OpenSSL (tested for OpenSSL/1.1.0g)
 *  - cURL (tested for libcurl/7.58.0)
 *  - json (source code included)
 *
 * BEFORE USING THIS SDK you must set the credentials(API_ID, API_SECRET)
 * and a filepath(TOKEN_FILEPATH) for saving sessions/tokens in the czorp.cpp
 * file.
 *
 * IMPLEMENTATION HINTS
 *   Any function, excluding CZORP_init and CZORP_cleanup, shall be considered
 *   thread safe.
 *   Error handling for implementations shall be split into two, once in
 *   checking for soft errors (functions return value evaluates to true) and
 *   once in catching hard errors via try catch exception mechanism (these
 *   errors shall usually not be recoverable).
 *
 * COMPILING
 *   In order to use this SDK you will need a c++17 compatible compiler and you
 *   will need to link to the 3rd party libraries
 *    -lssl -lcrypto -lcurl -lpthread (-lstdc++fs)
 *
 *   In particular the c++17 filesystem library is in use, which at the time
 *   this SDK was written was in experimental mode. Therefore if your
 *   system's filesystem library is in experimental mode you may need to link
 *   with -lstdc++fs.
 *
 * TERMS AND CONDITIONS
 *   THIS LIBRARY, EXCLUDING THE 3RD PARTY LIBRARIES OPENSSL AND CURL IN USE,
 *   SHALL BE INTERPRETED AS PART OF CZORP'S SERVICE AND IS THEREFORE PUBLISHED
 *   AND LICENSED UNDER CZORP'S TERMS AND CONDITIONS AND LICENSE WHICH SHALL BE
 *   FOUND AT https://zcorp/terms-and-conditions.
*/
#ifndef CZORP_H
#define CZORP_H

//c++17
#include <optional>

#include <string>

#define CZORP_VERSION 1000004




/*
 * CZORP_init:
 *     Initializes CZorp and 3rd party libraries within CZorp.
 *     Must be invoked exactly once before any other API call and
 *     further API calls shall not be made if this function returns an error.
 * return:
 *     On success evaluates to false, on error evaluates to true and
 *     a human readable error message can be accessed via value().
 */
std::optional<std::string> CZORP_init();




/*
 * CZORP_cleanup:
 *     Performs a library cleanup.
 *     Must be invoked exactly once after any other API call.
 */
void CZORP_cleanup();




/*
 * CZORP_signup:
 *     Performs CZORP SIGNUP API call, see (czorp.com/doc for more).
 * email:
 *     The recipients email address.
 * return:
 *     On success evaluates to false, on error evaluates to true and
 *     a human readable error message can be accessed via value().
 */
std::optional<std::string> CZORP_signup(std::string const&email);




/*
 * CZORP_signin:
 *     Performs CZORP SIGNIN API call, see (czorp.com/doc for more).
 * email:
 *     The recipients email address.
 * password:
 *     The recipients password.
 * return:
 *     On success evaluates to false, on error evaluates to true and
 *     a human readable error message can be accessed via value().
 */
std::optional<std::string> CZORP_signin(std::string const&email,
                                        std::string const&password);




/*
 * CZORP_token:
 *     Performs CZORP TOKEN API call, see (czorp.com/doc for more).
 * return:
 *     On success evaluates to false, on error evaluates to true and
 *     a human readable error message can be accessed via value().
 */
std::optional<std::string> CZORP_token();




/*
 * CZORP_password_reset:
 *     Performs CZORP PASSWORD-RESET API call, see (czorp.com/doc for more).
 * email:
 *     The recipients email address.
 * return:
 *     On success evaluates to false, on error evaluates to true and
 *     a human readable error message can be accessed via value().
 */
std::optional<std::string> CZORP_password_reset(std::string const&email);




/*
 * CZORP_signout:
 *     Performs CZORP SIGNOUT API call, see (czorp.com/doc for more).
 * return:
 *     On success evaluates to false, on error evaluates to true and
 *     a human readable error message can be accessed via value().
 */
std::optional<std::string> CZORP_signout();




#endif // CZORP_H
