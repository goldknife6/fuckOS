#ifndef _TYPE_H
#define _TYPE_H

#ifndef __ASSEMBLER__


#ifndef NULL
#define NULL ((void*) 0)
#endif/*!NULL*/

typedef _Bool bool;
enum { false, true };

// Explicitly-sized versions of integer types
typedef __signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

// Pointers and addresses are 32 bits long.
// We use pointer types to represent virtual addresses,
// uintptr_t to represent the numerical values of virtual addresses,
// and physaddr_t to represent physical addresses.

typedef uint32_t uintptr_t;
typedef uint32_t viraddr_t;
#ifndef CONFIG_PAE
typedef uint32_t physaddr_t;
#else
typedef uint64_t physaddr_t;
#endif/*_CONFIG_PAE_*/

// Page numbers are 32 bits long.
typedef uint32_t pfn_t;

typedef int32_t pid_t;
// size_t is used for memory object sizes.

typedef uint32_t size_t;
// ssize_t is a signed version of ssize_t, used in case there might be an
// error return.
typedef int32_t ssize_t;

// off_t is used for file offsets and lengths.
typedef int32_t off_t;

typedef uint16_t dev_t;
#endif/*!__ASSEMBLER__*/

#endif/*_TYPE_H*/


