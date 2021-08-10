//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	Simple basic typedefs, isolated here to make it easier
//	 separating modules.
//    


#ifndef __DOOMTYPE__
#define __DOOMTYPE__

// #define macros to provide functions missing in Windows.
// Outside Windows, we use strings.h for str[n]casecmp.


#ifdef _WIN32

#define strcasecmp _stricmp
#define strncasecmp _strnicmp

#else

#include <strings.h>

#endif

//
// The packed attribute forces structures to be packed into the minimum 
// space necessary.  If this is not done, the compiler may align structure
// fields differently to optimize memory access, inflating the overall
// structure size.  It is important to use the packed attribute on certain
// structures where alignment is important, particularly data read/written
// to disk.
//

#ifdef __GNUC__
#define PACKEDATTR __attribute__((packed))
#else
#define PACKEDATTR
#endif

// C99 integer types; with gcc we just use this.  Other compilers 
// should add conditional statements that define the C99 types.

// What is really wanted here is stdint.h; however, some old versions
// of Solaris don't have stdint.h and only have inttypes.h (the 
// pre-standardisation version).  inttypes.h is also in the C99 
// standard and defined to include stdint.h, so include this. 

#include <inttypes.h>
#include <string.h>

#ifdef _WINNT_NATIVE_MODE
// Make file functions aliases of their nt_* equivalents.
#define printf nt_printf
#define fprintf nt_fprintf
#define fopen nt_fopena
#define fclose nt_fclose
#define fread nt_fread
#define fwrite nt_fwrite
#define ftell nt_ftell
#define fseek nt_fseek
#define fclose nt_fclose
#define fflush nt_fflush
#define getenv nt_getenv
#define malloc nt_malloc
#define calloc nt_calloc
#define realloc nt_realloc
#define free nt_free
#define putchar nt_putchar
#define puts nt_puts
#define strdup nt_strdup
#define mkdir nt_mkdir
#define remove nt_remove
#define rename nt_rename
#define FILE NT_FILE
struct NT_FILE;
typedef struct NT_FILE NT_FILE;
#ifdef stderr
#undef stderr
#endif
#define stderr ((NT_FILE*)-1)
#ifdef stdout
#undef stdout
#endif
#define stdout ((NT_FILE*)-2)
extern int __cdecl nt_printf(const char* fmt, ...);
extern int __cdecl nt_fprintf(const NT_FILE* file, const char* fmt, ...);
extern NT_FILE* nt_fopen(const wchar_t* filename, const char* mode);
extern NT_FILE* nt_fopena(const char* filename, const char* mode);
extern size_t nt_fread(void* buffer, size_t size, size_t count, NT_FILE* f);
extern size_t nt_fwrite(const void* buffer, size_t size, size_t count, NT_FILE* f);
extern int nt_fflush(NT_FILE* f);
extern int nt_ftell(NT_FILE* file);
extern int nt_fseek(NT_FILE* stream, long offset, int origin);
extern void nt_fclose(NT_FILE* f);
extern void* nt_malloc(size_t size);
extern void* nt_calloc(size_t size1, size_t size2);
extern void nt_free(void* ptr);
extern void* nt_realloc(void* ptr, size_t newsize);
extern char* nt_getenv(const char* name);
extern void nt_exit(int exit_code);
extern char* nt_strdup(char* src);
extern int nt_mkdir(const char* directory);
extern int nt_remove(const char* directory);
extern int nt_rename(const char* oldfilename, const char* newfilename);
extern int nt_puts(const char* string);
extern int nt_putchar(int);
#define exit nt_exit
#pragma warning(error : 4133)
#pragma warning(error : 4022)
#pragma warning(error : 4047)
#pragma warning(disable : 4127)
#pragma warning(disable : 4242)
#endif

#ifdef __cplusplus

// Use builtin bool type with C++.

typedef bool boolean;

#else

typedef unsigned char boolean;
#define true 1
#define false 0

#endif

typedef uint8_t byte;

#include <limits.h>

#ifdef _WIN32

#define DIR_SEPARATOR '\\'
#define DIR_SEPARATOR_S "\\"
#define PATH_SEPARATOR ';'

#else

#define DIR_SEPARATOR '/'
#define DIR_SEPARATOR_S "/"
#define PATH_SEPARATOR ':'

#endif

#define arrlen(array) (sizeof(array) / sizeof(*array))

#endif

