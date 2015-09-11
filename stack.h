#pragma once

#include "jasminclude.h"

#include "Variable.h"

#include "method.h"

#include "parse.h"

using namespace std;

namespace jasmstack
{

vector<unique_ptr<Method>> methods;

vector<unique_ptr<Variable>> heap;

Variable* fetch ( string name, string scope );

Method* fetch ( string name,int args );

bool bothHave ( vector<string> vec,vector<string> vec1 );

void push ( Method* var );

void push ( Variable* var );

void free ( string name, string scope );

void gc();

void processExpression ( vector<string>& expr,string scope );
}
