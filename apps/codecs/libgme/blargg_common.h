// Sets up common environment for Shay Green's libraries.
// To change configuration options, modify blargg_config.h, not this file.

#ifndef BLARGG_COMMON_H
#define BLARGG_COMMON_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#undef BLARGG_COMMON_H
// allow blargg_config.h to #include blargg_common.h
#include "blargg_config.h"
#ifndef BLARGG_COMMON_H
#define BLARGG_COMMON_H

#if defined(ROCKBOX)
#include "codeclib.h"
#endif

#if 1 /* IRAM configuration is not yet active for all libGME codecs. */
    #undef  ICODE_ATTR
    #define ICODE_ATTR

    #undef  IDATA_ATTR
    #define IDATA_ATTR

    #undef  ICONST_ATTR
    #define ICONST_ATTR

    #undef  IBSS_ATTR
    #define IBSS_ATTR
#endif

// BLARGG_RESTRICT: equivalent to C99's restrict, where supported
#if __GNUC__ >= 3 || _MSC_VER >= 1100
	#define BLARGG_RESTRICT __restrict
#else
	#define BLARGG_RESTRICT
#endif

// STATIC_CAST(T,expr): Used in place of static_cast<T> (expr)
#ifndef STATIC_CAST
	#define STATIC_CAST(T,expr) ((T) (expr))
#endif

// blargg_err_t (0 on success, otherwise error string)
#ifndef blargg_err_t
	typedef const char* blargg_err_t;
#endif

#define BLARGG_4CHAR( a, b, c, d ) \
	((a&0xFF)*0x1000000L + (b&0xFF)*0x10000L + (c&0xFF)*0x100L + (d&0xFF))

// BOOST_STATIC_ASSERT( expr ): Generates compile error if expr is 0.
#ifndef BOOST_STATIC_ASSERT
	#ifdef _MSC_VER
		// MSVC6 (_MSC_VER < 1300) fails for use of __LINE__ when /Zl is specified
		#define BOOST_STATIC_ASSERT( expr ) \
			void blargg_failed_( int (*arg) [2 / (int) !!(expr) - 1] )
	#else
		// Some other compilers fail when declaring same function multiple times in class,
		// so differentiate them by line
		#define BOOST_STATIC_ASSERT( expr ) \
			void blargg_failed_( int (*arg) [2 / !!(expr) - 1] [__LINE__] )
	#endif
#endif

// BLARGG_COMPILER_HAS_BOOL: If 0, provides bool support for old compiler. If 1,
// compiler is assumed to support bool. If undefined, availability is determined.
#ifndef BLARGG_COMPILER_HAS_BOOL
	#if defined (__MWERKS__)
		#if !__option(bool)
			#define BLARGG_COMPILER_HAS_BOOL 0
		#endif
	#elif defined (_MSC_VER)
		#if _MSC_VER < 1100
			#define BLARGG_COMPILER_HAS_BOOL 0
		#endif
	#elif defined (__GNUC__)
		// supports bool
	#elif __cplusplus < 199711
		#define BLARGG_COMPILER_HAS_BOOL 0
	#endif
#endif
#if defined (BLARGG_COMPILER_HAS_BOOL) && !BLARGG_COMPILER_HAS_BOOL
	// If you get errors here, modify your blargg_config.h file
	typedef int bool;
	static bool true  = 1;
	static bool false = 0;
#endif

// blargg_long/blargg_ulong = at least 32 bits, int if it's big enough
#include <limits.h>

#if INT_MAX >= 0x7FFFFFFF
	typedef int blargg_long;
#else
	typedef long blargg_long;
#endif

#if UINT_MAX >= 0xFFFFFFFF
	typedef unsigned blargg_ulong;
#else
	typedef unsigned long blargg_ulong;
#endif

// int8_t etc.


// ROCKBOX: If defined, use <codeclib.h> for int_8_t etc
#if defined (ROCKBOX)
	#include <codecs/lib/codeclib.h>
// HAVE_STDINT_H: If defined, use <stdint.h> for int8_t etc.
#elif defined (HAVE_STDINT_H)
	#include <stdint.h>
	#define BOOST

// HAVE_INTTYPES_H: If defined, use <stdint.h> for int8_t etc.
#elif defined (HAVE_INTTYPES_H)
	#include <inttypes.h>
	#define BOOST

#else
	#if UCHAR_MAX == 0xFF && SCHAR_MAX == 0x7F
		typedef signed char     int8_t;
		typedef unsigned char   uint8_t;
	#else
		// No suitable 8-bit type available
		typedef struct see_blargg_common_h int8_t;
		typedef struct see_blargg_common_h uint8_t;
	#endif
		
	#if USHRT_MAX == 0xFFFF
		typedef short           int16_t;
		typedef unsigned short  uint16_t;
	#else
		// No suitable 16-bit type available
		typedef struct see_blargg_common_h int16_t;
		typedef struct see_blargg_common_h uint16_t;
	#endif
		
	#if ULONG_MAX == 0xFFFFFFFF
		typedef long            int32_t;
		typedef unsigned long   uint32_t;
	#elif UINT_MAX == 0xFFFFFFFF
		typedef int             int32_t;
		typedef unsigned int    uint32_t;
	#else
		// No suitable 32-bit type available
		typedef struct see_blargg_common_h int32_t;
		typedef struct see_blargg_common_h uint32_t;
	#endif
#endif

#endif
#endif
