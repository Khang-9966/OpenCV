#ifndef _util_h_
#define _util_h_


#include <stdarg.h>
#include <string.h>
#include <stdio.h>




#define CLAMP(val,min,max) \
    ((val)=(val)>(max)?(max):(val)<(min)?(min):(val))


#define NELEMS(var) \
    (sizeof((var)) / sizeof((var)[0]))




// fmt is like s(n)printf, except that you don't need
// to provide a buffer.  The downside is that you can't
// count on the pointer you get back to live forever.
// It will live for awhile though -- probably like 32
// calls at least.  Oh, and it will only support strings
// up to 1024 chars.  Even given those limitations, it's
// still rather useful when you just want to printf up a
// string.
char * Fmt( const char * fmt, ... );
char * VFmt( const char * fmt, va_list args );



// returns nonzero if the string 'needle' is in the array of strings
// 'haystack'.  Returns 0 otherwise.
int HaveArg( int count, const char ** haystack, const char * needle );


// returns the number of instances of needle in haystack
int CountArg( int count, const char ** haystack, const char * needle );


// returns the value associated with key (a string following the key in
// args) if one exists, or NULL otherwise.
const char * ValueForKey( int count, const char ** args, const char * key );


// returns the substring of str that is either the entire string,
// or the portion of the string after the last '.' character.
const char * Extension( const char * str );


// either leaves the string unmodified, or replaces the last instance
// of the '.' character in the string with a '\0'.
const char * StripExtension( char * str );


// this takes a string, and does argc/argv parsing on it, up to 'max' args.
int Argify( char * args, char * argv[], int max );







#endif /* _util_h_ */