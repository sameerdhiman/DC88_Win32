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

/*  C88 COMPILER PASS1	-	C5.C  */

//Status: C5.c is complete

/* System Include files */
#include <string.h>

/* Local Include files */
#include "C88.h"
#include "C3.h"
#include "C4.h"
#include "C5.h"
#include "C6.h"
#include "C7.h"
#include "PASS1.h"
#include "GLBVAR.h"
#include "NODES.h"
#include "OBJ.h"

/* TYPE OF A STRING. NEEDED BY ODD CASTS. */
struct {char ary; int length; char tp;} string_type={ARRAY,0,CCHAR};

//LOCAL FUNCTION PROTOTYPES
char heir25(int*);
char heir26(int*);
int args(struct pargs*);
int lookup_member(char*);
int find_member(char*);

/* OPERATER IS MUL AND DIV (* AND /) */
char heir23(int node[]){
	int  nodeo[3],node2[2];
	char lval,bv;

	lval=heir24(node);
	while (heir == 23) {
		nodeo[0]=bv=bvalue;
		nodeo[1]=node[0];
		tokit();
		heir24(node2);
		nodeo[2]=node2[0];
		if (nodeo[1] == 0 || nodeo[2] == 0) nooper();
		node[0]=tree3(nodeo);
		if (bv != MOD) node[1]=(uintptr_t)maxtype((void*)node[1], (void*)node2[1]);
		else node[1]=(uintptr_t)nofloat((void*)node[1], (void*)node2[1]);
		lval=0;
    }
	return lval;
}

