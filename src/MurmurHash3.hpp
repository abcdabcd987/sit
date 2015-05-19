// Modified and adapted by Lequn Chen

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#include <cstdint>

#include <tuple>
//-----------------------------------------------------------------------------

#ifdef ENV64
typedef std::tuple<uint64_t, uint64_t> HashValue;
HashValue MurmurHash3 ( const void * key, int len, uint32_t seed = 42 );
#else
typedef std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> HashValue;
HashValue MurmurHash3 ( const void * key, int len, uint32_t seed = 42 );
#endif


//-----------------------------------------------------------------------------

#endif // _MURMURHASH3_H_
