#pragma once

#include "jasminclude.h"
#include <map>
#include <memory>

using namespace std;


class Class
{
public:
    Class() {

    }
    ~Class() {
        name.clear();
    }
    string name;
};

map<string,Class> classes;


