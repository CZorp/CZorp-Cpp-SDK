# CZorp-Cpp-SDK
C++ SDK v1.0.04 for [CZorp RESTful API v1](https://czorp.com/doc)

# 3rd party libraries
This SDK uses following 3rd party libraries
- [OpenSSL](https://www.openssl.org/) (tested for OpenSSL/1.1.0g)
- [cURL](https://curl.haxx.se/) (tested for libcurl/7.58.0)
- json (source code included)

# Implementation hints
BEFORE USING THIS SDK you must set the credentials `API_ID`, `API_SECRET` and a filepath `TOKEN_FILEPATH` for saving sessions/tokens in `czorp.cpp`.

Any function, excluding `CZORP_init` and `CZORP_cleanup`, shall be considered thread safe. Error handling for implementations shall be split into two, once in checking for soft errors (functions return value evaluates to true) and once in catching hard errors via try catch exception mechanism (these errors shall usually not be recoverable).

# Compiling
You will need a c++17 compatible compiler and you will need to link to the 3rd party libraries
```
-lssl -lcrypto -lcurl -lpthread (-lstdc++fs)
```

In particular the c++17 filesystem library is in use, which at the time
this SDK was written was in experimental mode. Therefore if your
system's filesystem library is in experimental mode you may need to link
with `-lstdc++fs`.

# Example
A demo application is included in `main.cpp`, which can be used for testing using your credentials `APP_ID` and `APP_SECRET`.

# SDK Components
## Macros
### CZORP_VERSION
Is a combination of the major, minor and patch SDK version in use, into a single integer `MNNNPPP`, where `M` is the single digit major, `NNN` the three digit minor and `PPP` the three digit patch version number.
```cpp
#define CZORP_VERSION
```
## Functions
### CZORP_init
Initializes CZorp and 3rd party libraries within CZorp. Must be invoked exactly once before any other API call and further API calls shall not be made if this function returns an error.
+ **return** On success evaluates to false, on error evaluates to true and a human readable error message can be accessed via `value()`.
```cpp
std::optional<std::string> CZORP_init();
```
### CZORP_cleanup
Invokes 3rd party libraries cleanup functions and saves the optional valid CZorp session to file.
Must be invoked exactly once after any other API call.
```cpp
void CZORP_cleanup();
```
### CZORP_signup
Performs CZORP [SIGNUP](https://czorp.com/doc#signup) API call.
+ **email** The recipients email address.
+ **return** On success evaluates to false, on error evaluates to true and a human readable error message can be accessed via `value()`.
```cpp
std::optional<std::string> CZORP_signup(std::string const&email);
```
### CZORP_signin
Performs CZORP [SIGNIN](https://czorp.com/doc#signin) API call.
+ **email** The recipients email address.
+ **password** The recipients password.
+ **return** On success evaluates to false, on error evaluates to true and  a human readable error message can be accessed via `value()`.
```cpp
std::optional<std::string> CZORP_signin(std::string const&email, std::string const&password);
```
### CZORP_token
Performs CZORP [TOKEN](https://czorp.com/doc#token) API call.
+ **return** On success evaluates to false, on error evaluates to true and a human readable error message can be accessed via `value()`.
```cpp
std::optional<std::string> CZORP_token();
```
### CZORP_password_reset
Performs CZORP [PASSWORD-RESET](https://czorp.com/doc#password-reset) API call.
+ **email** The recipients email address.
+ **return** On success evaluates to false, on error evaluates to true and a human readable error message can be accessed via `value()`.
```cpp
std::optional<std::string> CZORP_password_reset(std::string const&email);
```
### CZORP_signout
Performs CZORP [SIGNOUT](https://czorp.com/doc#signout) API call.
+ **return** On success evaluates to false, on error evaluates to true and a human readable error message can be accessed via `value()`.
```cpp
std::optional<std::string> CZORP_signout();
```

# TERMS AND CONDITIONS
THIS SDK, EXCLUDING THE 3RD PARTY LIBRARIES OPENSSL AND CURL IN USE,
SHALL BE INTERPRETED AS PART OF CZORP'S SERVICE AND IS THEREFORE PUBLISHED
AND LICENSED UNDER CZORP'S TERMS AND CONDITIONS AND LICENSE WHICH SHALL BE
FOUND AT https://zcorp/terms-and-conditions.

© 2019 [czorp.com](https://czorp.com)
