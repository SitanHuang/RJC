#include "print.h"

template<typename T>
string toString ( T t ) {
	stringstream ss;
	ss << t;
	return ss.str();
}

void print ( vector<string> vec ) {
	cout << toString ( parseString ( vec ) );
}