/* OPERATER IS PREFIX (* & - ! ++ --) */
/* OR SIZEOF */
char heir24(int node[]){
	int  nodeo[3],backup;
	char lval,needrp,*bptr,savenest;
	char savestor;
	unsigned saveat,saveloc,savetype;

	if (heir == RESERVED && bvalue == RSIZEOF) {
		tokit();
		needrp=0;
		backup=(uintptr_t)cur;
		savenest=nested;
		/*	save add information in case initilizing a variable */
		savestor=addstor;
		savetype=(uintptr_t)addtype;
		saveat=(uintptr_t)addat;
		saveloc=(uintptr_t)addloc;
		if (ifch('(')) {
			if (specs(SUNSTOR)) {
				node[0]=oconst(dsize(abstract(0)));
				node[1]=(uintptr_t)int_type;
				addstor=savestor;
				addtype=(void*)savetype;
				addat=(void*)saveat;
				addloc=(void*)saveloc;
				return 0;
            }
			else {
				addstor=savestor;
				cur=(void*)backup;
				nested=savenest;
				tokit();
				needrp=1;
            }
        }
/*	if SIZEOF, do not allow a logical expression.	*/
		heir24(node);
		addstor=savestor;
		addtype=(void*)savetype;
		addat=(void*)saveat;
		addloc=(void*)saveloc;
		node[0]=oconst(dsize((void*)node[1]));
		node[1]=(uintptr_t)int_type;
		if (needrp) notch(')');
		return 0;
    }

	switch(curch) {
		case '*':
			if (heir == 23) {
				tokit();
				heir24(node);
				bptr=(void*)node[1];
				if (*bptr == PTRTO) bptr++;
				else if (*bptr == ARRAY) bptr+=3;
				else {
					if (is_big || (*bptr != CINT && *bptr != CUNSG))
						error("illegal indirection");
					bptr=c_type;
					}
				node[1]=(uintptr_t)bptr;
				if (*bptr == ARRAY) return 0;
				nodeo[0]=IND+typeof_(bptr);
				nodeo[1]=node[0];
				if (nodeo[1] == 0) nooper();
				node[0]=tree2(nodeo);
				return 1;
				}
			break;
		case '&':
			if (heir == 18) {
				tokit();
				lval=heir24(node);
				bptr=(void*)node[1];
				if (*bptr == ARRAY || *bptr == FUNCTION)
					return 0;
				if (lval == 0 && *bptr != CLABEL && *bptr != CSTRUCT) {
					error("illegal address");
					return 0;
					}
				nodeo[0]=TA;
				nodeo[1]=node[0];
				if (nodeo[1] == 0) nooper();
				node[0]=tree2(nodeo);
				node[1]=(uintptr_t)abstract((void*)node[1]);
				return 0;
				}
			break;
		case '-':
			if (heir == 22) {
				tokit();
				if (heir == CONSTANT) {
					wvalue=-wvalue;
					heir24(node);
					return 0;
					}
				else if (heir == LCONSTANT) {
					dvalue=-dvalue;
					heir24(node);
					return 0;
					}
				else if (heir == FFCONSTANT) {
					fvalue[1] ^=0x8000;
					heir24(node);
					return 0;
					}
				else if (heir == FDCONSTANT) {
					fvalue[3] ^=0x8000;
					heir24(node);
					return 0;
					}
				heir24(node);
				nodeo[0]=NEG;
				nodeo[1]=node[0];
				if (nodeo[1] == 0) nooper();
				node[0]=tree2(nodeo);
				return 0;
				}
		case '+':
			if (heir == 22) {
				tokit();
				heir24(node);
				return 0;
				}
			if (heir == 24) {
				nodeo[0]=bvalue;
				tokit();
				lval=heir24(node);
				if (lval == 0) { error("need lval"); return 0; }
				nodeo[1]=node[0];
				if (nodeo[1] == 0) nooper();
				nodeo[2]=scaled((void*)node[1]);
				node[0]=tree3(nodeo);
				return 0;
				}
			break;
		case '!':
			if (heir == 24) {
				if (heir == CONSTANT) {
					wvalue=!wvalue;
					heir24(node);
					return 0;
					}
				nodeo[0]=NOT;
				tokit();
				lval=heir24(node);
				nodeo[1]=node[0];
				if (nodeo[1] == 0) nooper();
				node[0]=tree2(nodeo);
				node[1]=(uintptr_t)int_type;
				return 0;
				}
			break;
		case '\176':
				nodeo[0]=COMP;
				tokit();
				lval=heir24(node);
				nodeo[1]=node[0];
				if (nodeo[1] == 0) nooper();
				node[0]=tree2(nodeo);
				node[1]=(uintptr_t)nofloat((void*)node[1],(void*)node[1]);
				return 0;
		default: ;
    }
	lval=heir25(node);
	if (heir == 24 && (curch == '+' || curch == '-')) {
		nodeo[0]=bvalue+2;
		tokit();
		if (lval == 0) { error("need lval"); return 0;};
		nodeo[1]=node[0];
		if (nodeo[1] == 0) nooper();
		nodeo[2]=scaled((void*)node[1]);
		node[0]=tree3(nodeo);
		lval=0;
    }
	return lval;
}

