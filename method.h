/*
 * Created by Sitan HUang
 */
#pragma once

#include "jasminclude.h"

using namespace std;

class Method
{
public:
    Method();
    ~Method();

    vector<string> code;
    string name = "";
    vector<string> arguments;

};
