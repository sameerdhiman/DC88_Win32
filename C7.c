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

/*  C88 COMPILER PASS1	-	C7.C  */

//Status: C7.c is complete

/* System Include files */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* Local Include files */
#include "C2.h"
#include "C6.h"
#include "C7.h"
#include "PASS1.h"
#include "GLBVAR.h"
#include "NODES.h"
#include "OBJ.h"

void listtype(void);

// OUTPUTS ERROR MESSAGES
// Status: Complete
void e_msg(int reale, const char *str){
    onum(cline);
	oc(' ');
	if (reale >= 0){
		bptr=lineBeg;
		do {
			if (bptr == tokat){
                os(" $$ ");
			}
			if (*bptr == 9){
                os("    ");
			}
			else if (*bptr != CR && *bptr != CONTZ){
                oc(*bptr);
			}
        }
		while(*bptr != CONTZ && *bptr++ != LF);
		os("      ");
    }
	if (incnext){
		os("file: ");
		os(name);
		os("    ");
    }
	if (reale > 0) {
		os(" error:");
		nerr++;
    }
	else if (reale == 0)
		os(" warning:");
	else
		os("#error:");
	os((char*)str);

	ocrlf();
}

// OUTPUT ERROR MESAGES : PRIMARY
// Status: Complete
void error(const char *str){
    if (tokat) e_msg(1,str);
	if (curch == ';' || curch == CONTZ) {
		tokat=0;
		return;
    }
	if (tokat == 0) {
		tokit();
		return;
    }
	while (1) {
		while (*cur != LF && *cur != ';' && *cur != CONTZ && *cur != '{'
			&& *cur != '}') cur++;
		if (*cur == LF) dolf(1);
		else {
			tokit();
			tokat=0;
			return;
		}
    }
}

// OUTPUT NO OPERAND
// Status: Complete
void nooper(void){
	error("missing operand");
}

// OUTPUT LINE ERROR
// Status: Complete
void error_l(const char *str){
    e_msg(1, str);
	skipl();
}

// OUTPUT WARNINGS
// Status: Complete
void warning(const char *str){
	e_msg(0,str);
	nwarn++;
}

// TYPEOF_
// Status: Complete
int typeof_(char *ptr){
	int val;

	if (*ptr == PTRTO && is_big == 0){
        val=CUNSG;
    }
	else val = *ptr;
	return (val << 8);
}

// DATA SIZE
// Status: Complete
unsigned int dsize(char *ptr){
	unsigned int  tot;

	if (*ptr == ARRAY) {
		wp = (uintptr_t*)(ptr+1);
		tot=*wp;
		if (tot == -1) tot=0;
		return (tot*dsize(ptr+1+sizeof(int)));
    }
	if (*ptr == CCHAR || *ptr == CSCHAR){
        tot=1;
    }
	else if (*ptr == CLONG || *ptr == CFLOAT){
        tot=4;
    }
	else if (*ptr == CDOUBLE){
        tot=8;
    }
	else if (*ptr == CSTRUCT) {
		wp = (uintptr_t*)(ptr+1);
		wp = (uintptr_t*)(*wp);
		strutag = (stStag*)wp;
		tot = strutag->staglen;
		if (tot == 0) error("undefined structure");
    }
	else if (is_big && *ptr == PTRTO){
        tot=4;
    }
	else if (*ptr == FUNCTION){
        return dsize(ptr+1);
    }
	else tot=2;
	return tot;
}

// ON CRITICAL ERROR EXITS THE PROGRAM
// Status: Complete
void real_exit(int cc){
    exit(cc);
}

// Status: Complete
int scaled(char *ptr){
	int val;
	if (*ptr == ARRAY){
        val=dsize(ptr+3);
    }
	else if (*ptr == PTRTO){
        val=dsize(ptr+1);
    }
	else val=1;
	return val;
}

// OUTPUT CHARACTER : working perfectly
// Status: Complete
void oc(char ch){
    putchar(ch);
}

// OUTPUT A STRING POINTED TO BY PASSED POINTER : working perfectly
// Status: Complete
void os(const char *str){
	while (*str){
         oc(*str++);
	}
}

// OUTPUT SIGNED OR UNSIGNED NUMBERS : working perfectly
// Status: Complete
void onum(int num){
    if (num < 0){
		oc('-');
		num=-num;
    }
	if (num >= 10){
        onum(num/10);
	}
	oc(num % 10 + '0');
}

// OUTPUT SIGNED OR UNSIGNED BYTE : working perfectly
// Status: Complete
void obnum(char bnum){
    int num;
    onum(num = bnum);
}

// OUTPUT HEXADECIMAL OF UNSIGNED BYTE (8-bit) : working perfectly
// Status: Complete
void ohb(char bnum){
    char hinibble = (bnum >> 4) & 0x0F;
    char lonibble = bnum & 0x0F;

    char hexhi = (hinibble < 10) ? ('0' + hinibble) : ('A' + (hinibble - 10));
    char hexlo = (lonibble < 10) ? ('0' + lonibble) : ('A' + (lonibble - 10));

    oc(hexhi);
    oc(hexlo);
}

