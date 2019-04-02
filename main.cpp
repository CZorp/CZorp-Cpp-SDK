/*
 * CZorp Demo Application - "Password Generator"
 * © 2019 czorp.com
 *
 * BEFORE RUNNING THIS EXAMPLE you must set the credentials(API_ID, API_SECRET)
 * and a filepath(TOKEN_FILEPATH) for saving sessions/tokens in the czorp.cpp
 * file.
 *
 * Compile with
 * g++ -std=c++17 main.cpp czorp.cpp json/jsonarray.cpp json/jsonobject.cpp json/jsonvalue.cpp -lpthread -lssl -lcrypto -lcurl -lstdc++fs
 *
 * Run with
 * ./a.out
*/
#include <iostream>

#include "czorp.h"
#include <thread>

using namespace std;

//simple demo application which generates and prints a password
void passwordGenerator()
{
    srand(time(NULL));
    string pwd(16,0);
    for(auto &c : pwd)
      c = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
          "abcdefghijklmnopqrstuvwxyz"
          "0123456789"[rand()%62];
    cout << "Your generated password: " << pwd << endl;
}

int main()
{
    //init czorp
    if(auto error_msg = CZORP_init()){
        cerr << error_msg.value() << endl;
        return 0;
    }

    //check if valid previous session
    bool is_signed_in = !CZORP_token();

    //main loop
    while(true)
    {
        if(!is_signed_in)//check if signed in
        {//user not signed in branch
            cout << "Type\n"
                 << "    signin           sign in\n"
                 << "    signup           sign up\n"
                 << "    password-reset   reset your password\n"
                 << "    quit             to quit"
                 << endl;

        }
        else
        {//user signed in branch
            cout << "Type\n"
                 << "    app              start the application\n"
                 << "    signout          sign out\n"
                 << "    quit             to quit"
                 << endl;
        }
        //read input
        string input;
        cout << ">>";
        getline(cin,input);

        string email, password;
        //switch input
        if(input == "signin" && !is_signed_in)
        {
            cout << "Your email address\n>>";
            getline(cin,email);
            cout << "Your password\n>>";
            getline(cin,password);
            if(auto error_msg = CZORP_signin(email,password))
                cerr << error_msg.value() << endl;
            else
                is_signed_in = true;
        }
        else if(input == "signup" && !is_signed_in)
        {
            cout << "Your email address\n>>";
            getline(cin,email);
            if(auto error_msg = CZORP_signup(email))
                cerr << error_msg.value() << endl;
            else
                cout << "Please follow the instructions in your email." << endl;
        }
        else if(input == "password-reset" && !is_signed_in)
        {
            cout << "Your email address\n>>";
            getline(cin,email);
            if(auto error_msg = CZORP_password_reset(email))
                cerr << error_msg.value() << endl;
            else
                cout << "Please follow the instructions in your email." << endl;
        }
        else if(input == "signout" && is_signed_in)
        {
          if(auto error_msg = CZORP_signout())
            cerr << error_msg.value() << endl;
          else
              is_signed_in = false;
        }
        else if(input == "app" && is_signed_in)
            passwordGenerator();
        else if(input == "quit")
            break;
        else
            cout << input << " is not a valid option." << endl;
    }

    //cleanup
    CZORP_cleanup(); //cleanup and save session (token)

    return 0;
}