/* LOOK FOR A PRIMARY */
char heir25(int node[]){
	char lval,*bptr,really_stru;
	int  node2[2],nodeo[3],scal; //,struct_len;

	lval=heir26(node);
	if (heir == 25 && node[1] == CSTRUCT) {	/* need offset of struct */
		node2[1]=node[0];
		node2[0]=TA+(is_big ? (PTRTO << 8) : (CUNSG << 8));
		node[0]=tree2(node2);
    }

	really_stru=0;
	while (1) {
		if (ifch('[')) {
			heir11(node2);
			if (node2[0] == 0) nooper();
			if (is_big && node2[1] == PTRTO) error("illegal index");
			if (notch(']')) return 0;
			bptr=(void*)node[1];
			if ((scal=scaled(bptr)) != 1) scale(node2,scal);
			nodeo[0]=ADD;
			nodeo[1]=node[0];
			nodeo[2]=node2[0];
			node[0]=tree3(nodeo);
			if (*bptr == ARRAY) bptr+=3;
			else if (*bptr == PTRTO) bptr++;
			else {
				if (is_big)	error("illegal indirection");
				bptr=c_type;
            }
			node[1]=(uintptr_t)bptr;
			lval=0;
			really_stru=0;
			if (*bptr != ARRAY && *bptr != CSTRUCT) {
				nodeo[0]=IND+typeof_(bptr);
				nodeo[1]=node[0];
				node[0]=tree2(nodeo);
				lval=1;
            }
			else if (*bptr == CSTRUCT) really_stru=1;
        }
		else if (heir == 25) {
			tokit();
			struopr = (stOperand*)nameat;
			if (is_big && node[1] != PTRTO && node[1] != CSTRUCT)
				error("illegal indirection");
			if(!find_member((void*)node[1]))
				return 0;
			if (heir != OPERAND || struopr->nstor != SMEMBER)
				{ error("need member"); return 0;}
			if (struopr->noff) {
				nodeo[1]=oconst(struopr->noff);
				nodeo[0]=ADD;
				nodeo[2]=node[0];
				node[0]=tree3(nodeo);
            }
			lval=0;
			really_stru=0;
			if (struopr->ntype[0] != ARRAY && struopr->ntype[0] != CSTRUCT) {
				nodeo[0]=IND+typeof_(&struopr->ntype[0]);
				nodeo[1]=node[0];
				node[0]=tree2(nodeo);
				lval=1;
            }
			else if (struopr->ntype[0] == CSTRUCT) really_stru=1;
			node[1]=(uintptr_t)&struopr->ntype[0];
			tokit();
        }
		else if (curch == '(') {
			bptr=(void*)node[1];
			if (*bptr == FUNCTION) bptr++;
			else if (*bptr == PTRTO && *(bptr+1) == FUNCTION) bptr+=2;
			else {
				if (is_big) error("illegal indirection");
				else warning("indirect call");
            }
			nodeo[0]=CALL+typeof_(bptr);
			nodeo[1]=node[0];
			for(wp=(uintptr_t*)protohash[(uintptr_t)nameat&31]; wp; wp=(uintptr_t*)(*wp))
				if(*(wp+1)== (uintptr_t)nameat) {
					nodeo[2]=args((void*)(*(wp+2)));
					goto pfmt;
                }
			int xargs = -1;
			nodeo[2]=args((void*)xargs);
pfmt:		struct_type=(void*)bptr;
			nodeo[3]=struct_type->sttype == CSTRUCT ?
				struct_type->ststagat->staglen : 0;
			node[0]=tree4(nodeo);
			if (bptr != (char*)node[1]) node[1]=(uintptr_t)bptr; else node[1]=(uintptr_t)int_type;
			really_stru=0;
			lval=0;
        }
		else break;
    }

	/* if found a structure as an element of another structure or array,
		need to take contents of.	*/
	if (really_stru) {
		nodeo[1]=node[0];
		nodeo[0]=IND+(CSTRUCT << 8);
		node[0]=tree2(nodeo);
    }
	return lval;
}

