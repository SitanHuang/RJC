#pragma once

#include "jasminclude.h"

using namespace std;

string vectortoString ( vector< string > vec, string delimeter = " " );

bool isLetter ( const char& c );

bool isDelimiter ( const char& c );

bool isDigit ( const char& c );

#define insertToken()                   \
    vec.push_back(currentToken);         \
    currentToken = "";

#define emSubExpr ""

#define PAR_ERROR(s) throw "Parsed error: " + s;
#define PAR_ERRORLN(s) throw "Parsed error: " + s + "\n";

vector<string> getTokens ( string& expression );

double parseDecimal ( vector<string> expression );

double parseDecimal ( string expression );

long parseInteger ( vector<string> expression );

long parseInteger ( string expression );

bool startsWith ( const char* base, const char* str );

bool endsWith ( const char* base, const char* str );

string& quotoken ( string&orgin );

string parseString ( vector<string>& vec );

string parseString ( string expression );

bool inArray ( initializer_list<const char*> a, string b );

void trimTokens ( vector<string>& vec );

#define JASM_OPERATIONS {"eq","neq","or","and"}

bool parseBool ( vector<string>& vec );

bool parseBool ( string expression );

vector<string> split ( vector<string> base, string delimiter );

vector<string> split ( string base, char delimiter = '>' );


