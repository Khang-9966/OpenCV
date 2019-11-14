#ifndef _ITDEBUG_H
#define _ITDEBUG_H

#include <iostream>
#include <sstream>
#include <stdexcept>



/*

  Handy macros for debugging.  Usage:
  IT_WARN("Warning: bad value for foo:" << foo);

  */

static std::ostringstream streamToStringConverter;

/*
	The STREAM_TO_STRING macro converts a set of stream operations into a string,
	e.g STREAM_TO_STRING("Hello" << ", " << str
*/

#define STREAM_TO_STRING(arg) static_cast<std::ostringstream&>(	streamToStringConverter.str(""), \
																streamToStringConverter << arg).str()

#define IT_THROW(arg)	throw std::runtime_error(STREAM_TO_STRING(arg))


#define IT_WARN(arg)	std::clog << arg << std::endl

#endif