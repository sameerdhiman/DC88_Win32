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
	    char stagcl;                    /* Stag Class */
	    unsigned int schain;            /* Stag Chain */
	    unsigned short staglen;         /* Stag Length */
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
        unsigned short noff;        /* offset */  //Fixed: 00188.c (int to short)
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
