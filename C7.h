/**
## License & Legal Information
Released under the GNU GPL v3.  See http://www.gnu.org/licenses/gpl.txt

This project is a port of the classic DeSmet C Compiler from DOS to
Windows(x86/x64). It combines original source code with modern platform
adaptations. 

### Compiler Toolchain
The core compiler toolchain is licensed under the
 **GNU General Public License, Version 3.0 (GPLv3)**. 

The original DeSmet C compiler source code was released under the terms of the
GNU General Public License, either **Version 2 or any later version**.
In accordance with that provision, this Windows port exercises the option to
upgrade the distribution terms to **GPLv3** to ensure modern patent protection
and better compatibility with contemporary open-source build tools.

* A copy of the full license text is available in the [LICENSE](./LICENSE) file.
* Original source files retain their historical copyright notices.

#### Original DeSmet C Compiler source code License & Legal Information
*  Released under the GNU GPL.  See http://www.gnu.org/licenses/gpl.txt
*
*  This program is part of the DeSmet C Compiler
*
*  DeSmet C is free software; you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by the
*  Free Software Foundatation; either version 2 of the License, or any
*  later version.
*
*  DeSmet C is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
*  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
*  for more details.
**/

/*  C88 COMPILER PASS1	-	C7.H  */

#ifndef _C7_H
#define _C7_H

#include <stdint.h>

/* FUNCTION PROTOTYPES */

void e_msg(int, const char*);
void error(const char*);
void error_l(const char*);
void warning(const char*);
void nooper(void);
int typeof_(char*);
void real_exit(int);
unsigned int dsize(char*);
int scaled(char*);

// OUTPUT SINGLE CHRACTER OR STRING
void oc(char);
// OUTPUT STRING
void os(const char*);

// OUTPUT SIGNED OR UNSIGNED NUMBERS
void onum(int);
// OUTPUT SIGNED OR UNSIGNED BYTE
void obnum(char);
// OUTPUT HEX OF UNSIGNED BYTE (8-bit)
void ohb(char);
// OUTPUT HEX OF UNSIGNED INT (32-bit)
void ohw(int);
// OUTPUT LINE FEED CHARACTER
void ocrlf(void);

void ctlb(char);
void ctlw(int);
void ctls(const char*);

void treew(int);
int tree1(int*);
int tree2(int*);
int tree3(int*);
int tree4(int*);
int tree5(int*);
int treev(int, int*);
void tree3z(void);

void dumpsym(void);
void objb(char);
void objw(int);
void objs(char*);
void objtype(char, int);

#endif // _C7_H
