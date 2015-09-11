#pragma once
#include "jasminclude.h"
#include "parse.h"
#include "stack.h"
#include "jasm.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"

using namespace std;

map<string, void ( * ) ( vector<string> args,string scope ) > jasmNativeFunctions;

#define nosucharguments throw "Method not found: " + string(__FUNCTION__) +\
						" for " + to_string(args.size()) + " arguments";

void array$set ( vector<string> args,string scope )
{
    using namespace rapidjson;
    if ( args.size() != 2 ) {
        nosucharguments;
    }
    vector<string> indexexpr = getTokens ( args[0] );
    vector<string> strexpr = getTokens ( args[1] );
    jasmstack::processExpression ( indexexpr,scope );
    jasmstack::processExpression ( strexpr,scope );
    int index = parseInteger ( indexexpr );
    string str = parseString ( strexpr );
    if ( startsWith ( str.c_str(),"\\\"" ) &&endsWith ( str.c_str(),"\\\"" ) ) {
        str.erase ( str.begin() );
        str.erase ( str.end()-2 );
    }
    Variable* value = jasmstack::fetch ( "value",scope );
    string v = value->varvalue;
    Document d;
    d.Parse ( v.c_str() );
    if ( index>= ( signed int ) d.Size() ) {
        throw "Index out of bound: " + to_string ( index );
    }
    Document d2;
    d2.Parse ( str.c_str() );
    if ( d2.HasParseError() ) {
        throw "Parsed error: " + str;
    }
    if ( d2.IsString() ) {
        d[index].SetString ( d2.GetString(),d2.GetStringLength() );
    } else if ( d2.IsArray() ) {
        d[index].SetArray();
        d[index].Clear();
        for ( SizeType i = 0; i < d2.Size(); i++ ) {
            d[index].PushBack ( d2[i],d.GetAllocator() );
        }
    } else if ( d2.IsDouble() ) {
        d[index].SetDouble ( d2.GetDouble() );
    } else if ( d2.IsInt() ) {
        d[index].SetInt ( d2.GetInt() );
    } else {
        throw "No type found: " + d2.GetType();
    }
    StringBuffer buffer;
    Writer<StringBuffer> writer ( buffer );
    d.Accept ( writer );
    value->varvalue = buffer.GetString();
}

void array$at ( vector<string> args,string scope )
{
    using namespace rapidjson;
    if ( args.size() !=1 ) {
        nosucharguments;
    }
    vector<string> expr = getTokens ( args[0] );
    jasmstack::processExpression ( expr,scope );
    int i = parseInteger ( expr );
    Variable* value = jasmstack::fetch ( "value",scope );
    string v = value->varvalue;
    Document d;
    d.Parse ( v.c_str() );
    if ( i>= ( signed int ) d.Size() ) {
        throw "Index out of bound: " + to_string ( i );
    }
    const Value& val = d[i];
    if ( val.IsNull() ) {
        throw "Null pointer";
    }
    Variable* returnvalue = new Variable;
    string file = regex_replace ( scope,regex ( "(^.+)>[^>]+$" ),"$1" );
    if ( val.IsArray() ) {
        returnvalue->type = "string";
        StringBuffer buffer;
        Writer<StringBuffer> writer ( buffer );
        val.Accept ( writer );
        returnvalue->varvalue = buffer.GetString();
    } else if ( val.IsDouble() ) {
        returnvalue->type = "dec";
        returnvalue->varvalue = to_string ( val.GetDouble() );
    } else if ( val.IsInt() ) {
        returnvalue->type = "int";
        returnvalue->varvalue = to_string ( val.GetInt() );
    } else if ( val.IsString() ) {
        returnvalue->type = "string";
        returnvalue->varvalue = val.GetString();
    } else {
        throw "No type found: " + val.GetType();
    }
    returnvalue->name = "return" + file;
    returnvalue->scope = INFILE;
    returnvalue->scopeinfo = file;
    jasmstack::push ( returnvalue );
}

void array$init ( vector<string> args,string scope )
{
    using namespace rapidjson;
    if ( args.size() !=2 ) {
        nosucharguments
    }
    Variable* value = jasmstack::fetch ( args[0],scope );
    string v = value->varvalue;
    Document d;
    d.Parse ( v.c_str() );
#define array$initnotanarray throw v + " is not an array!";
    if ( d.HasParseError() ||!d.IsArray() ) {
        array$initnotanarray;
    }
    const Value& val = d;
    int size = val.Size();
    if ( args[1]=="false" ) {
        Variable* var = new Variable;
        var->name = "size";
        var->scope = INFILE;
        setToLastClassScope ( var,scope );
        var->type = "int";
        var->varvalue = to_string ( size );
        jasmstack::push ( var );
    } else {
        jasmstack::fetch ( "size",scope )->varvalue = to_string ( size );
    }
}

void string$toarray ( vector<string> args,string scope )
{
    if ( args.size() !=0 ) {
        nosucharguments;
    }
    Variable* value = jasmstack::fetch ( "value",scope );
    string v ( value->varvalue );
    string returnVal = "[";
    for ( int i = 0; i < ( signed int ) v.size(); i++ ) {
        returnVal.append ( "\"" );
        string s ( 1,v[i] );
        replace ( s,"\\","\\\\" );
        replace ( s,"\"","\\\"" );
        replace ( s,"\n","\\n" );
        replace ( s,"\t","\\t" );
        replace ( s,"\r","\\r" );
        returnVal.append ( s );
        returnVal.append ( "\"," );
    }
    returnVal.erase ( returnVal.end()-1 );
    returnVal.append ( "]" );
    string file = regex_replace ( scope,regex ( "(^.+)>[^>]+$" ),"$1" );
    Variable* returnvalue = new Variable;
    returnvalue->type = "string";
    returnvalue->scopeinfo = file;
    returnvalue->scope = INFILE;
    returnvalue->varvalue = returnVal;
    returnvalue->name = "return" + file;
    jasmstack::push ( returnvalue );
}

void native_h_init()
{
    jasmNativeFunctions["array$init"] = array$init;
    jasmNativeFunctions["array$at"] = array$at;
    jasmNativeFunctions["array$set"] = array$set;
    jasmNativeFunctions["string$toarray"] = string$toarray;
}

void callNative ( string name,string scope,vector<string> passarg )
{
    if ( jasmNativeFunctions[name]==nullptr ) {
        throw "Method not found: " + name;
    }
    ( jasmNativeFunctions[name] ) ( passarg,scope );
}


