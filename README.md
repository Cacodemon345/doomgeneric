# DoomGeneric NTNative
This is a fork of ozkl's DoomGeneric which adds port to the BootExecute environment of Windows XP and later.

# Requirements for building DoomGeneric NTNative
1. Visual Studio 2017 Windows XP toolset.
2. InbvShim (https://github.com/Cacodemon345/inbvbootdrv).
3. Windows 7 DDK (for building InbvShim and ntdll.lib).

# Requirements for running
Only tested on Windows XP 32-bit. I don't know about later versions.

# Building
Note: Only 32-bit Debug builds are supported, anything else is broken.
Copy over the ntdll.lib file from \\path\to\WinDDK\7600.16385.1\lib\wxp\i386\ to the doomgeneric directory inside the project. Open the solution, right click "doomgeneric_nt" and select "Build".

# Building InbvShim
From the x86/x64 Free Build Environment, cd to the directory where you have cloned InbvShim repository, and type 'build' to build the driver. You will find the inbvbootdrv.sys file in the objfre_wxp_x86 (objfre_win7_x64 if building for x64) folder.

# Installing InbvShim
Copy it to your system32\Drivers directory of your Windows installation. And then grab the inbvbootdrvinst.reg from one of the releases and double-click it to install.

# Installing DoomGeneric NTNative
Copy doomgeneric_nt.exe to C:\Windows\system32\ directory.
There are two ways of launching it:
1. Registry modification:

**WARNING:** Backing up the registry is very important before going down this path.

Open Registry Editor (regedit), go to HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\. Double-click BootExecute.

Replace this:
```
autocheck autochk *
```

with:
```
autocheck autochk *
doomgeneric_nt -iwad \??\C:\Windows\doom2.wad -cdrom
```

Replace \\??\C:\Windows\doom2.wad with the location where the IWAD is installed (remember to prefix it with \\??\ before the full path).

Note that I don't recommend this way because it can make input non-functional, making it impossible to quit the program and requiring a hard reset.

2. Native Shell (assuming you installed it beforehand):

cd to the directory where DoomGeneric NTNative is installed and type:
```
doomgeneric_nt.exe -iwad \??\path\to\iwad.wad -cdrom
```

Replace \\path\to\ with the location where the IWAD is installed.

# Extra bonuses:
1. 16-color mode for Windows GDI port activated with -4bit option.

# Bugs:
1. Savegames are broken.
2. Picking a weapon crashes the program (bug inherited from original DoomGeneric).
3. It's slow as hell, probably could use FastDoom's EGA drawing code for it.

# License:
Same as original DoomGeneric, except for some files:

i_main_nt.c: ReactOS project license.
doomgeneric_nt.c: Uses code both from ZenWINX and Native Shell (LGPL).
Bundled NDK: Used under the terms of GPLv2.
