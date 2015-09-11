/*
 * parse.cpp
 *
 *  Created on: Jun 15, 2015
 *      Author: huang_000
 */

#include "parse.h"

string vectortoString ( vector<string> vec,string delimeter ) {
	string exprstring = "";
	for ( string s : vec ) {
		if ( s=="\n" ) {
			s = "\\n";
		} else if ( s==JASM_LINE_SEP ) {
			s = "\n";
		}
		exprstring.append ( s );
		exprstring.append ( delimeter );
	}
	return exprstring;
}

bool isLetter ( const char& c ) {
	// if we do not define '"' as a character, no tokens will be generated for string quoation marks
	if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c == '"' ) || ( c == '_' ) ) {
		return true;
	} else {
		return false;
	}
}



bool isDelimiter ( const char& c ) {
	switch ( c ) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '=':
		case '(':
		case ')':
		case '{':
		case '}':
		case '^':
			// case '.':		ignore dot operator	-- object resolution will at runtime
			//					also decimal 3.14159
		case '!':
		case '>':
		case '<':
		case ',':
		case '#':
		case ';':
			return true;
		default:
			return false;
	}
}

bool isDigit ( const char& c ) {
	bool ret = false;
	switch ( c ) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			ret = true;
			break;
		default:
			ret = false;
			break;
	}
	return ret;
}

vector<string> getTokens ( string& expression ) {
	vector<string> vec;
	const char* array = expression.c_str();
	int length = expression.size();
	int index = 0;
	string currentToken = "";
	bool inQuote = false;
	bool value = false;
	while ( index < length ) {
		if ( ( array[index] == '\t'||array[index] == '\r' ) &&!inQuote ) {
			//do nothing when indent
		} else if ( array[index] == '\n'&&!inQuote ) {
			insertToken() currentToken = JASM_LINE_SEP;
			insertToken()
		} else if ( array[index] == '"' || array[index] == '\'') {
			if ( inQuote ) {
				inQuote = false;
				currentToken = currentToken + '"';
				insertToken()
			} else {
				inQuote = true;
				currentToken = currentToken + '"';
			}
		} else if ( array[index] == '\\' ) {
			if ( inQuote ) {
				char escapedChar = array[++index];
				switch ( escapedChar ) {
					case 'n':
						escapedChar = '\n';
						break;
					case 'r':
						escapedChar = '\r';
						break;
					case 't':
						escapedChar = '\t';
						break;
				}
				currentToken = currentToken + escapedChar;
			} else {
				PAR_ERROR ( string ( "escape should not in this position(" ) +to_string ( index ) +string ( ")" ) )
			}
		} else if ( array[index] == ' ' ) {
			if ( inQuote ) {
				currentToken = currentToken + array[index];
			} else {
				insertToken()
			}
		} else if ( isDigit ( array[index] ) || isLetter ( array[index] ) || ( value && array[index] == '.' ) ) {
			value = true;
			currentToken = currentToken + array[index];
		} else if ( isDelimiter ( array[index] ) ) {
			if ( inQuote ) {
				currentToken = currentToken + array[index];
			} else {
				if ( value ) {
					insertToken()
				}
				value = false;
				currentToken = currentToken + array[index];
				insertToken()
			}
		} else {
			currentToken = currentToken + array[index];
		}
		index++;
	}
	if ( currentToken != "" ) {
		insertToken()
	}
	trimTokens(vec);
	return vec;
}

double parseDecimal ( vector< string > tokens ) {
	double l = 0;
	bool valued = false;
	bool plus = false;
	bool minus = false;
	bool times = false;
	bool div = false;
	int subBraNum = 0;
	bool bracket = false;
	string subExpr = emSubExpr;
	for ( auto& token : tokens ) {
		if ( token=="" ) {
			continue;
		}
		if ( subBraNum == 0 ) {
			bracket = false;
		}
		if ( token == "(" ) {
			subBraNum++;
			bracket = true;
		} else if ( token == ")" ) {
			if ( bracket ) {
				subBraNum--;
				if ( bracket&&subBraNum == 0 ) {
					std::stringstream strstream;
					strstream<<parseDecimal ( subExpr );
					token = "";
					strstream>>token;
					bracket = !bracket;
					subExpr = "";
				}
			}
		}
		if ( bracket&&! ( subBraNum==1&&token == "(" ) ) {
			subExpr = subExpr + token;
		}
		if ( !bracket ) {
			if ( plus ) {
				l += stod ( token.c_str() );
				plus = !plus;
			} else if ( times ) {
				l = l * stod ( token.c_str() );
				times = !times;
			} else if ( div ) {
				l = l / stod ( token.c_str() );
				div = !div;
			} else if ( minus ) {
				l -= stod ( token.c_str() );
				minus = !minus;
			} else if ( !valued ) {
				l = stod ( token.c_str() );
				valued = !valued;
			} else if ( token == "+" ) {
				plus = true;
			} else if ( token == "-" ) {
				minus = true;
			} else if ( token == "*" ) {
				times = true;
			} else if ( token == "/" ) {
				div = true;
			} else {
				PAR_ERRORLN ( string ( "Token: " ) + token );
			}
		}
	}
	return l;
}