/* OPERAND OR CONSTANT OR STRING */
char heir26(int node[]){
	int  nodeo[5],save,dim;
	char lval,*typad,slen,savenest,oldstor,toked,tp,*oldtype;

	if(ifch('(')) {
		oldstor=addstor;
		oldtype=addtype;
		if (specs(SUNSTOR)) {	/* check for a cast	*/
			save=(uintptr_t)abstract(0);
/*	dont allow a cast of a logical expression	*/
			lval=heir24(node);
			if (node[0] == 0) nooper();
			tp=(char)node[1];
			node[1]=node[0];
			node[0]=CAST+typeof_((void*)save);
			if (is_big && lval) {

				if (node[0] == CAST+(PTRTO << 8) && tp != PTRTO &&
				tp != CSTRUCT && tp != ARRAY && tp != FUNCTION)
					error("illegal indirection");
            }
			node[0]=tree2(node);
			node[1]=save;
        }
		else {
			addtype=oldtype;
			lval=heir11(node);
			notch(')');
        }
		addstor=oldstor;
		return lval;
    }

	toked=0;
	if (heir == UNDEF) {
		newname();
		toked=1;
		tokit();
		if (ininit) {
			toked=0;
			wp=(uintptr_t*)mfree;		/*	remove the hash pointer for this guy	*/
			hash[hashno]= (char*)(uintptr_t)(*wp);
			error("undefined variable");
        }
		else if (curch == '(') {
            struopr = (stOperand*)nameat;
			addstor=struopr->nstor=SEXTERN;
			addat=nameat;
			struopr = (stOperand*)addat;
			struopr->ntype[0]=FUNCTION;
			*addtype=CINT;
			addloc=&struopr->ntype[1];
			atype(addtype);
#if	CHECK
			if (copt) objtype(OPTYPE,struopr->noff);
#endif
        }
		else {			/* must be a local integer */
			warning("undefined variable");
			struopr = (stOperand*)nameat;
			struopr->nstor=SAUTO;
			locoff-=2;
			struopr->noff=locoff;
			struopr->ntype[0]=CINT;
			mfree=&struopr->ntype[1];
        }
		goto nowop;
    }

    struopr = (stOperand*)nameat;
	if (heir == OPERAND && struopr->nstor != STYPEDEF) {

nowop:	nodeo[1]=nodeo[2]=nodeo[3]=0;
        struopr = (stOperand*)nameat;
		if (struopr->nstor < SAUTO) nodeo[1]=struopr->noff;
		else nodeo[2]=struopr->noff;

		typad=&struopr->ntype[0];

		if (toked == 0) tokit();

		while (curch == '.' || curch == '[') {
			if (curch == '.') {
				if (*typad != CSTRUCT) {
					if (*typad != PTRTO) {
                        error("need structure");
                        return 0;
                    }
					break;
                }

				tokit();
				find_member(typad);

				struopr = (stOperand*)nameat;
				if (heir != OPERAND || struopr->nstor != SMEMBER) {
					error("need member");
					return 0;
                }
				/* if a zero offset, set flag for could be member of union */
				if (struopr->noff) nodeo[2]+=struopr->noff;
				else nodeo[3]=1;

				typad=&struopr->ntype[0];
				tokit();
            }
			else {
				if (*typad != ARRAY) break;
				while (*cur == LF && cur > savEnd) {
					cur=nestfrom[--nested];
					whitesp();
                }
				save=(uintptr_t)cur;
				savenest=nested;
				tokit();
				if (heir == CONSTANT) {
					dim=wvalue;
					tokit();
					if (ifch(']')) {
						typad+=(1 + sizeof(int));       //typad+=3;
						dim=wvalue;
						nodeo[2]+=dsize(typad)*dim;
						continue;
                    }
                }

				cur=(void*)save;
				nested=savenest;
				curch='[';
				break;
            }
        }
		nodeo[0]=OPND+typeof_(typad);
		node[0]=tree4(nodeo);
		node[1]=(uintptr_t)typad;
		if (*typad < BITS) {
			if (*typad < CLABEL) return 1;
        }
		else if (*typad < FUNCTION || *typad > ARRAY) return 1;
		return 0;
    }
	if (heir == CONSTANT) {
		node[0]=oconst(wvalue);
		node[1]=wvalue > 255 ? (uintptr_t)int_type : (uintptr_t)c_type;
		tokit();
		return 0;
    }
	if (heir == LCONSTANT) {
		nodeo[0]=CONST+(CLONG<<8);
		nodeo[1]=dvalue;
		stLoHiWrd *stdvalue = (stLoHiWrd*)dvalue;
		//nodeo[2]=((void *)&dvalue)->hiword;
		nodeo[2]=(uintptr_t)&stdvalue->hiword;
		node[0]=tree3(nodeo);
		node[1]=(int)l_type;
		tokit();
		return 0;
    }
	if (heir == FFCONSTANT) {
		nodeo[0]=CONST+(CFLOAT<<8);
		nodeo[1]=fvalue[0];
		nodeo[2]=fvalue[1];
		node[0]=tree3(nodeo);
		node[1]=(int)ff_type;
		tokit();
		return 0;
		}
	if (heir == FDCONSTANT) {
		nodeo[0]=CONST+(CDOUBLE<<8);
		nodeo[1]=fvalue[0];
		nodeo[2]=fvalue[1];
		nodeo[3]=fvalue[2];
		nodeo[4]=fvalue[3];
		node[0]=tree5(nodeo);
		node[1]=(int)fd_type;
		tokit();
		return 0;
		}
	if (heir == STRNG) {
		nodeo[0]=OPND+(ARRAY<<8);
		nodeo[1]=++ordinal;
		nodeo[2]=nodeo[3]=0;
		node[0]=tree4(nodeo);
		node[1]=(uintptr_t)&string_type;
		ctlb(1);
		ctlb(SSTATIC);
		ctlw(ordinal);
		ctlb(0);			/* name is null */
concatstring:
		ctlb(INITSTR);
		slen=0;
		while (string[(unsigned char)slen] || string[(unsigned char)slen+1] != 0xff) slen++;
		string_type.length=slen;
		ctlb(slen);
		extern int i;
		i=0;
		while (i < slen) ctlb(string[i++]);
		tokit();
		if(heir==STRNG) goto concatstring;
		ctlb(0xFF);
		ctlb(INITEND);
		return 0;
		}
	node[0]=0;
	return 0;
}

