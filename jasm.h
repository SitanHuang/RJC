#pragma once

#include <string>
#include <vector>
#include <fstream>

#define _____DEBUG

#include "Variable.h"
#include "stack.h"
#include "print.h"
#include "utils.h"
#include "method.cpp"
#include "Class.h"
#include <memory>

#include <limits.h> /* PATH_MAX */
#include <stdio.h>
#include <stdlib.h>

using namespace std;

string MAIN_FILE = "";

void setToLastClassScope ( Variable* v, string scope )
{
    vector<string> scopes = split ( scope );
    v->scopeinfo = scopes[0];
    bool matched = false;
    for ( int i = 1; i < ( signed int ) scopes.size(); i++ ) {
        if ( regex_match ( scopes[i],regex ( "\\$class\\$.+[0-9]+" ) ) ) {
            v->scopeinfo.append ( ">" );
            v->scopeinfo.append ( scopes[i] );
            matched = true;
        } else if ( !matched ) {
            v->scopeinfo.append ( ">" );
            v->scopeinfo.append ( scopes[i] );
        } else {
            break;
        }
    }
}
#include "native.h"

/**
 * Read until a token
 * Used for skip tokens, doesn't return anything
 */
void readUntil ( vector<string>&tokens, string end, int& index )
{
    for ( ; index < ( signed int ) tokens.size(); index++ ) {
        if ( tokens[index] == end ) {
            break;
        }
    }
}

/**
 * Read until a token
 * Used for read until a line
 * @return tokens
 */
vector<string> readUntil2 ( vector<string>&tokens, string end, int& index )
{
    vector<string> vec;
    for ( ; index < ( signed int ) tokens.size(); index++ ) {
        if ( tokens[index] == end ) {
            break;
        }
        vec.push_back ( tokens[index] );
    }
    return vec;
}

/**
 * read until tokens
 * include sub statements
 * Used for read a code block
 * @return tokens
 * @param start sub statements start
 * @param end sub statements and this statement ending
 */
vector<string> advReadUntil2 ( vector<string> tokens,initializer_list<const char*> start, initializer_list<const char*> end,
                               int& index, int starts = 1 )
{
    vector<string> vec;
    for ( ; index < ( signed int ) tokens.size(); index++ ) {
        if ( tokens[index]=="fcn"||tokens[index]=="loop"||tokens[index]=="if" ) {
            vec.push_back ( tokens[index] );
            for ( string s : advReadUntil2 ( tokens,start, {"end"},++index ) ) {
                vec.push_back ( s );
            }
            vec.push_back ( "end" );
        } else {
            if ( inArray ( end,tokens[index] ) ) {
                starts--;
            }
            if ( starts<=0 ) {
                break;
            }
            vec.push_back ( tokens[index] );
        }
    }
    return vec;
}

#define printVector(v) for(auto& s : v){\
					       cout << s << " ";\
					   }

std::string dirnameOf(const std::string& fname)
{
     size_t pos = fname.find_last_of("\\/");
     return (std::string::npos == pos)
         ? ""
         : fname.substr(0, pos);
}


/***
 * Run code from string
 */
