#include "jasm.h"

#pragma once

#include "stack.h"

#include "method.h"

#include <memory>

#define gd(s)

namespace jasmstack {



	Method* fetch ( string name,int arg ) {
		for ( unique_ptr<Method>& ptr : methods ) {
			if ( name==ptr->name&&arg == ( signed int ) ptr->arguments.size() ) {
				return ptr.get();
			}
		}
		return nullptr;
	}


	Variable* fetch ( string name, string scope ) {
		for ( auto& ptr : heap ) {
			if ( ptr->name==name&& ( scope.find ( ptr->scopeinfo ) ==0||ptr->scopeinfo=="$global$" ) ) {
				return ptr.get();
			}
		}
		return nullptr;
	}

	void push ( Method* var ) {
		for ( unique_ptr<Method>& ptr : methods ) {
			if ( var->name==ptr->name&&var->arguments.size() == ptr->arguments.size() ) {
				methods.erase(std::remove(methods.begin(), methods.end(), ptr), methods.end());
				break;
			}
		}
		methods.push_back ( unique_ptr<Method> ( var ) );
	}

	void push ( Variable* var ) {
		heap.insert ( heap.begin(),unique_ptr<Variable> ( var ) );
	}

	void gc() {
		if ( heap.size() >100 ) {
			gd ( cout << endl << "======gc" << heap.size() << "=======" << endl );
			for ( int i = 0; i < ( signed int ) heap.size(); i++ ) {
				if ( heap[i]->scope == INVISIBLE ) {
					gd ( string info = "gc: removed " + heap[i]->name + " scopeinfo=" + heap[i]->scopeinfo );
					delete heap[i].release();
					heap.erase ( heap.begin() + i );
					i--;
					gd ( cout << info << endl );
				} else {
					if ( heap[i] == nullptr ) {
						delete heap[i].release();
						heap.erase ( heap.begin() + i );
						i--;
						gd ( cout << "gc: removed one unusable variable" << endl );
					} else {
						gd ( cout << "gc: skip " << heap[i]->name << " scopeinfo="
						     << heap[i]->scopeinfo << endl );
					}
				}
			}
		}
	}

	bool isNum ( string s ) {

		bool num = true;
		bool dot = false;

		for ( int i = 0; i < ( signed int ) s.size(); i++ ) {
			if ( !isDigit ( s[i] ) ) {
				num  = false;
			}
			if ( s[i] == '.' ) {
				dot = true;
			}
		}

		if ( dot&&!num ) {
			return false;
		}
		if ( !num ) {
			return false;
		}

		return true;
	}

	void processExpression ( vector< string >& expr, string scope ) {
		for ( int i = 0; i < ( signed int ) expr.size(); i++ ) {
			if ( ! ( startsWith ( expr[i].c_str(),"\"" ) &&endsWith ( expr[i].c_str(),"\"" ) ) //not string
			        &&! ( expr[i].size() ==1&&isDelimiter ( expr[i].operator[] ( 0 ) ) ) //not delimiter
			        &&expr[i]!=""&&expr[i]!="\t"&&expr[i]!=JASM_LINE_SEP//not whitespace
			        &&!inArray ( JASM_OPERATIONS,expr[i] ) //not operator
			        &&!isNum ( expr[i] ) //not number
			   ) {
				string name = expr[i];
				Variable* var = fetch ( name,scope );
// 				cout << ">"+var->name << ":" << scope << ":" + var->varvalue << endl;
				if ( var==nullptr ) {
					//Calling function
					int start = i;
					string methodName = expr[i];
					vector<string> klass = split ( methodName,'.' );
					string scope2 = scope;
					scope2.append ( ">" );
					scope2.append ( methodName );
					scope2.append ( to_string ( rand() % 10000 ) );
					vector<string> passarg = advReadUntil2 ( expr, {}, {";"},++i );
					i++;
					for ( int i2 = 0; i2 <= ( signed int ) passarg.size() +1; i2++ ) {
						expr[start + i2] = "";
					}
					passarg = split ( passarg,"," );
					if ( klass.size() > 1 ) {

						Variable* var;
						for ( int i = 0; i < ( signed int ) klass.size()-1; i++ ) {
							if ( i==0 ) {
								var = fetch ( klass[i],scope );
							} else {
								if ( var==nullptr ) {
									throw "Variable not found: " + methodName;
								}
								var = fetch ( klass[i],var->varvalue );
							}
						}
						if ( var==nullptr ) {
							throw "Variable not found: " + methodName;
						}
						methodName = var->classType+"$"+klass[klass.size()-1];
						scope2 = var->varvalue + ">" + methodName +  to_string ( rand() % 10000 ) ;
					}
					Method* method = fetch ( methodName,passarg.size() );
					if ( method==nullptr ) {
						throw "Method not found: " + methodName + " for " + to_string ( passarg.size() ) + " arguments";
					}
					vector<string> code ( method->code );
					for ( int i3 = 0; i3 < ( signed int ) method->arguments.size(); i3++ ) {
						if ( getTokens ( method->arguments[i3] ) [0]=="string" ) {
							passarg[i3] = regexreplacestringtoescape ( passarg[i3] );
						}
						string s = ":" + method->arguments[i3] + " " + passarg[i3];
						vector<string> tokens2 = getTokens ( s );
						tokens2.push_back ( JASM_LINE_SEP );
						tokens2.push_back ( JASM_LINE_SEP );
						for ( int i2 = ( tokens2.size()-1 ); i2 >= 0; i2-- ) {
							code.insert ( code.begin(),tokens2[i2] );
						}
					}
					JASM_RUNCODE ( code,scope2 );
					Variable* returnVar = fetch ( string ( "return" + scope2 ),scope2 );
					if ( returnVar==nullptr ) {
						throw "Method didn't return anything";
					}
					expr[start] = returnVar->varvalue;
					if ( returnVar->type=="string" ) {
						expr[start] = regexreplacestringtoescape ( expr[start] );;;
						quotoken ( expr[start] );
						expr[start] = "\"" + expr[start] + "\"";
					}
				} else {
					if ( var->type=="class" ) {
						expr[i] = var->name + ".toString";
						expr.insert ( expr.begin() + ( i+1 ),";" );
						i--;
						continue;
					}
					expr[i] = var->varvalue;
					if ( var->type=="string" ) {
						expr[i] = regexreplacestringtoescape ( expr[i] );;
						quotoken ( expr[i] );
						expr[i] = "\"" + expr[i] + "\"";
					}
				}
			}
		}
	}

}