int args(struct pargs *ap) {
	int  n;
	int  list[7],node[2],nodeo[4],val;
	char *ntype;

	tokit();
	n=7;
	if (ifch(')')) {
		if(ap != NULL && ap != (struct pargs *)(intptr_t)-1)     //if(ap!=0 && ap!=-1)
			error("not enough arguments");
		return 0;
    }
	do {
		if(ap == NULL) {
			error("too many arguments");
			return 0;
        }
		val=heir12(node);
		if (node[0] == 0) {
			error("missing argument");
			return 0;
        }
		if(ap != (struct pargs *)(intptr_t)-1) {
			if(typeof_(ap->ptype) != typeof_((void*)node[1])
			&& ap->ptype[0] < ARRAY && *((char*)node[1]) < ARRAY) {
				if(wopt) warning("argument type conversion");
				ntype=(void*)node[1];
				node[1]=node[0];
				node[0]=CAST+typeof_(ap->ptype);
				if (is_big && val) {

					if (node[0] == CAST+(PTRTO << 8) && *ntype != PTRTO &&
					*ntype != CSTRUCT && *ntype != ARRAY && *ntype != FUNCTION)
						error("illegal indirection");
					}
				node[0]=tree2(node);
				node[1]=(uintptr_t)ap->ptype;
            }
			ap=ap->plink;
        }
		ntype=(void*)node[1];
		if (*ntype == CSTRUCT) {
			if (wopt) warning("structure argument");
			 /* need address of struct */
			nodeo[0]=TA+(is_big ? (PTRTO << 8) : (CUNSG << 8));
			nodeo[1]=node[0];
			nodeo[1]=tree2(nodeo);
			nodeo[0]=MOVE;
			nodeo[2]=0;
			struct_type=(void*)ntype;
			nodeo[3]=struct_type->ststagat->staglen;
			node[0]=tree4(nodeo);
			}
		list[--n]=node[0];
		if (n == 1) {
			list[0]=LST+6;
			list[6]=treev(7,list);
			n=6;
        }
    }
	while (ifch(','));
	if (notch(')')) return 0;
	if(ap != NULL && ap != (struct pargs *)(intptr_t)-1) {
		error("not enough arguments");
		return 0;
    }
	if (n == 6) val=list[6];
	else {
		list[0]=LST+7-n;
		val=tree1(list);
		treev(7-n,&list[n]);
    }
	return val;
}

/*	create an abstract data type. if argument is not zero result type
	must go in table and argument points to thing address taken of.	*/

