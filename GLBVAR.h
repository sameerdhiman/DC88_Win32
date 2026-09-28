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

/*  C88 COMPILER PASS1	-	GLBVAR.H  */

#ifndef _GLBVAR_H
#define _GLBVAR_H

#include <stdio.h>
#include <stdint.h>
/*
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
*/

#define	CHECK	1			/*	true if generating checkout compiler */
#define LIMITED 0
#define MAXPATH 256
#define HEAPMEM 65536

#define LINESIZE 512
#define FILESIZE 4096
#define SAVEEND 2
#define MACROEXP 4096
#define MACROEND 1

#define MAXFILENAME 65

/* SEE variables */
//char see_exit = 0;   // Disable see

/* Generic pointers */
char *cur, curch, *nameat;

/* Memory related variables */
char *mfree, *sofmem;
uintptr_t maxmem, minmem, hiwater;

/* Input filename related variables */
char name[MAXFILENAME];
FILE *srcFile, *treeFile, *controlFile, *file;
//unsigned int file;

/* Object check file related variables */
FILE *objFilenum;
char *inobj;
char objFilename[65];

#if	CHECK
	char objbuf[512];
#endif

/* Command line options flags */
char aopt, is_big, copt, trigraphs, wopt, xkwd, __stdc, soopt, topt, eopt, sopt,
     xopt;

/* Variables used in object file */
char c_type[1], int_type[1], u_type[1], l_type[1], ff_type[1], fd_type[1],
     p_type[2];

/* Tree and Control file related variables */
unsigned int ntree, *intree, treebuf[512];
char *intreexx, *inctl, ctlbuf[512];

/* Line processing related variables */
//char lineBuf[512], fileBuf[2048], savEnd[2];
//char macExp[2048], macEnd[1];
extern char *lineBuf, *fileBuf, *savEnd, *macExp, *macEnd;

char *macxp;
char cline, lineinc, lastline;
char newline, *lineBeg, *lineEnd, *eofptr, *bufEnd;
char *last;

/* Macro processing related variables */
char mactokn;

/* Storage related variables */
char *addat,*addtype,addstor,*addloc,*addparm,*addproto,deftype;

#define MAXINC	20
unsigned int* incPara[MAXINC];
char incnext;

#define MAXNEST 60
char *nestfrom[MAXNEST];
char *macfrom[MAXNEST], *macproto;

/* Misc variables */
unsigned int wvalue, locoff, locplus, fun_ret_len, retnum, pbytes, ordinal;
char *original, *funname, was_ext, laststmt, funtype, *lastname, blevel,
     *findover, ininit, *anon, have_asm;

/* Date and Time related variables */
char mdate[12], mtime[9];

/* Number processing */
long dvalue;
unsigned int fvalue[4];

/* Errors and Warnings */
unsigned int nerr, nwarn;

/* Unsorted variables */
char *tokat;
char string[257], istring[40];
char heir;
char macOp, *macname[MAXNEST], bvalue, in_stru;

unsigned int *wp, hashno, nested;

/* HASH TABLES */
char *hash[33], *machash[32], *protohash[32], *labhash[32], *bptr;

//static char pNest;

//#define skipl() cur=lineEnd-1

/* Structures and Unions */
typedef union {
    char byte;
    unsigned int word;
    } uniData;

typedef struct {
    unsigned int loword;
    unsigned int hiword;
    } stLoHiWrd;

struct stag {
    char cstype;
    char *stat;
    unsigned int staglen;
    } __attribute__((packed)) stagat;
//

// Anonymous structure with Direct Instance Declaration
struct {
    char sttype;
    struct stag *ststagat;
    } __attribute__((packed)) *struct_type;
//

// Procedure Arguments
struct pargs {
    struct pargs * plink;
    char ptype[1];
    } __attribute__((packed));
//

typedef struct {
	char _name[65];
	char *_cur,*_lineBeg,*_lineEnd,*_bufEnd,*_eofptr;
	char _cline, _lineinc;
	FILE *_file;
	char _lineBuf[LINESIZE],_fileBuf[FILESIZE];
} __attribute__((packed)) stSaveArea;
//

// Pseudo argument stack (addnest) //Fixed: 00138.c
typedef struct {
	    char argstk[1024];
	    char *argfrom[MAXNEST];
} __attribute__((packed)) stArgstack;

#define	SAVSIZE	sizeof(stSaveArea)
#define JUMP3 (sizeof(char) + sizeof(int))

extern uniData *unibytwrd;
extern stArgstack *stAStk;

#define argstk (stAStk->argstk)
#define argfrom (stAStk->argfrom)

#endif // _GLOBAL_H