// OUTPUT HEXADECIMAL OF UNSIGNED INT (32-bit) : working perfectly
// Status: Complete
void ohw(int inum){
    const char hex_digits[] = "0123456789ABCDEF";
    char buffer[9];
    buffer[8] = '\0';

    if(inum == 0){
        for(int i = 7; i >= 0; i--){
            buffer[i] = '0';
        }
    }
    else{
        for (int i = 7; i >= 0; i--) {
            buffer[i] = hex_digits[inum & 0xF];
            inum >>= 4;
        }
    }
    os(buffer);
}

// OUTPUT CARRIAGE RETURN LINE FEED : working perfectly
// Status: Complete
void ocrlf(void){
	oc(LF);
}

// WRITE CONTROL BYTE TO TEMPORARY FILE : working perfectly
// Status: Complete
void ctlb(char byt){
	if(!soopt){
		if (inctl == &ctlbuf[512]) {
			if (fwrite(ctlbuf, sizeof(char), sizeof(ctlbuf), controlFile) == -1){
				os("cannot write temporary");
				real_exit(2);
            }

			inctl = ctlbuf;
        }
		*inctl++ = byt;
    }
}

// WRITE CONTROL WORD 16, 32 OR 64 BIT TO TEMPORARY FILE
// Status: Complete
void ctlcalc(int wsize, int val){
    for(int i = 1; i <= (wsize - 1); i++){
        ctlb(val >> 8);
    }
}

// WRITE CONTROL WORD TO TEMPORARY FILE
// Status: Complete
void ctlw(int wrd){
	ctlb(wrd);      // Extract lowest 8-bit value
    ctlcalc(sizeof(int), wrd);
}

// WRITE CONTROL STRING TO TEMPORARY FILE : working perfectly
// Status: Complete
void ctls(const char *str){
	do
		ctlb(*str);
	while (*str++);
}

// WRITE TREE WORD TO TEMPORARY FILE
// Status: Complete
void treew(int wrd){
	if(!soopt) {
		if (intree == &treebuf[512]){
			if (fwrite(treebuf, sizeof(int), sizeof(treebuf), treeFile) == -1){
				os("cannot write temporary");
				real_exit(2);
            }
			intree=treebuf;
        }
		*intree++=wrd;
		ntree++;
    }
}

// Status: Complete
int tree1(int *branch){
	int at = ntree;

	treew(*branch);
	return at;
}

// Status: Complete
int tree2(int branch[]){
	int at = ntree;

	treew(branch[0]);
	treew(branch[1]);
	return at;
}

// Status: Complete
int tree3(int branch[]){
	int at = ntree;

	treew(branch[0]);
	treew(branch[1]);
	treew(branch[2]);
	return at;
}

// Status: Complete
int tree4(int branch[]){
	int at = ntree;

	treew(branch[0]);
	treew(branch[1]);
	treew(branch[2]);
	treew(branch[3]);
	return at;
}

// Status: Complete
int tree5(int branch[]){
	int at = ntree;

	treew(branch[0]);
	treew(branch[1]);
	treew(branch[2]);
	treew(branch[3]);
	treew(branch[4]);
	return at;
}

// Status: Complete
int treev(int num, int branch[]){
	int at = ntree;

	for (int i=0; i < num; i++)
		treew(branch[i]);

	return at;
}

// Status: Complete
void tree3z(void){
	treew(0);
	treew(0);
	treew(0);
}

#if CHECK
// DEBUG STUFF TO STDOUT
// Status: Incomplete due to bugs
void dumpsym(void) {
	char *lastt;
	//char ch;
	//int  num;

	bptr = sofmem;  // changed from bptr=_memory();

	while (bptr < mfree) {
		ohw((uintptr_t)bptr);             //output bptr address
		oc(' ');
		unibytwrd = (uniData*)bptr;
		ohw(unibytwrd->word);         //output output 4 byte chain address
		oc(' ');
		bptr+=sizeof(uintptr_t);    //bptr+=2;  //advance bptr 4 bytes
		while (*bptr)
			oc(*bptr++ & 0x7f);     //output string
		oc(' ');
		lastt=++bptr;               //place bptr at class
		switch (*bptr) {
			case RESERVED:	os("RESERVED ");
							obnum(*++bptr);
							bptr++;
							break;
			case STREF:
			case STAG:		os("STAG ");
                            strutag = (stStag*)bptr;
							ohw(strutag->schain);
							oc(' ');
							onum(strutag->staglen);
							bptr+=sizeof(stStag);        //bptr+=5;
							break;
			case DEFINED:	os("DEFINED ");
							bptr += sizeof(uintptr_t);    //bptr+=2;
							while (*++bptr != DEFEND) {
                                onum(*bptr);
                                oc(' ');
							}
                            if (*++bptr == DEFSTR) {
                                os("DEFSTR ");
                                bptr++;
                                do
                                    if (*bptr >= ' ') oc(*bptr);
                                while (*bptr++ != LF);
                            }
                            bptr++;
							break;
			case OPERAND:	os("OPERAND ");
                            struopr = (stOperand*)bptr;
							ohw((uintptr_t)(struopr->nchain));
							oc(' ');
							onum(struopr->nlen);
							oc(' ');
							onum(struopr->nstor);
							oc(' ');
							onum(struopr->noff);
							bptr += sizeof(stOperand) - 1; //bptr+=7;
							listtype();
							break;
			default:		os("Mystery Symbol ");
                            unibytwrd = (uniData*)bptr;
							ohw(unibytwrd->word);
        }
		ocrlf();
		unibytwrd = (uniData*)bptr;
		//if (unibytwrd->word == 0xffff) bptr=lastt+18;
		if (unibytwrd->word == 0xffffffff) bptr=lastt+18+sizeof(uintptr_t);
    }

	os("Normal End");
	ocrlf();
}