void* abstract(char *oldtype){
	char oldstor,*oldloc,*oldt,*oldat,*absat;

	oldstor=addstor;
	oldloc=addloc;
	oldt=addtype;
	oldat=addat;

    unibytwrd = (uniData*)mfree;
	//mfree->word = 0;
	unibytwrd->word = 0;
	//(mfree + 2)->byte = 0;			/* null name */
	unibytwrd = (uniData*)(mfree + sizeof(uintptr_t));
    unibytwrd->byte = 0;

	addat = mfree + 1 + sizeof(uintptr_t); //addat=mfree+3;

	struopr = (stOperand*)addat;
	struopr->opcl=OPERAND;
	struopr->nchain=0;
	struopr->nlen=1;
	struopr->nstor=addstor=SUNSTOR;
	struopr->noff=0;
	addloc=&struopr->ntype[0];

	if (oldtype) {
		*addloc++=PTRTO;
		addtype=oldtype;
    }
	else {
		getab(0);
		notch(')');
    }
	atype(addtype);
	addstor=oldstor;
	addloc=oldloc;
	addtype=oldt;
	absat=addat;
	addat=oldat;
	struopr = (stOperand*)absat;
	return (&struopr->ntype[0]);
}

void getab(char lvl){
	char otype; //char *otype;

	while (1) {
		if (ifch('*')) {
			getab(1);
			*addloc++=PTRTO;
			return;
        }
		otype=*addtype;  //otype=addtype;
		if(lvl) while((heir==RESERVED && bvalue >= RCDECL && bvalue <= RPASCAL)
					|| (heir == UNDEF) || (heir == OPERAND))
			tokit();
		if(lvl) while(specs(0)) ifch(',');
		*addtype=otype;  //addtype=otype;
		if(heir == RELIPSE) tokit();
		if (ifch('(')) {
			if (curch != ')') {
				getab(1);
				notch(')');
				continue;
            }
			tokit();
			*addloc++=FUNCTION;
        }
		else if (ifch('[')) {
			if (ifch(']')) {
				*addloc++=PTRTO;
				return;
            }
			if (heir != CONSTANT) {
				error("missing dimension");
				return;
            }
			tokit();
			notch(']');
			*addloc++=ARRAY;
			unibytwrd = (uniData*)addloc;
			unibytwrd->word=wvalue;
			addloc+=2;
        }
		else return;
    }
}

int oconst(char con){
	int nodeo[2];

	if (con > 255){
        nodeo[0]=CONST+(CINT<<8);
	}
	else nodeo[0]=CONST+(CCHAR<<8);

	nodeo[1]=con;
	return tree2(nodeo);
}

int lookup_member(char * mptr) {

	while (mptr) {
        struopr = (stOperand*)mptr;
		char *nptr = mptr - struopr->nlen;
		if (strcmp(nptr, string) == 0)
			return heir=*(nameat=mptr);
		strutag = (stStag*)mptr;
		mptr=(void*)strutag->schain;
    }
	return 0;
}

/*	FIND_MEMBER	--	find a member name. use the stag's chain list if possible,
					otherwise look for any member name.	*/

int find_member(char *typeat){
	char *typeis,*memat; //*name_at, *string_at,

	typeis=typeat;
	if (heir == OPERAND || heir == UNDEF) {
		while (*typeis == PTRTO) typeis++;
		if (*typeis == CSTRUCT) {
			struct_type=(void*)typeis;
			memat=(void*)struct_type->ststagat;		  /* address of stag */
			strutag = (stStag*)memat;
			memat=(void*)strutag->schain;		/* address of first member */
			if(lookup_member(memat))
				return 1;
        }
		else if (*typeis == CCHAR) {
			wp = (uintptr_t*)anon;
			while(wp) {
				struct_type = (void*)(wp+1);
				memat=(void*)struct_type->ststagat;		  /* address of stag */
				strutag = (stStag*)memat;
				memat=(void*)strutag->schain;	  /* address of first member */
				if(lookup_member(memat))
					return 1;
				wp = (uintptr_t*)(*wp);
            }
        }
		else {
			error("Need structure");
			return 0;
        }
    }
	error("member not in structure");
	return 0;
}
