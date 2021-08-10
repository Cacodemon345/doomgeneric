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
//	Main program, simply calls D_DoomMain high level loop.
//

//#include "config.h"

#include <stdio.h>

//#include "doomtype.h"
//#include "i_system.h"
#include "m_argv.h"
#ifdef _WIN32
#include <excpt.h>
#endif

//
// D_DoomMain()
// Not a globally visible function, just included for source reference,
// calls all startup code, parses command line options.
//

void D_DoomMain (void);

void M_FindResponseFile(void);

void dg_Create();

#ifdef _WINNT_NATIVE_MODE
extern int nt_printf(const char*, ...);
#define printf nt_printf
int __cdecl main(int argc, char **argv, char** envp, unsigned long debflag)
#else
#define nt_printf printf
int main(int argc, char **argv)
#endif
{
    int ncode;
    // save arguments
#ifdef _WIN32
    __try
#endif
    {
        myargc = argc;
        myargv = argv;

        M_FindResponseFile();

        // start doom
        printf("Starting D_DoomMain\r\n");

        dg_Create();

        D_DoomMain();
    }
#ifdef _WIN32
    __except(ncode = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER)
    {
        nt_printf("Exception thrown. Exiting. Code: 0x%X\n", ncode);
    }
#endif
    return 0;
}