// Status: Incomplete due to bugs
void listtype(void) {
	char tp;

	do {
		oc(' ');
		tp=*bptr++;
		unibytwrd = (uniData*)bptr;
		switch (tp) {
			case CCHAR:		os("CCHAR");
							break;
			case CSCHAR:	os("CSCHAR");
							break;
			case CINT:		os("CINT");
							break;
			case CUNSG:		os("CUNSG");
							break;
			case CLONG:		os("CLONG");
							break;
			case CFLOAT:	os("CFLOAT");
							break;
			case CDOUBLE:	os("CDOUBLE");
							break;
			case CLABEL:	os("CLABEL");
							break;
			case CSTRUCT:	os("CSTRUCT ");
							ohw(unibytwrd->word);
							bptr+=sizeof(int);  //changed from bptr+=2;
							break;
			case FUNCTION:	os("FUNCTION");
							break;
			case ARRAY:		os("ARRAY ");
							onum(unibytwrd->word);
							bptr+=sizeof(int);  //changed from bptr+=2;
							break;
			case PTRTO:		os("PTRTO");
							break;
			default:		os("BITS ");
							onum(tp & 15);
							oc(' ');
							onum((tp >> 4)+1);
							break;
        }
    }
	while (tp >= FUNCTION);
}

// DEBUG STUFF TO THE OBJECT FILE
// Status: Complete
void objb(char byt){
	if(!soopt) {
		if (inobj == &objbuf[512]) {
			if (fwrite(objbuf, sizeof(char), sizeof(objbuf), objFilenum) == -1){
				os("cannot write temporary");
				real_exit(2);
            }
			inobj=objbuf;
        }
		*inobj++=byt;
    }
}

// Status: Complete
void objw(int wrd){
	objb(wrd);
	objb(wrd>>8);
}

// Status: Complete
void objs(char *str){
	do
		objb(toupper((*str) & 0x7f));
	while (*str++);
}

static char last_seg=ODSEG;

// Status: Complete
void objtype(char otype, int num){
	int  len;
	char *bp,shorter,ttyp,*sptr,*bptr;

	/*	must say if in data or code for statics	*/
	struopr = (stOperand*)addat;
	if (struopr->ntype[0] == FUNCTION) {
		if (last_seg != OCSEG) objb(last_seg=OCSEG);
    }
	else if (last_seg != ODSEG) objb(last_seg=ODSEG);
	objb(otype);
	bp=addat-struopr->nlen;
	if (otype != OPTYPE) objs((char*)bp);
	else {
		if (struopr->nstor == SSTATIC && funname) {
			bptr=funname;
			while (*bptr) objb(*bptr++);
			objb('_');
			}
		while (*bp) objb(toupper(*bp++));
		objb('_');
		objb(0);
		}

	objw(num);
/*	get the length of the type info and output first	*/

	len=1;
	bp=struopr->ntype;
	shorter=0;
	while (*bp >= FUNCTION) {
		if (len >= 9) {
			shorter=1;
			break;
			}
		if (*bp == ARRAY) {
			bp+=2;
			len+=2;
			}
		bp++;
		len++;
		}
	if (*bp == CSTRUCT) {
		len+=2;
		}
	objb(len+shorter);
	bp=struopr->ntype;
	while (len--) {
		objb(ttyp=*bp++);
		if (ttyp == ARRAY) {
			objb(*bp++);
			objb(*bp++);
			len-=2;
			}
		if (ttyp == CSTRUCT) {
            unibytwrd = (uniData*)bp;
			sptr=(char*)(unibytwrd->word);
			bp+=2;
            strutag = (stStag*)sptr;
			objw(strutag->staglen);
			len-=2;
			}
		}
	if (shorter) objb(CINT);
	}

#endif // CHECK
