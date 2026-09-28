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

/*  C88 COMPILER PASS1	-	PASS1.H  */

#ifndef PASS1_H_
#define PASS1_H_

#include <stdio.h>
#include <stdint.h>

/*	LTYPE VALUES	*/
#define LETTER 1
#define DIGIT 2
#define SPACE 3

#define	DEFCPY	251         //Define Copy
#define	DEFPAS	252         //Define Paste
#define	DEFSFY	253
#define DEFSTR	254         //Define String
#define DEFEND	255         //Mark Define End

/*	symbol table types	*/

#define RESERVED	1		/* BYTE CONTAINING RESERVED VALUE */
	typedef struct {
	    char rescl;         /* Reserved Class */
	    char rvalue;        /* Reserved Value */
	} __attribute__((packed)) stReserved;

/*
	typedef struct {
        unsigned int addr;
        char *rname;
        char resclass;
        char resnum;
	} __attribute__((packed)) stRes;
*/

// Structure Tag
#define STAG		2		            /* forward chain, size of structure */
	typedef struct {
	    char stagcl;           /* Stag Class */
	    unsigned int schain;            /* Stag Chain */
	    unsigned int staglen;           /* Stag Length */
    } __attribute__((packed)) stStag;
//

#define DEFINED		3		/* #args, (arg # or DEFSTR chars... LF)
								...DEFEND	*/
	typedef struct {
	    char defcl;
	    char *dchain;
	    char dnlen;
	    char dargs;
	    char dval;
    } __attribute__((packed)) stDefined;
//

#define DEFPARM		4		/* number of argument	*/
	typedef struct {
	    char dpcl;
	    char dpnum;
    } __attribute__((packed)) stDefparm;
//

#define OPERAND		26		/* op class, forward chain of members,
							 length of name, storage, offset or ordinal */
	typedef struct {
	    char opcl;                  /* operand class */
        char *nchain;               /* forward chain of members */
        char nlen;                  /* length of name */
        char nstor;                 /* storage */
        unsigned int noff;          /* offset */
        char ntype[1];              /* ordinal means SNo.#*/
    } __attribute__((packed)) stOperand;
//
//extern stOperand *oprnode;

/*	named heir values */

#define CONSTANT	27
#define	LCONSTANT	28
#define FFCONSTANT	29
#define FDCONSTANT	30
#define STRNG		31
#define UNDEF		99
#define	STREF		100

/*	storage classes. in nstor field.	*/

#define SEXTONLY	0
#define SSTATIC		1
#define SEXTERN		2
#define SUNSTOR		3
#define SMEMBER		4
#define SAUTO		5
#define SPARM		6
#define STYPEDEF	7

#define LF			10
#define	CR			13
#define CONTZ		26

/*	the reserved words */
/*	v3.1 03/08/88 -	resorted reserved word in ANSI order */

#define RAUTO		1
#define RBREAK		2
#define RCASE		3
#define RCHAR		4
#define RCONST		5
#define RCONTINUE	6
#define RDEFAULT	7
#define RDO			8
#define	RDOUBLE		9
#define RELSE		10
#define RENUM		11
#define REXTERN		12
#define RFLOAT		13
#define RFOR		14
#define RGOTO		15
#define RIF			16
#define RINT		17
#define RLONG		18
#define	RNOALIAS	19
#define RREGISTER	20
#define RRETURN		21
#define RSHORT		22
#define RSIGNED		23
#define RSIZEOF		24
#define RSTATIC		25
#define RSTRUCT		26
#define RSWITCH		27
#define RTYPEDEF	28
#define RUNION		29
#define RUNSIGNED	30
#define RVOID		31
#define RVOLATILE	32
#define RWHILE		33

/* predefined macro names */

#define	R_LINE		34
#define	R_FILE		35
#define	R_DATE		36
#define	R_TIME		37
#define	R_STDC		38

/* extended keywords + MSC keywords */

#define RASM		39
#define	RCDECL		40
#define	RFAR		41
#define	RFORTRAN	42
#define	RHUGE		43
#define	RINTERRUPT	44
#define	RNEAR		45
#define	RPASCAL		46
#define	RDOTDOT		47
#define RELIPSE		48

extern char ltype[128];
extern stOperand *struopr;
extern stStag *strutag;
extern stDefined *strudef;
extern stDefparm *strudparm;
#endif // PASS1_H_
