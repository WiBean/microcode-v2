#include "Utilities.h"

// for abs(float)
#include <cmath>

namespace wibean {
  namespace utils {

	String boolToString(bool input) {
	  if( input ) {
		return "true";
	  }
	  else {
		return "false";
	  }
	};

	String floatToString(float input) {
	  // max out at +-1000C because we don't need more than that.  Save memory.
	  if( abs(input) >= 1000.f ) {
		input = (1-2*std::signbit(input)) * 999.9f;
	  }
	  // would be great to use sprintf here but we don't get that with spark.io/arduino strings :(
	  //char temp[6]; // 123.4\0
	  //std::sprintf(temp,"%.1f",input);
	  //return temp;

	  return String(input, 1);
	};
	
	int takeNext(String const& command, uint16_t const start, int & outValue)
	{
	  // are we at the end of the string?
	  if( start >= command.length() ) {
		return -1;
	  }
	  // find out next separator
	  int outMark = command.indexOf(',',start);
	  if( outMark == -1 ) {
		// there were no further separators found, assume the rest of the string
		// is a number
		outValue = command.substring(start).toInt();
		return command.length(); // indicate that the value is good, and we're at the end
	  }
	  // we are guarded against over-runs here because we ensure that .length() > 0 before we get here
	  else if( (outMark == start) && (outMark == command.length()-1) ) {
		// we have a trailing separator and no value after
		outMark = -1;
	  }
	  else {
		outValue = command.substring(start,outMark).toInt();
	  }
	  return outMark;
	};
  }
}