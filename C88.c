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

/*  C88 COMPILER PASS1	-	C88.C  */

/* System Include files */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/* Local Include files */
#include "C88.h"
#include "C2.h"
#include "C3.h"
#include "C4.h"
#include "C5.h"
#include "C6.h"
#include "C7.h"
#include "PASS1.h"
#include "GLBVAR.h"
#include "NODES.h"
#include "OBJ.h"

/* Private Prototypes */
void init(int, char**);
void addres(char, char*);
void endit(void);
void global(void);
void newfun(char*, int, int);
char* newstru(void);
void newenum(void);
void doparms(void);


/* Constant Variables */
const char version[]={"C88 Compiler V3.1h  Copyright Mark DeSmet, 1982-1989\n"};
const char mmonths[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

/* ASCII Table */
char ltype[128] = {34,34,34,34,34,34,34,34,34,SPACE,21,34,SPACE,SPACE,34,34,34,
                   34,34,34,34,34,34,34,34,34,20,34,34,34,34,34,3,4,5,34,34,6,7,
                   8,34,11,9,10,11,12,13,14,DIGIT,DIGIT,DIGIT,DIGIT,DIGIT,DIGIT,
                   DIGIT,DIGIT,DIGIT,DIGIT,34,34,15,16,17,34,34,LETTER,LETTER,
                   LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,
                   LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,
                   LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,34,
                   34,34,18,LETTER,34,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,
                   LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,
                   LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,LETTER,
                   LETTER,LETTER,LETTER,LETTER,34,19,34,34,34};

/* Global Variables */
char nxtProg[70] = {"Z:GEN "};

static char pNest;

/* Contiguous memory block for line and file buffer */
//int total_elements = LINESIZE + FILESIZE + SAVEEND + MACROEXP + MACROEND;
char inFileBuf[LINESIZE + FILESIZE + SAVEEND + MACROEXP + MACROEND];
char *lineBuf = inFileBuf;
char *fileBuf = &inFileBuf[LINESIZE];
char *savEnd = &inFileBuf[LINESIZE + FILESIZE];
char *macExp = &inFileBuf[LINESIZE + FILESIZE + SAVEEND];
char *macEnd = &inFileBuf[LINESIZE + FILESIZE + SAVEEND + MACROEXP];

//Initialize structures
stOperand *struopr = NULL;
stStag *strutag = NULL;
stDefined *strudef = NULL;
stDefparm *strudparm = NULL;
uniData *unibytwrd = NULL;
stArgstack *stAStk = NULL;      //Fixed: 00138.c

int main(int argc, char *argv[])
{
    init(argc, argv);

    while(curch != CONTZ){
        ntree=1;
		global();
		if (ntree > 1) {
			ctlb(3);		/* flag flush of tree */
			ntree=1;
		}
    }

    endit();

    //free(sofmem);
    //fclose(srcFile);
    //fclose(treeFile);
    //fclose(controlFile);
    return 0;
}

void init(int argc, char **argv) {
    // char *intname; //intermediate filename CTEMP1 and CTEMP2
    char option, cppopt=0;
    int optGEN = 6, i = 0;

    if(argc < 2) {
        os("No input file.");
        real_exit(2);
    }

    if(argv[1][0] == '-' && toupper(argv[1][1]) == 'V') {
        os(version);
        real_exit(0);
    }

    cur = argv[1];
    while(*cur && *cur != '.'){
        objFilename[i++] = *cur;
        nxtProg[optGEN++] = toupper(*cur++);
    }

    /* RnD purpose: allocate 1024 bytes memory */
    sofmem = mfree = (void*)malloc(HEAPMEM);
    uintptr_t memmax = (uintptr_t)sofmem + (HEAPMEM - 1);
    stAStk = (stArgstack*)malloc(sizeof(stArgstack));   //Fixed: 00138.c
    maxmem = memmax;

    if(mfree == NULL){
        os("Unable to allocate memory");
        real_exit(2);
    }

    addres(0,"auto.break.case.char.const.continue.default.do.double.else.enum.");
	addres(11,"extern.float.for.goto.if.int.long.noalias.register.return.short.");
	addres(22,"signed.sizeof.static.struct.switch.typedef.union.unsigned.");
	addres(30,"void.");
	*nameat=RCHAR;	/* make VOID same as CHAR	*/
	/* ANSI extended keywords */
	addres(31,"volatile.while.__LINE__.__FILE__.__DATE__.__TIME__.__STDC__.");
	addres(38,"asm.cdecl.far.fortran.huge.interrupt.near.pascal.");

	cur = "__DESMET__";
    add_define();

    macxp=macExp;

    /* place for date and time */
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    /* Formats buffer layouts accurately (HH:MM:SS) and (MMM DD YYYY) */
    strftime(mtime, sizeof(mtime), "%H:%M:%S", timeinfo);
    strftime(mdate, sizeof(mdate), "%b %d %Y", timeinfo);

#if LIMITED == 0
	for (optGEN=2; optGEN < argc; optGEN++) {
		strcat(nxtProg," ");
		cur = argv[optGEN];
		if (*cur == '-') cur++;		/* allow leading - on options	*/
		option=toupper(*cur);
		if (option != 'I' && option != 'N' && option != 'P')
			strcat(nxtProg,(char*)cur);
		do {
			cur++;
			switch (option) {
				case 'A':	aopt=1;
							break;
				case 'B':	is_big=1;
							/*	add LARGE_CASE to symbol table	*/
							cur = "LARGE_CASE";
							add_define();
							cur = "__LARGE__";
							add_define();
							break;
				case 'C':	copt=1;
							break;
				case 'D':	nxtProg[0]=toupper(*cur++);
							break;
				case 'E':	cppopt=1;	/* Local Flag */
							break;
				case 'I':	strcpy(istring, cur);
							if (istring[1] == 0) {
								istring[1]=':';
								istring[2]=0;
								}
							*cur=0;
							break;
				case 'N':	add_define();
							goto next_arg;
							break;
				case 'O':	strcpy(objFilename,(char*)cur);
							*cur=0;
							break;
				case 'P':	while(*cur)
								switch(toupper(*cur++)) {
									case 'T':	trigraphs++;
												break;
									case 'W':	wopt=1;
												break;
									case 'X':	xkwd=1;
												__stdc=0;
												break;
									}
							break;
				case 'S':	soopt=1;
							break;
				case 'T':	topt=toupper(*cur++);
							break;
				case 'V':	while(*cur)
								switch(toupper(*cur++)) {
									case 'E':	eopt=1;
												break;
									case 'S':	sopt=1;
												break;
									}
							break;
				case 'X':	xopt=1;
			} /* Switch end */
		} while (*cur); /* Do-While End */
next_arg:;
	} /* For loop end */
#endif

    if(!is_big){
        cur = "__SMALL__";
        add_define();
    }
	i=0;
	while (objFilename[i] && objFilename[i] != '.') i++;		/* Read file name upto . separator */
	if (objFilename[i] == 0) strcat(objFilename,".o");			/* Append .O extension to output file name */
	c_type[0]=CCHAR;
	int_type[0]=CINT;
	u_type[0]=CUNSG;
	l_type[0]=CLONG;
	ff_type[0]=CFLOAT;
	fd_type[0]=CDOUBLE;
	p_type[0]=PTRTO; p_type[1]=CCHAR;

/* --- open file --- */
    strcpy(name,argv[1]);
	for (i=0; i < 63; i++) {
		if (name[i] == 0) {
			strcpy(&name[i],".c");
			break;
		}
		if (name[i] == '.' && name[i+1] != '.') break;
    }
    if ((file = fopen(name,"r")) == NULL) {
		os("Can not open: ");
		os(argv[1]);
		real_exit(2);
	}

    /* IF pre-processor option (-E) is passed */
	if(cppopt) {
        // not implemented yet
	}

	os(version);

    if(!soopt) {

#if CHECK
		if (copt) {
			if ((objFilenum = fopen(objFilename, "wb")) == NULL) {
				os("cannot create ");
				os(objFilename);
				real_exit(2);
			}
			inobj=&objbuf[0];
			if (is_big) objb(OBIG);
			objb(ONAME);
			objs(name);
		}
#endif

		/*	create or open temporary files.	*/

		if ((treeFile=fopen("CTEMP2", "wb")) == NULL) {
			os("cannot create CTEMP2");
			real_exit(2);
		}
		intree=treebuf;

		if ((controlFile=fopen("CTEMP1", "wb")) == NULL) {
			os("cannot create CTEMP1");
			real_exit(2);
		}
		inctl=ctlbuf;
	}
	uintptr_t memx = (uintptr_t)mfree;
	minmem=hiwater=memx;
	bufEnd=savEnd;
	cur=nestfrom[0]=bufEnd-1;
	*cur=LF;
	lineinc=1;
	dolf(0);
	tokit();
}

void addres(char nres, char *res){
    cur = res;

    while(*cur){
        tokit();
        newname();
        *nameat++=RESERVED;
		*nameat=++nres;
		mfree=nameat+1;
        cur++;
    }
}

void endit(void) {
    unsigned int isize;

    #if 0
	int i;

	for (i=0; i < 32; i++) {
		char *p;
		uniData *ptr;
		for(p=hash[i]; p; p = (char*)ptr->word){
			char *q = p + sizeof(int);
			while(*q++) ;
			if(*q == STREF) {
				os("undefined structure ");
				*(p + sizeof(int)) &= 0x7F;
				os(p + sizeof(int));
				ocrlf();
				nerr++;
			}
			uniData *ptr = (uniData*)p;
		}
	}
#endif // 0

	if(!soopt) {
#if CHECK
		if (sopt) dumpsym();
		if (copt) {
			objb(ODSEG);	/* re-establish data default	*/
			if (inobj-objbuf && fwrite(objbuf, sizeof(char), inobj-objbuf, objFilenum) == -1) {
				os("cannot write ");
				os(objFilename);
				real_exit(2);
			}
			fclose(objFilenum);
			if (nerr) unlink(objFilename);
		}
#endif

		if (nerr == 0) {
			ctlb(0);
			ctlw(nwarn);
			ctlw((hiwater-minmem)/((maxmem-minmem)/100));
			if (fwrite(ctlbuf, sizeof(char), inctl-ctlbuf, controlFile) == -1) {
				os("cannot write temporary");
				real_exit(2);
			}
			fclose(controlFile);

			isize = (unsigned int)intree;
			if (fwrite(treebuf, sizeof(char), isize-(int)treebuf, treeFile) == -1) {
				os("cannot write temporary");
				real_exit(2);
			}
			fclose(treeFile);

/*	if x option is off, use _chain to trigger gen.	*/
#if 0
			if (xopt == 0) {
				if (have_asm == 0 && aopt == 0) {
					strcat(nextpgm," Z");
				}
				if (nextpgm[0] == 'Z') _chain(&nextpgm[2]);
				else _chain(nextpgm);
			}
#endif // 0
            fclose(file);
            free(stAStk);
            free(sofmem);
            for(int i = 0; i <= (MAXINC - 1); i++){
                free(incPara[i]);
            }
			real_exit(0);					/* otherwise just EXIT	*/
		}
		else {
			fclose(controlFile);
			fclose(treeFile);
			fclose(file);
			free(stAStk);
            free(sofmem);
            for(int i = 0; i <= (MAXINC - 1); i++){
                free(incPara[i]);
            }
		}

		if (nwarn) {
			os("Number of Warnings = ");
			onum(nwarn);
			os("    ");
		}
		os("Number of Errors = ");
		onum(nerr);
		real_exit(2);
	}
	else real_exit(0);
}

void global(void){
    char *oldfree, *oldcur = NULL, oldch;   //*fundef,
	int  funord,oldnerr,funline;
	char gotlname;

	if(curch == ';') {
		tokit();
		return;
    }

    specs(SEXTERN);

    if(addstor > SEXTERN && addstor != STYPEDEF) {
		error("only STATIC and EXTERN allowed at this level");
		return;
    }

    if(ifch(';')) return;

    do{
        if (addvar(0) == 0) {
			error("illegal external declaration");
			newfun(mfree,0,0);
			return;
		}
		was_ext=0;
		atype(addtype);

#if CHECK
		if (copt && addstor != STYPEDEF){
            struopr = (stOperand*)addat;
            objtype(OPTYPE,struopr->noff);
		}
#endif
		if (copt) funline=cline; else funline=0;

        struopr = (stOperand*)addat;
		if (struopr->ntype[0] == FUNCTION) {
			//fundef=addat;                 //Set but not used
			funtype=struopr->ntype[1];		/* needed to force ret type */
			fun_ret_len=0;

			if (funtype == CSTRUCT) {
                struct_type = (void*)&struopr->ntype[1];
                fun_ret_len = struct_type->ststagat->staglen;
                pbytes=is_big ? 4 : sizeof(char*);
            }
			else pbytes=0;
			if (is_big && funtype == PTRTO) ;
			else if (funtype > CDOUBLE) funtype=CUNSG;
			oldfree=mfree;
			funord=struopr->noff;
			funname=addat-struopr->nlen;
			gotlname=0;
			if ((addparm || addproto) && nested > pNest) {
				macxp = macfrom[pNest + 1];
				nested = pNest;
            }
			if (addparm) {
				gotlname=1;
#if CHECK
				if (copt) {	/* supply name for locals */
					objb(OLNAME);
					objs((char*)funname);
					}
#endif
				oldnerr=nerr;
				cur=addparm;
				lineinc=1;
				tokit();
				doparms();
				if (oldnerr != nerr)
					while (curch != '{' && curch != CONTZ) tokit();
				}

			if (addproto) {
				struct pargs *pp, **lp;

				oldcur=cur;
				oldch=curch;

				wp=(uintptr_t*)mfree;
				uintptr_t x = (uintptr_t)addat;

				*wp=(uintptr_t)protohash[(x&31)];
				protohash[(x&31)]=(char*)wp;

				*(wp+1)=(uintptr_t)addat;
				//*(lp = (void*)(wp+2)) = pp = (void*)(wp+3);   //Original

				lp = (struct pargs **)(wp + 2);
				pp = (struct pargs *)(wp + 3);
				*lp = pp;

				if(macproto) {
					cur = macproto;
					while(cur != addproto) tokit();
				}
				else cur=addproto;

				tokit();

				if(heir == RESERVED && curch == 'v' && *cur == ')')
                    goto noargs;

				do {
					if(heir == RESERVED && bvalue == RELIPSE) {
						//*lp=-1;   // ToDo needs rewriting

                        *lp = (struct pargs *)(intptr_t)-1;
						tokit();
						notch(')');
						goto eliparg;
                    }

					addloc=(char*)pp->ptype;
					specs(SUNSTOR);
					getab(1);
                    /* Needs attention here */
					//if (*addtype > 255) {
					//Added *addtype == PTRTO to fix 00089.c
					if(*addtype == ARRAY || *addtype == CSTRUCT || *addtype == PTRTO){
						char typb;

						do {
							*addloc++=typb=*addtype++;
							if (typb == ARRAY) {
								*addloc++=*addtype++;
								*addloc++=*addtype++;
								}
                        } while (typb >= FUNCTION);

						if (typb == CSTRUCT) {
                            uniData *tmploc = (uniData*)addloc;
                            uniData *tmptype = (uniData*)addtype;
							tmploc->word = tmptype->word;
							addloc+=sizeof(uintptr_t);  //addloc+=2;
                        }
                    }
					else *addloc++=*addtype;

					lp = (struct pargs **)pp;
                    pp = (struct pargs *)addloc;
                    *lp = pp;

                } while(ifch(','));

				notch(')');
noargs:			*lp = NULL;        //*lp = 0;
eliparg:		oldfree=mfree=(char*)pp;
				cur=oldcur;
				curch=oldch;
            }

			if (ifch('{')) {
				if(addproto) {
					int poff=0;
					findover=mfree-1;

					if(macproto) {
						cur = macproto;
						while(cur != addproto) tokit();
						}
					else cur=addproto;

					macproto=addproto=NULL;        //macproto=addproto=0;
					cline++;
					lineinc=0;
					tokit();

					if(heir == RESERVED && curch == 'v' && *cur == ')') {
						tokit();
						goto voidspec;
                    }

					do {
						if (specs(SPARM) == 0 || addvar(0) == 0) {
							error("bad parameter declare");
							findover=NULL;     //findover=0;
							if (addat > mfree) mfree=addat;
							break;
                        }
                        struopr = (stOperand*)addat;
						struopr->noff=poff+4+(is_big+is_big); /* 4 or 6 bytes from bp */
						atype(addtype);
						poff+=2;

						if (struopr->ntype[0] == CLONG) poff+=2;

/*	could be a structure	*/
						else if (struopr->ntype[0] == CSTRUCT) {
                            /* Temporarily disabled */
							struct_type = (void*)&struopr->ntype;
							poff+=struct_type->ststagat->staglen-2;
							if (wopt) warning("structure argument");
                        }
						else if (is_big && struopr->ntype[0] == PTRTO) poff+=2;
/*	a FLOAT is really a DOUBLE	*/
						else {
							if (struopr->ntype[0] == CFLOAT) struopr->ntype[0]=CDOUBLE;
							if (struopr->ntype[0] == CDOUBLE) poff+=6;
                        }
                    } while (ifch(','));

voidspec:			notch(')');
					notch('{');
                }

#if CHECK
				if (copt && gotlname == 0) {	/* supply name for locals */
					objb(OLNAME);
					objs((char*)funname);
					}
#endif
				/* funname is to separate statics	*/
				newfun(oldfree,funord,funline);
				funname=NULL;      //funname=0;
				return;
			}
			funname=NULL;      //funname=0;
			addproto=NULL;     //addproto=0;
		}
		else doinit();

    }while (ifch(','));

	notch(';');
}

void doparms(void){

	char *next;
	uintptr_t  *pchain, *fchain;
	unsigned int bigger;

	pchain=0;
	cur=addparm;
	addparm=0;
	findover=mfree-1;
	tokit();
	do {
		if (heir == OPERAND && nameat < findover) heir=UNDEF;
		if (heir != UNDEF) {
			error("duplicate argument");
			findover=0;
			return;
        }

		newname();

		if (pchain) *pchain=(uintptr_t)nameat;
		else fchain=(uintptr_t*)nameat;

        struopr = (stOperand*)nameat;
		pchain = (uintptr_t*)struopr->nchain;   //&stnameat->nchain;
		struopr->opcl = OPERAND;
		struopr->nstor = SAUTO;
		struopr->noff = pbytes+4+(is_big+is_big); /* 4 or 6 bytes from bp */
		pbytes+=2;					/* assume parms are two bytes each */
		struopr->ntype[0]=CINT;		/* assume parm is integer */

		for (int i=1; i <= 10; i++)		/* but leave room to grow */
			struopr->ntype[i]=0xff;

		mfree = &struopr->ntype[11];
		tokit();
    }while(ifch(','));

	if (notch(')')) return;

	while (specs(SPARM)) {
		do {
			if (curch == ';') break;	/* allow struct decls without parms*/

			if (addvar(0) == 0 || addat > mfree) {
				error("bad parameter declare");
				findover=0;
				if (addat > mfree) mfree=addat;
				return;
            }

			addparm=0;
			atype(addtype);

/*	if parm is not 2 bytes long, must adjust offsets	*/
			bigger=0;
			struopr = (stOperand*)addat;

			if (struopr->ntype[0] == CLONG) bigger=2;
/*	could be a structure	*/
			else if (struopr->ntype[0] == CSTRUCT) {
				struct_type=(void*)&struopr->ntype;
				bigger=struct_type->ststagat->staglen-2;
				if (wopt) warning("structure argument");
            }
			else if (is_big && struopr->ntype[0] == PTRTO) bigger=2;
/*	a FLOAT is really a DOUBLE	*/
			else {
				if (struopr->ntype[0] == CFLOAT) struopr->ntype[0]=CDOUBLE;
				if (struopr->ntype[0] == CDOUBLE) bigger=6;
            }

			if (bigger) {
				pbytes+=bigger;
				next=struopr->nchain;
				while (next) {
                    struopr = (stOperand*)next;
					struopr->noff+=bigger;
					next=struopr->nchain;
                }
            }
        } while(ifch(','));

		if (notch(';')) break;
    }
	findover = NULL;
#if CHECK
	if (copt) {
		while (fchain) {
			addat=(char*)fchain;
			struopr = (stOperand*)addat;
			objtype(OLTYPE,struopr->noff);
			//fchain=((void *)fchain)->nchain;
			fchain=(uintptr_t*)struopr->nchain;
        }
    }
#endif
}

void newfun(char *oldfree, int funord, int funline){
	int  pstart,rnode[3];
	unsigned retval;

#if LIMITED == 0
    #if CHECK
        if (sopt && nerr > 0) dumpsym();
	#endif // CHECK
#endif
	locoff=0;
	laststmt=0;
	pstart = compound(oldfree,1);

	if (laststmt != RET) {
		retval=0;
		if (fun_ret_len) error("must return structure");
		else if (funtype == CFLOAT || funtype == CDOUBLE) {
			warning("must return float");
			rnode[1]=oconst(0);
			rnode[0]=CAST+(funtype<<8);
			retval=tree2(rnode);
        }
		rnode[0]=RET;
		rnode[1]=retval;
		rnode[2]=tree2(rnode);
#if CHECK
		if (copt) {
			rnode[0]=STMT;
			rnode[1]=rnode[2];
			rnode[2]=retnum;
			rnode[2]=tree3(rnode);
			}
#endif
		rnode[0]=LST+2;
		rnode[1]=pstart;
		pstart=tree3(rnode);
    }

	ctlb(2);
	ctlw(funord);
	ctlw(ntree-1);
	ctlw(pstart);
	ctlw(pbytes);
	if (locoff & 1) locoff--;
	ctlw(locoff);
	ctlw(funline);
}

/* PUT ANY STORAGE TYPES INTO ADDSTOR AND A POINTER TO A TYPE IN ADDTYPE */
int specs(char defstor){
    char tempstor, *tempaddat, tempstru;
	int  got;
    struopr = (stOperand*)nameat;

    addstor = defstor;
    deftype = CINT;
    addtype = &deftype;
    got = 0;

    while (1) {
		if (heir == RESERVED) {
			got=1;
			switch (bvalue){
				case RCONST:
				case RNOALIAS:
				case RVOLATILE:
				case RCDECL:
				case RFAR:
				case RFORTRAN:
				case RHUGE:
				case RINTERRUPT:
				case RNEAR:
				case RPASCAL:
				case RELIPSE:
								break;
				case RAUTO:
				case RREGISTER:	if (addstor != SPARM) addstor=SAUTO;
								break;
				case RSTATIC:	addstor=SSTATIC;
								break;
				case REXTERN:	addstor=SEXTONLY;
								break;
				case RTYPEDEF:	addstor=STYPEDEF;
								break;
				case RCHAR:		if(*addtype != CSCHAR) *addtype=CCHAR;
								break;
				case RSHORT:
				case RINT:		if(*addtype != CUNSG && *addtype != CLONG) *addtype = CINT;
								break;
				case RSIGNED:	*addtype = CSCHAR;
								break;
				case RLONG:		if (*addtype == CUNSG) error("illegal type");
								*addtype = CLONG;
								break;
				case RUNSIGNED:	if (*addtype == CLONG) error("illegal type");
								*addtype = CUNSG;
								break;
				case RFLOAT:	*addtype = (*addtype == CLONG) ? CDOUBLE : CFLOAT;
								break;
				case RDOUBLE:	*addtype = CDOUBLE;
								break;
				case RSTRUCT:
				case RUNION:	tempstor=addstor;
								tempstru=in_stru;
								tempaddat=addat;
								stagat.stat=newstru();
								in_stru=tempstru;
								addstor=tempstor;
								addat=tempaddat;
								stagat.cstype=CSTRUCT;
								addtype = (char*)&stagat;
								return 1;
				case RENUM:		newenum();
								return 1;
				default:		return 0;
            }
			tokit();
        }
		else if (heir == OPERAND && struopr->nstor == STYPEDEF) {
			got=1;
			addtype = &struopr->ntype[0];
			tokit();
        }
		else return got;
	}
}

// ADD xxxx :  : working perfectly
char addvar(char found){
	int  val;

	if (! found) {
		if (curch == '*') {
			tokit();
			found=addvar(0);
			if (found) *addloc++=PTRTO;
			return found;
        }

		if (ifch('(')) {
			found=addvar(0);
			notch(')');
			return (addvar(found));
		}

		while(heir==RESERVED && bvalue >= RCDECL && bvalue <= RPASCAL)
			tokit();

		if (heir == UNDEF) newname();

		if (heir == OPERAND) {
            struopr = (stOperand*)nameat;
			if (addstor == SPARM) {
				if (nameat > findover) heir=UNDEF;
			}
			else if (in_stru || nameat < findover || addstor == SEXTONLY ||
				struopr->nstor == SEXTONLY || struopr->ntype[0] == FUNCTION) {
				if (addstor == SEXTERN || addstor == SEXTONLY ||
					(addstor == SSTATIC && funname == 0)) original=nameat;
				newname();
				heir=UNDEF;
            }
        }

		if (heir == UNDEF) {
			addat=nameat;
			*nameat=OPERAND;
            struopr = (stOperand*)addat;
			struopr->nstor=(addstor == SPARM) ? SAUTO : addstor;
			addloc=&struopr->ntype[0];
			found=1;
			tokit();
        }
    }

	if (found) {
		while (curch == '(' || curch == '[') {
			if (curch == '(') {		/* skip past parms if any */
				pNest = nested;
				addparm = cur;
				macproto = addproto = 0;
				tokit();
				struopr = (stOperand*)nameat;
				if((heir == OPERAND && struopr->nstor == STYPEDEF)
					|| (heir!=UNDEF && heir!=OPERAND)) {
					addproto=addparm;
					addparm=0;
					if(cur > savEnd && cur < macEnd)
						macproto=lastname;
                }
				if (curch != ')') {
					int rplvl = 1;
					while(rplvl){
						if(curch == '(')
							rplvl++;
						if(curch == ')')
							rplvl--;
						tokit();
					}
                }
				else {
					addparm=addproto=0;
					tokit();
                }
				*addloc++=FUNCTION;
            }
			else {
				tokit();
				if (heir == CONSTANT || heir == LCONSTANT || curch == '('
				    || (heir == RESERVED && bvalue == RSIZEOF)) val=constexp();
				else val=-1;
                struopr = (stOperand*)addat;
				if (addstor == SPARM && addloc == &struopr->ntype[0]) {
					*addloc++=PTRTO;
				}
				else {
					*addloc++=ARRAY;
                    unibytwrd = (uniData*)addloc;
					if (val != -1) {
						unibytwrd->word=val;
					}
					else {
						if (addstor <= SEXTERN || addstor == STYPEDEF){
                            unibytwrd->word=-1;
						}
						else {
							error("sorry, must have dimension for locals");
							unibytwrd->word=1;
						}
					}
					addloc += sizeof(uintptr_t);    //addloc+=2;
				}
				notch(']');
			}
		}
	}
	return found;
}

// ADD TYPE BYTE(S) TO END OF OPERAND TYPES  : working perfectly
void atype(char *addtype){
	char typb;
	int i;

	//if (addtype > 255)
	//Added *addtype == PTRTO to fix 00089.c
	if (*addtype == ARRAY || *addtype == CSTRUCT || *addtype == PTRTO) {
        struopr = (stOperand*)addat;
		if (addstor == SPARM && *addtype==ARRAY && addloc == &struopr->ntype[0]){
			addtype+=3;
			*addloc++=PTRTO;
        }
		do {
			*addloc++=typb=*addtype++;
			if (typb == ARRAY) {
				*addloc++=*addtype++;
				*addloc++=*addtype++;
            }
        } while(typb >= FUNCTION);

		if (typb == CSTRUCT) {
            uniData *tmploc = (uniData*)addloc;
            uniData *tmptype = (uniData*)addtype;
			tmploc->word = tmptype->word;
			addloc += sizeof(uintptr_t);         //addloc+=2;
        }
    }
	else *addloc++=*addtype;    //keep an eye here. OLD: *addloc++ = addtype

	if (mfree < addloc) {
		if (addstor <= SEXTERN) {
			if (original) {
							/* have older definition */
                stOperand *tmpori = (stOperand*)original;
                struopr = (stOperand*)addat;
				struopr->noff = tmpori->noff;
				i=0;			/* types must agree	*/

				while (1) {
                    //stOperand *tmpori = (stOperand*)original;
					typb=tmpori->ntype[i];

					if (typb != struopr->ntype[i]) {
						warning("conflicting types");
						break;
                    }

					if (typb == ARRAY) i+=2;
					else if (typb != FUNCTION && typb != PTRTO) break;
					i++;
                }

				if (struopr->ntype[0] == FUNCTION) original=0;
				else {
					if (addstor != SEXTONLY) {
						original=0;
						struopr->nstor=SSTATIC;
						was_ext=1;
                    }
                }
            }
			else {
                struopr = (stOperand*)addat;
				struopr->noff = ++ordinal;

				if (struopr->ntype[0] == FUNCTION) {
					while (addloc <= &struopr->ntype[10])
						*addloc++=0xff;
					struopr->nchain = hash[32];
					hash[32]=addat;
					ctlb(1);
					ctlb(addstor == SEXTONLY ? SEXTERN: addstor);
					ctlw(struopr->noff);
					ctls((char*)(addat-struopr->nlen));
					ctlb(INITFUN);
                }
            }
        }
		mfree=addloc;
    }
}

char* newstru(void) {
    char is_stru,*struat,bitsin;
	int  sesize;
	char *real_type, *memchain;
	extern int i, j;

	is_stru=bvalue == RSTRUCT;
	tokit();
	if (heir == OPERAND || heir == UNDEF) {
		string[0]|=0x80;
		bptr=hash[hashno];
		while (bptr) {
			j=-1;
			wp = (uintptr_t*)bptr;
			bptr += sizeof(uintptr_t);          //bptr += 2;
			while (*bptr++ == string[++j]);
			if (j > i) {
				nameat=(void*)wp;
				nameat+=i+1+sizeof(uintptr_t);  //nameat+=i+3;
				heir=*nameat;
				if (heir == STAG || heir == STREF) goto gotSTAG;
				}
			bptr = (char*)(uintptr_t)(*wp);
			}
		heir=UNDEF;
		}
gotSTAG:
	in_stru=1;
	if (heir == STAG || heir == STREF) {
		memchain=struat=nameat;
		tokit();
		}
	else {
		if (heir != UNDEF) {	/* create dummy stag	*/
            unibytwrd = (uniData*)mfree;
			unibytwrd->word=(uintptr_t)anon;
			anon=mfree;
			unibytwrd = (uniData*)(mfree+sizeof(uintptr_t));    //mfree+2
			unibytwrd->byte=CSTRUCT;
			unibytwrd = (uniData*)(mfree+1+sizeof(uintptr_t));      //mfree+3
			unibytwrd->word=(uintptr_t)(mfree+1+2*sizeof(uintptr_t));   //mfree+5
			memchain=struat=nameat=mfree+1+2*sizeof(uintptr_t);         //mfree+5
        }
		else {
			string[0]|=0x80;
			newname();
			*wp=(uintptr_t)hash[hashno];
			hash[hashno]=(void*)wp;
			memchain=struat=nameat;
			tokit();
        }
        strutag = (stStag*)nameat;
		strutag->stagcl=STREF;
		mfree=(void*)&strutag->staglen;
		mfree+=sizeof(uintptr_t);       //mfree+=2; Why?
    }

	if (ifch('{')) {			/* allow pointer to forward structure */
		if(*struat != STREF && wopt)
			warning("structure redefinition");
		*struat = STAG;

		strutag = (stStag*)struat;

		strutag->schain = strutag->staglen = 0;

		if (is_stru == 0) strutag->staglen = 2;

		bitsin=0;

		do {
			if (specs(SMEMBER) == 0) {
				error("bad member declare");
				return struat;
            }
			if (addstor != SMEMBER) {
				error("bad member storage");
				return struat;
            }
			real_type=addtype;
			do {
				if (ifch(':')) {
					if (heir != CONSTANT || wvalue > 16) {
						error("field needs constant");
						return struat;
					}
					if (wvalue == 0 || wvalue+bitsin > 16) {
						if (is_stru && bitsin){
                            strutag = (stStag*)struat;
                            strutag->staglen+=2;
                        }
						bitsin=wvalue;
					}
					else bitsin+=wvalue;
					tokit();
					continue;
				}
				if (addvar(0) == 0) {
					/* allow stags in unions	*/
					struct stag stagat;
					if (!is_stru && addtype == (char*)&stagat) {
						//sesize=stagat.stat->staglen;
						sesize = (uintptr_t)(stagat.stat = (void*)&stagat.staglen);
						if (sesize == 0) error("undefined structure");
						goto got_size;
					}
					error("bad STRUCT declare");
					return struat;
				}
				addparm=0;
				struopr = (stOperand*)memchain;
				//((void *)memchain)->nchain=addat;
				struopr->nchain = addat;
				memchain=addat;
				if (ifch(':')) {
					if (heir != CONSTANT || wvalue > 16 || wvalue == 0) {
						error("field needs constant");
						return struat;
					}
					if (wvalue+bitsin > 16) {
						if (is_stru) {
                            strutag = (stStag*)struat;
                            strutag->staglen+=2;
                        }
						bitsin=0;
					}
					addtype=(void*)(BITS+(wvalue-1)*16+bitsin);
					bitsin+=wvalue;
					struopr = (stOperand*)addat;
					strutag = (stStag*)struat;
					if (is_stru) struopr->noff=strutag->staglen;
					else struopr->noff=0;
					addloc=&struopr->ntype[0];	/* forget real type */
					atype(addtype);
					tokit();
				}
				else {
					if (bitsin) {
						if (is_stru) {
                            strutag = (stStag*)struat;
                            strutag->staglen+=2;
                        }
						bitsin=0;
					}
					atype(addtype);
#if CHECK
					if (copt) {
                        strutag = (stStag*)struat;
						objtype(OMTYPE,is_stru ? strutag->staglen: 0);
					}
#endif
                    struopr = (stOperand*)addat;
					sesize=dsize(&struopr->ntype[0]);
					strutag = (stStag*)struat;          //Fixed: 00106.c (Shifted from line 1206 to 1207)
got_size:			if (is_stru) {
						struopr->noff=strutag->staglen;
						strutag->staglen+=sesize;
					}
					else {
						if (sesize > strutag->staglen) strutag->staglen=sesize;
						struopr->noff=0;
					}
				}
				addtype=real_type;
			}
			while (ifch(','));
			notch(';');
		}
		while (! ifch('}'));
		if (bitsin && is_stru) {
            strutag = (stStag*)struat;
            strutag->staglen+=2;
        }
	}
	return struat;
}

void newenum(void){
    char *defat;
	int enumnum,num;

	enumnum=0;
	tokit();
	/*	ignore the enum name if present	*/
	if (heir == OPERAND || heir == UNDEF) {
		tokit();
	}

	/* if list of values, treat as #define x 1, #define y 2 etc.	*/
	if (ifch('{')) {
		do {
			while (ifch(','));

			if(ifch('}')) return;

			if (heir != UNDEF) {
                error_l("duplicate enum");
                return;
            }
			newname();
			defat=nameat;
			tokit();
			if (ifch('=')) enumnum=constexp();
			strudef = (stDefined*)defat;
			strudef->defcl=DEFINED;
			strudef->dargs=255;
			defat=&strudef->dval;
			*defat++=DEFSTR;
			/*	add number of the enum	*/
			num=enumnum++;
			if (num > 9999) *defat++=num/10000+'0';
			if (num > 999) *defat++=num/1000+'0';
			if (num > 99) *defat++=num/100+'0';
			if (num > 9) *defat++=(num/10)%10+'0';
			*defat++=num%10+'0';
			*defat++=LF;
			*defat=DEFEND;
			mfree=defat+1;
		} while (! ifch('}'));
	}
}