double parseDecimal ( string expression ) {
	vector<string> tokens = getTokens ( expression );
	return parseDecimal ( tokens );
}


long parseInteger ( vector<string> tokens ) {
	long l = 0;
	bool valued = false;
	bool plus = false;
	bool minus = false;
	bool times = false;
	bool div = false;
	int subBraNum = 0;
	bool bracket = false;
	string subExpr = emSubExpr;
	for ( auto& token : tokens ) {
		if ( token=="" ) {
			continue;
		}
		if ( subBraNum == 0 ) {
			bracket = false;
		}
		if ( token == "(" ) {
			subBraNum++;
			bracket = true;
		} else if ( token == ")" ) {
			if ( bracket ) {
				subBraNum--;
				if ( bracket&&subBraNum == 0 ) {
					std::stringstream strstream;
					strstream<<parseInteger ( subExpr );
					token = "";
					strstream>>token;
					bracket = !bracket;
					subExpr = "";
				}
			}
		}
		if ( bracket&&! ( subBraNum==1&&token == "(" ) ) {
			subExpr = subExpr + token;
		}
		if ( !bracket ) {
			if ( plus ) {
				l += atol ( token.c_str() );
				plus = !plus;
			} else if ( times ) {
				l = l * atol ( token.c_str() );
				times = !times;
			} else if ( div ) {
				l = l / atol ( token.c_str() );
				div = !div;
			} else if ( minus ) {
				l -= atol ( token.c_str() );
				minus = !minus;
			} else if ( !valued ) {
				l = atol ( token.c_str() );
				valued = !valued;
			} else if ( token == "+" ) {
				plus = true;
			} else if ( token == "-" ) {
				minus = true;
			} else if ( token == "*" ) {
				times = true;
			} else if ( token == "/" ) {
				div = true;
			} else {
				PAR_ERRORLN ( string ( "Token: " ) + token );
			}
		}
	}
	return l;
}

long parseInteger ( string expression ) {
	vector<string> tokens = getTokens ( expression );
	return parseInteger ( tokens );
}

/** detecting whether base is starts with str
 */
bool startsWith ( const char* base,const char* str ) {
	return ( strstr ( base, str ) - base ) == 0;
}
/** detecting whether base is ends with str
 */
bool endsWith ( const char* base,const char* str ) {
	int blen = strlen ( base );
	int slen = strlen ( str );
	return ( blen >= slen ) && ( 0 == strcmp ( base + blen - slen, str ) );
}

string& quotoken ( string&orgin ) {
	string quo = "\"";
	if ( startsWith ( orgin.c_str(),quo.c_str() ) &&endsWith ( orgin.c_str(),quo.c_str() ) ) {
		orgin = orgin.substr ( 1,orgin.size()-2 );
	}
	return orgin;
}