void JASM_RUNCODE ( vector<string> tokens,string file="$inline$",string className = "" )
{
    int line = 0;
    try {
        if ( file == "$inline$" ) {
            file.append ( to_string ( rand() % 10000 ) );
        }
        vector<Variable*> tmpVars;
        for ( int index = 0; index < ( signed int ) tokens.size(); index++,line++ ) {
            if ( tokens[index] == ""||tokens[index] == JASM_LINE_SEP ) {
                continue;
            } else if ( tokens[index] == "/" ) {
                readUntil ( tokens, JASM_LINE_SEP, index );
                continue;
            } else if ( tokens[index] == ( "#" ) ) { //output
                vector<string> expr = readUntil2 ( tokens, JASM_LINE_SEP, ++index );
                jasmstack::processExpression ( expr,file );
                print ( expr );
            } else if ( tokens[index].find ( ":" ) == 0||tokens[index].find ( "$" ) == 0 ||tokens[index].find ( "@" ) ==0 ) { //define variable
                bool global = tokens[index].find ( "$" ) == 0;
                bool inClass = tokens[index].find ( "@" ) ==0;
                string type = tokens[index].substr ( 1, tokens[index].length() );
                string name = tokens[++index];
                Variable* v = new Variable;
                v->name = name;
                v->type = type;
                v->scope = ( global?GLOBAL:INFILE );
                v->scopeinfo = ( global?"$global$":file );
                if ( inClass ) {
                    setToLastClassScope ( v,file );
                }
                vector<string> expr = readUntil2 ( tokens, JASM_LINE_SEP, ++index );
                if ( v->type!="class" ) {
                    jasmstack::processExpression ( expr,file );
                    setValueToVar ( v,expr );
                } else {
                    //:class awesomeObject freakingAwesome "Sitan Huang"
                    string klass = expr[0];
                    auto it = classes.find ( klass );
                    Class c = it->second;
                    if ( it==classes.end() ) {
                        throw "Class not found: " + klass;
                    }
                    //Calling init function
                    string methodName = klass+"$init";
                    int i = 1;
                    vector<string> passarg = readUntil2 ( expr,JASM_LINE_SEP, i );
                    passarg = split ( passarg,"," );
                    Method* method = jasmstack::fetch ( methodName,passarg.size() );
                    if ( method==nullptr ) {
                        throw "Method not found: " + methodName + " for " + to_string ( passarg.size() ) + " arguments";
                    }
                    vector<string> code ( method->code );
                    for ( int i = 0; i < ( signed int ) method->arguments.size(); i++ ) {
                        if ( getTokens ( method->arguments[i] ) [0]=="string" ) {
                            passarg[i] = regexreplacestringtoescape ( passarg[i] );;;;
                        }
                        string s = ":" + method->arguments[i] + " " + passarg[i];
                        vector<string> tokens2 = getTokens ( s );
                        tokens2.push_back ( JASM_LINE_SEP );
                        for ( int i2 = ( tokens2.size()-1 ); i2 >= 0; i2-- ) {
                            code.insert ( code.begin(),tokens2[i2] );
                        }
                    }
                    string scope = ( inClass?v->scopeinfo:file );
                    scope.append ( ">$class$" );
                    scope.append ( klass );
                    scope.append ( to_string ( rand() %10000 ) );
                    v->varvalue = scope;
                    v->classType = klass;
                    JASM_RUNCODE ( code,scope );
                }
                jasmstack::push ( v );
                if ( !global && !inClass ) {
                    tmpVars.push_back ( v );
                }
            } else if ( tokens[index]=="if" ) {
                vector<string> vec = advReadUntil2 ( tokens, {"if"}, {"end"},++index );
                vec.push_back ( "end" );
                string scope = file;
                int i = 0;
                //should read from vec
                vector<string> expr= readUntil2 ( vec,JASM_LINE_SEP,i );
                //still in parent scope
                jasmstack::processExpression ( expr,file );
                if ( parseBool ( expr ) ) {
                    vector<string> code = advReadUntil2 ( vec, {"if"}, {"end","elif","else"},i );
                    scope.append ( ">if" );
                    scope.append ( to_string ( rand() % 10000 ) );
                    JASM_RUNCODE ( code,scope );
                } else {
                    //skip if
                    advReadUntil2 ( vec, {"if"}, {"end","elif","else"},i );
                    i--;
                    while ( i < ( signed int ) vec.size() ) {
                        i++;
                        if ( vec[i] == "elif" ) {
                            expr= readUntil2 ( vec,JASM_LINE_SEP,++i );
                            jasmstack::processExpression ( expr,file );
                            if ( parseBool ( expr ) ) {
                                vector<string> code = advReadUntil2 ( vec, {"if"}, {"end","elif","else"},i );
                                scope = file;
                                scope.append ( ">elif" );
                                scope.append ( to_string ( rand() % 10000 ) );
                                JASM_RUNCODE ( code,scope );
                                break;
                            } else
                                vector<string> code = advReadUntil2 ( vec, {"if"}, {"end","elif","else"},i );
                            i--;
                        } else if ( vec[i] == "end" ) {
                            break;
                        } else if ( vec[i] == "else" ) {
                            vector<string> code = advReadUntil2 ( vec, {"if"}, {"end","elif","else"},++i );
                            scope = file;
                            scope.append ( ">else" );
                            scope.append ( to_string ( rand() % 10000 ) );
                            JASM_RUNCODE ( code,scope );
                            i--;
                        }
                    }
                }
            } else if ( tokens[index]=="fcn" ) {
                string funname = ( className!=""?className + "$":className ) + tokens[++index];
                vector<string> argvec = readUntil2 ( tokens,JASM_LINE_SEP,++index );
                vector<string> arguments = split ( argvec,"," );
                Method* method = new Method;
                method->name = funname;
                method->arguments = arguments;
                method->code = advReadUntil2 ( tokens, {}, {"end"},++index );
                jasmstack::push ( method );
            } else if ( tokens[index]=="loop" ) {
                vector<string> expr = readUntil2 ( tokens,JASM_LINE_SEP,++index );
                vector<string> code = advReadUntil2 ( tokens, {}, {"end"},++index );
                vector<string> newexpr ( expr );
                jasmstack::processExpression ( newexpr,file );
                while ( parseBool ( newexpr ) ) {
                    string scope = file;
                    scope.append ( ">loop" );
                    scope.append ( to_string ( rand() % 10000 ) );
                    JASM_RUNCODE ( code,scope );
                    newexpr = vector<string> ( expr );
                    jasmstack::processExpression ( newexpr,file );
                }
            } else if ( tokens[index]=="return" ) {
                string type = tokens[++index];
                vector<string> expr = readUntil2 ( tokens,JASM_LINE_SEP,++index );
                jasmstack::processExpression ( expr,file );
                Variable* returnvalue = new Variable;
                returnvalue->name = "return" + file;
                returnvalue->type = type;
                returnvalue->scope = INFILE;
                returnvalue->scopeinfo = file;
                setValueToVar ( returnvalue,expr );
                jasmstack::push ( returnvalue );
                index = tokens.size();
            } else if ( tokens[index]=="load" ) {
				vector<string> expr = readUntil2 ( tokens,JASM_LINE_SEP,++index );
				jasmstack::processExpression ( expr,file );
				string newfile = parseString(expr);
                newfile = dirnameOf(MAIN_FILE) + "/" + quotoken ( newfile );
                string code = "";
                char c;
                fstream input;
                input.open ( newfile );
                if ( input.fail() ) {
                    throw to_string ( input.failbit ) + ":Error: faild to open '" + newfile + "'.";
                }
                while ( input.get ( c ) ) {
                    code = code + c;
                }
                vector<string> tokens = getTokens ( code );
                JASM_RUNCODE ( tokens,newfile );
            } else if ( tokens[index]=="class" ) {
                string name = tokens[++index];
                vector<string> code = advReadUntil2 ( tokens, {}, {"end"},++index );
                Class klass;
                klass.name = name;
                classes[name] = klass;
				
                string scope = file;
                scope.append ( ">decclass" );
                scope.append ( name );
                JASM_RUNCODE ( code,scope,name );
            } else if ( tokens[index]=="module" ) {
                string name = tokens[++index];
                vector<string> code = advReadUntil2 ( tokens, {}, {"end"},++index );
                Class klass;
                klass.name = name;
                classes[name] = klass;
				
				string funname = name + "$init";
                vector<string> arguments;
                Method* method = new Method;
                method->name = funname;
                method->arguments = arguments;
                method->code = vector<string>();
				jasmstack::push(method);
                string scope = file;
                scope.append ( ">decclass" );
                scope.append ( name );
                JASM_RUNCODE ( code,scope,name );
				vector<string> cod = {":class",name,name,JASM_LINE_SEP};
				JASM_RUNCODE ( cod,file );
            } else if ( tokens[index]=="callnative" ) {
                string methodName = tokens[++index];
                vector<string> passarg = readUntil2 ( tokens,JASM_LINE_SEP,++index );
                passarg = split ( passarg,"," );
                string scope = file;
                scope.append ( ">" );
                scope.append ( methodName );
                scope.append ( to_string ( rand() % 10000 ) );
                callNative ( methodName,scope,passarg );
            } else {
                Variable* var = jasmstack::fetch ( tokens[index],file );
                if ( var ) {
                    //Variable
                    vector<string> expr = readUntil2 ( tokens,JASM_LINE_SEP,++index );
                    jasmstack::processExpression ( expr,file );
                    setValueToVar ( var,expr );
                } else {
                    //Calling function
                    string methodName = tokens[index];
                    vector<string> klass = split ( methodName,'.' );
                    vector<string> passarg = readUntil2 ( tokens,JASM_LINE_SEP,++index );
                    passarg = split ( passarg,"," );
                    string scope = file;
                    scope.append ( ">" );
                    scope.append ( methodName );
                    scope.append ( to_string ( rand() % 10000 ) );
                    Method* method;
                    if ( klass.size() > 1 ) {

                        Variable* var;
                        for ( int i = 0; i < ( signed int ) klass.size()-1; i++ ) {
                            if ( i==0 ) {
                                var = jasmstack::fetch ( klass[i],file );
                            } else {
                                if ( var==nullptr ) {
                                    throw "Variable not found: " + methodName;
                                }
                                var = jasmstack::fetch ( klass[i],var->varvalue );
                            }
                        }
                        if ( var==nullptr ) {
                            throw "Variable not found: " + methodName;
                        }
                        methodName = var->classType+"$"+klass[klass.size()-1];
                        scope = var->varvalue + ">" + methodName +  to_string ( rand() % 10000 ) ;
                    }
                    method = jasmstack::fetch ( methodName,passarg.size() );
                    if ( method==nullptr ) {
                        throw "Method not found: " + methodName + " for " + to_string ( passarg.size() ) + " arguments";
                    }
                    vector<string> code ( method->code );
                    for ( int i = 0; i < ( signed int ) method->arguments.size(); i++ ) {
                        if ( getTokens ( method->arguments[i] ) [0]=="string" ) {
                            passarg[i] = regexreplacestringtoescape ( passarg[i] );
                        }
                        string s = ":" + method->arguments[i] + " " + passarg[i];
                        vector<string> tokens2 = getTokens ( s );
                        tokens2.push_back ( JASM_LINE_SEP );
                        tokens2.push_back ( JASM_LINE_SEP );
                        for ( int i2 = ( tokens2.size()-1 ); i2 >= 0; i2-- ) {
                            code.insert ( code.begin(),tokens2[i2] );
                        }
                    }
                    JASM_RUNCODE ( code,scope );
                }
            }
        }
        //======delete tmp variables
        for ( auto& v : tmpVars ) {
            v->scope = INVISIBLE;//let gc do it
        }

        jasmstack::gc();//TODO Auto gc without gc before return
#define runcode_throw_trace(s) cerr << "Error occured at " << file << " line " << ++line << endl;\
		throw s;
    } catch ( string& s ) {
        runcode_throw_trace ( s );
    } catch ( const char** s ) {
        runcode_throw_trace ( s );
    } catch ( char** s ) {
        runcode_throw_trace ( s );
    } catch ( char const* s ) {
        runcode_throw_trace ( s );
    } catch ( exception& s ) {
        runcode_throw_trace ( s );
    }
}

#include <chrono>
int main ( int argc, char **argv )
{
    try {
        native_h_init();
        if ( argc!=2 ) {
            throw string ( "Missing argument" );
        }
        string test_file = argv[1];
		char buf[PATH_MAX + 1];
		char *res = realpath(test_file.c_str(), buf);
		MAIN_FILE = string(res);
        srand ( time ( nullptr ) );
        string code = "";
        char c;
        fstream input;
        input.open ( test_file );
        if ( input.fail() ) {
            cerr << input.failbit << ": " << "Error: faild to open '" <<
                 test_file << "'." << endl;
            return 1;
        }
        while ( input.get ( c ) ) {
            code = code + c;
        }
        vector<string> tokens = getTokens ( code );
        JASM_RUNCODE ( tokens,test_file );
        jasmstack::gc();
        return 0;
    } catch ( string& s ) {
        cerr << s << endl;
        return 1;
    } catch ( const char** s ) {
        cerr << s << endl;
        return 1;
    } catch ( char** s ) {
        cerr << s << endl;
        return 1;
    } catch ( char const* s ) {
        cerr << s << endl;
        return 1;
    } catch ( exception& s ) {
        cerr << s.what() << endl;
        return 1;
    }
}


