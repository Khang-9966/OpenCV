#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <FL/fl_ask.H>
#pragma warning(pop)

namespace ImageMan {
    /**
     * exception class that is thrown by IMERR_THROW.
     *
     * catch block is:<pre>
     * catch (ImageMan::Exception& e) {
     *    IMERR_WARN("Exception: " << e.what());
     * }</pre>
     */
    class Exception : public std::runtime_error {
    public:
        /**
         * Construct an exception object from a string.
         *
         * Retrieve the message of the exception with the what() method.
         */
        Exception(const std::string& msg) : std::runtime_error(msg) {
            // Put a breakpoint here to see where the exception originated from
        }
    };

    namespace debug {
        static std::ostringstream streamToStringConverter;
    };

    /*
     The STREAM_TO_STRING macro converts a set of stream operations into a string,
     e.g STREAM_TO_STRING("Hello" << ", " << str
     */
};
/**
 * Macros for debugging and error messages.
 *
 * Usage:
 * // Warns in the clog
 * ERR_WARN("Warning: bad value for foo:" << foo);
 *
 * // Throws exception
 * ERR_THROW("Warning: bad value for foo:" << foo);
 *
 * // Pop-up alert
 * ERR_ALERT("Warning: bad value for foo:" << foo);
 */

#define IMERR_STREAM_TO_STRING(arg) static_cast<std::ostringstream&>(	ImageMan::debug::streamToStringConverter.str(""), \
    ImageMan::debug::streamToStringConverter << arg).str()
#define IMERR_THROW(arg)	throw ImageMan::Exception(IMERR_STREAM_TO_STRING(arg))
#define IMERR_WARN(arg)	std::clog << arg << std::endl
#define IMERR_ALERT(arg)	fl_alert(IMERR_STREAM_TO_STRING(arg).c_str())