string parseString ( vector<string>& tokens ) {
	string s = JASM_NULL;
	int subBraNum = 0;
	bool bracket = false;
	bool plus = false;
	bool valued = false;
	string subExpr = emSubExpr;
	int count = 0;
	for ( auto&token : tokens ) {
		if ( token == "" ) {
			continue;
		}
		//cout << token << "|";
		if ( subBraNum == 0 ) {
			bracket = false;
		}
		if ( token == "(" ) {
			subBraNum++;
			bracket = true;
		} else if ( token == ")" ) {
			if ( bracket ) {
				subBraNum--;
				if ( bracket&&subBraNum == 0 ) {
					token = parseString ( subExpr );
					bracket = !bracket;
					subExpr = "";
				}
			}
		}
		if ( bracket&&! ( subBraNum == 1 && token == "(" ) ) {
			subExpr = subExpr + token;
		}
		if ( !bracket ) {
			if ( plus ) {
				s = s + quotoken ( token );
				plus = !plus;
			} else if ( !valued ) {
				s = quotoken ( token );
				valued = !valued;
			} else if ( token == "+" ) {
				plus = true;
			} else {
				PAR_ERRORLN ( string ( "Token: " ) + token );
				//cout << "Error token: " << token << endl;
				//cout << "Error token: " << (int)token[0] << endl;
			}
		}
		count++;
	}
	return s;
}

string parseString ( string expression ) {
	vector<string> tokens = getTokens ( expression );
	return parseString ( tokens );
}

bool inArray ( initializer_list<const char*> a, string b ) {
	for ( const char* c : a ) {
		if ( string ( c ) ==b ) {
			return true;
		}
//  		else{
//  			cout << c << " not equal to " << b << endl;
//  		}
	}
	return false;
}

void trimTokens ( vector< string >& vec ) {
	for ( int i = 0; i < ( signed int ) vec.size(); i++ ) {
		if ( vec[i] == " "||vec[i] == ""||vec[i]=="\t"||vec[i]=="	" ) {
			vec.erase ( vec.begin() +i );
			i--;
		}
	}
}


bool parseBool ( vector< string >& vec ) {
	string left = "";
	string oper = "";
	string right = "";

	bool forleft = true;
	int bracket = 0;
	string subexpr;
	for ( int i = 0; i < ( signed int ) vec.size(); i++ ) {
		if ( vec[i]=="(" ) {
			bracket++;
		} else if ( vec[i]==")" ) {
			bracket--;
		}
		if ( bracket==0 ) {
			if ( subexpr=="" ) {
				if ( !inArray ( JASM_OPERATIONS,vec[i] ) ) {
					if ( forleft ) {
						left = left + vec[i];
					} else {
						right = right + vec[i];
					}
				} else {
					oper = vec[i];
					forleft = false;
				}
			} else {
				subexpr = subexpr.substr ( 1 );
				if ( forleft ) {
					left = left + to_string ( parseBool ( subexpr ) );
				} else {
					right = right + to_string ( parseBool ( subexpr ) );
				}
				subexpr = "";
			}
		} else {
			subexpr = subexpr + vec[i];
			subexpr.append ( " " );
		}
	}
	//====calculate====
	if ( oper=="" ) {
		return left == "1";
	} else if ( oper=="eq" ) {
		return left == right;
	} else if ( oper=="neq" ) {
		return left != right;
	} else if ( oper=="or" ) {
		return left=="1"||right=="1";
	} else if ( oper=="and" ) {
		return left=="1"&&right=="1";
	} else if ( oper=="bg" ) {
		return atol(left.c_str()) > atol(right.c_str());
	} else if ( oper=="sm" ) {
		return atol(left.c_str()) < atol(right.c_str());
	} else {
		throw "Unsupported operation " + oper;
	}

	return false;
}

bool parseBool ( string expression ) {
	vector<string> vec = getTokens ( expression );
	return parseBool ( vec );
}

vector<string> split ( vector<string> base, string delimiter ) {
	vector<string> vec;
	string currentToken = "";
	for ( int i = 0; i < ( signed int ) base.size(); i++ ) {
		if ( base[i] == delimiter ) {
			currentToken = currentToken.substr ( 0,currentToken.size()-1 );
			vec.push_back ( currentToken );
			currentToken = "";
		} else {
			currentToken = currentToken + base[i] + " ";
		}
	}
	if ( currentToken!="" ) {
		currentToken = currentToken.substr ( 0,currentToken.size()-1 );
		vec.push_back ( currentToken );
	}
	return vec;
}

vector<string> split ( string base, char delimiter ) {
	vector<string> vec;
	string currentToken = "";
	for ( int i = 0; i < ( signed int ) base.size(); i++ ) {
		if ( base[i] == delimiter ) {
			vec.push_back ( currentToken );
			currentToken = "";
		} else {
			currentToken = currentToken + base[i];
		}
	}
	if ( currentToken!="" ) {
		vec.push_back ( currentToken );
	}
	return vec;
}




