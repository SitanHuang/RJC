#pragma once
/*
 * jasminclude.cpp
 *
 *  Created on: Jun 15, 2015
 *      Author: huang_000
 */
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <cstring>
#include <regex>

/*
 * =========JASM_FUNCTION=========
 */
#define put(s) cerr << s;
#define DEB(s) cerr << "deb:" << s << endl;

/*
 * ===========JASM_VAR============
 */
#define JASM_LINE_SEP "LINE_ENDING"
#define JASM_NULL "NULL"

#define regexreplacestringtoescape(s) (startsWith(s.c_str(),"\"")&&endsWith(s.c_str(),"\"")?"\""+regex_replace(quotoken(s),regex("\""),"\\\"")+"\"":s);

