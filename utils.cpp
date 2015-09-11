#include "utils.h"
bool replace ( std::string& str, const std::string& from, const std::string& to ) {
	size_t start_pos = str.find ( from );
	if ( start_pos == std::string::npos ) {
		return true;
	}
	str.replace ( start_pos, from.length(), to );

	return false;
}
