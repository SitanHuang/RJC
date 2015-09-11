#pragma once

#include "jasminclude.h"

#include "Class.h"

#include "parse.h"

using namespace std;

/*
jasm variables scopes
*/
enum JASM_VAR_SC {
    INVISIBLE,
    INFILE,
    GLOBAL
};

class Variable
{
public:
    Variable() {
    }
    virtual ~Variable() {
        type.clear();
        name.clear();
        varvalue.clear();
    }
    JASM_VAR_SC scope = INVISIBLE;
    string type = "";
    string name = "";
    string scopeinfo = "";
    string varvalue = "NULL";
    string classType = "";
    //NOTICE varvalue can be object, if object, ...(go to stack::processExpression)
};

void setValueToVar ( Variable* var, vector< string > expr )
{
    string type = var->type;
    //Standard types
    if ( type=="" ) {
        throw "setValueToVar(): Empty type";
    } else if ( type=="int" ) {
        var->varvalue = to_string ( parseInteger ( expr ) );
    } else if ( type=="string" ) {
        var->varvalue = parseString ( expr );
    } else if ( type=="dec" ) {
        var->varvalue = to_string ( parseDecimal ( expr ) );
    } else {
        throw "setValueToVar(): No type found: " + type;
    }
}
