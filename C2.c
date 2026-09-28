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

/*  C88 COMPILER PASS1	-	C2.C  */

/* System Include files */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Local Include files */
#include "C2.h"
#include "C3.h"
#include "C4.h"
#include "C6.h"
#include "C7.h"
#include "PASS1.h"
#include "GLBVAR.h"
#include "NODES.h"

/* Local function prototypes */
void preproc(void);
int skipsome(char);
int match(char*);
void adddef(void);
void doinc(void);
int initsome(char*, int, int, int);
int findfile(char*, char*);
void savstate(void);

// SKIP LINE
int skipl(void){
    cur = lineEnd-1;
    return 0;
}

// PROCESS LINEFEED : WORKING PERFECTLY
void dolf(char skipit) {
	char *mcp;
	unsigned int lsiz;

	if (cur > savEnd) {
		if(cur < macEnd)
			macxp=macfrom[nested];
		cur=nestfrom[--nested];
		whitesp();
		}
	else {
		if (addparm == 0 && addproto == 0) cline+=lineinc;
		lineinc=0;
		newline=1;			/* generate STMT node	*/
		last=lineBeg=++cur;
		if(eofptr && cur >= eofptr){
            cur = eofptr;
            return;
		}
		while(1){
			while((cur = (char*)memchr(cur, LF, bufEnd-cur))){
				lineinc++;
				if((*cur-1) == '\\') {
					memcpy(cur-1, cur+1, (bufEnd-cur)-1);
					bufEnd-=2;
					cur--;
					continue;
					}
				if(*(cur-2) == '\\'){
					memcpy(cur-2, cur+1, (bufEnd-cur)-1);
					bufEnd-=3;
					cur-=2;
					continue;
					}
				lineEnd=cur+1;
				cur=lineBeg;
				if(trigraphs) {
					char *tp;

					while((tp=cur=memchr(cur, '?', lineEnd-cur))){
						if(*++cur == '?'){
							switch(*(cur+1)){
								case '=':	*tp='#';
											break;
								case '(':	*tp='[';
											break;
								case '/':	*tp='\\';
											break;
								case ')':	*tp=']';
											break;
								case '\'':	*tp='^';
											break;
								case '<':	*tp='{';
											break;
								case '!':	*tp='|';
											break;
								case '>':	*tp='}';
											break;
								case '-':	*tp='~';
											break;
								default:	continue;
								}
							memcpy(cur, cur+2, (bufEnd-cur)-2);
							bufEnd-=2;
							lineEnd-=2;
							}
						}
					cur=lineBeg;
					}

				if (skipit == 0) {
					while (ltype[(unsigned char)*cur] == SPACE) cur++;
					if (*cur == '#') preproc();				/* Call pre-processor */
					whitesp();
                }
				return;
            } /* while end */

			if(addparm)
				mcp = addparm;
			else if(macproto)
				mcp = macproto;
			else if(addproto)
				mcp = addproto;
			else mcp = lineBeg;

			if((lsiz=bufEnd-mcp) > 512) {
				error("line too long");
				exit(2);
            }
			else
				memcpy(&fileBuf[-lsiz], mcp, lsiz);

			lineBeg=&fileBuf[lineBeg-bufEnd];

			if(addparm){
                addparm=&fileBuf[-lsiz];
            }
			else if(addproto){
                addproto=&fileBuf[-lsiz];
            }
			//if (see_exit && incnext == 0) bufEnd = fileBuf + see_call(0,fileBuf,2048);
			else {
                bufEnd = fileBuf + fread(fileBuf, sizeof(char), FILESIZE, file);
            }

			if(bufEnd < fileBuf){
				error("file read error");
            }

			if(bufEnd < savEnd) {
				*bufEnd++=LF;
				*bufEnd++=LF;
				eofptr=bufEnd;
				*bufEnd++=CONTZ;
            }
			cur = fileBuf;
        } /* while(1) end*/
    }
}

// PROCESS PRAGMA SWITCHES
void pswitch(char *sw, int def){
    //char c;

	while(ltype[(unsigned char)*cur] == LETTER) cur++;
	while(ltype[(unsigned char)*cur] == SPACE) cur++;
	if(*cur == '=') *sw = def;
	else if(*cur == '-') *sw = 0;
	else if(*cur == '!') *sw = !sw;
	else *sw = 1;
}

// PRE-PROCESSOR : WORKING PERFECTLY
void preproc(void){
    char want;
	cur++;
	whitesp();
	if(*cur == LF)
		return;
	tokit();
	if (heir != RESERVED && heir != UNDEF && heir != OPERAND)
		error_l("bad control");

	else switch(string[0]) {
		case 'a':	if (match("asm")){
						if(xkwd){
                            cur=tokat;   /* backup so statement will find ASM */
                            return;
                        }
						else error("#asm option not on");
                    }
					break;

		case 'd':	if (match("define")){
                        adddef();
						return;
                    }
					break;

		case 'e':	if (match("elif") || match("else")){
                        skipsome(0);
						return;
                    }
					else if (match("endif")){
ppexit:                 skipl();
                        return;
                    }
					else if (match("error")) {
						char *cp;

						cp = (char*)string;
						while(cur < lineEnd)
							*cp++ = *cur++;
						while(--cp >= (char*)string && *cp <= 0x20)
							;
						*(cp+1) = '\0';
						e_msg(-1,string);
						goto ppexit;
                    }
					break;

		case 'i':	if (match("include")){
                        doinc();
						return;
                    }
					else if (match("if")) {
						long cexpr;
ifproc:					mactokn=1;
						tokit();
						cexpr=constexp();
						mactokn=0;
						if (cexpr == 0)
							if(skipsome(1))
								goto ifproc;
						goto ppexit;
                    }
					else if (match("ifdef") || match("ifndef")) {
						want=match("ifdef");
						if (ltype[(unsigned char)*cur++] != LETTER)
							error_l("invalid identifier in #ifdef/#ifndef");
						else {
							find(1);
							if ((heir == DEFINED && want) ||
							(heir != DEFINED && want == 0))
								goto ppexit;
							if(skipsome(1))
								goto ifproc;
							goto ppexit;
							}
						}
					break;

		case 'l':	if (match("line")) {
						tokit();
						if (heir != CONSTANT)
							error_l("invalid digit-sequence in #line");
						else {
							cline=wvalue;
							}
						whitesp();
						if(*cur == LF)
							return;
						tokit();
						if (heir != STRNG)
							error_l("invalid string-literal in #line");
						else
							strcpy(name, string);
						goto ppexit;
                    }
					break;

		case 'p':	if (match("pragma")) {
						switch(toupper(*cur++)){
							case 'T':	pswitch(&trigraphs, 0);
										break;
							case 'W':	pswitch(&wopt, 0);
										break;
							case 'E':	if(toupper(*cur) == 'X') {
											pswitch(&xkwd, 0);
											__stdc = !xkwd;
											}
										break;
                        }
						goto ppexit;
                    }
					break;

		case 'u':	if (match("undef")) {
						if (ltype[(unsigned char)*cur++] != LETTER)
							error_l("invalid identifier in #undef");
						else {
							find(1);
							if (heir == RESERVED && bvalue >= R_LINE && bvalue <= R_STDC)
								error_l("cannot #undef predefined names");
							else if (heir != DEFINED)
								error_l("#undef identifier not defined");
							else {
                                unibytwrd = (uniData*)(nameat-2);
                                unibytwrd->byte=' ';
                            }
                        }
						goto ppexit;
						}
					break;
		}
	error_l("bad control");
}

// SKIP SOME : WORKING PERFECTLY
int skipsome(char elifOK){
    char ifnest; //, //eltype; not used -- Sameer
	ifnest=1;
	while (ifnest) {
		skipl();
		dolf(1);
		while (ltype[(unsigned char)*cur] == SPACE) cur++;
		if (*cur == CONTZ) return 0;
		if (*cur == '#' && *(cur+1) != '\r' && *(cur+1) != '\n') {
			//eltype=0;
			tokit();
			tokit();
			if (heir == RESERVED || heir == UNDEF || heir == OPERAND) {
				if (match("if") || match("ifdef") || match("ifndef"))
					ifnest++;
				else if (match("endif")) ifnest--;
				else if (elifOK && match("else") && ifnest == 1)
					ifnest=0;
				else if (elifOK && match("elif") && ifnest == 1)
					return 1;
				}
			}
		}
	skipl();
	return 0;
}

// MATCHES PASSED STRING : WORKING PERFECTLY
int match(char *mat){
	char *str;

	str=string;
	do
		if (*str != *mat++) return 0;
	while (*str++);
	return 1;
}

// ADDS DEFINE INTO SYMBOL TABLE
//Status: Partially Complete | Buggy
void adddef(void){
    char *newdef, *defat;   //*olddef,
    char lastdef, wsp=0;
    int j;

/*	do not do normal tokit as may be redefining a define	*/
	tokat=cur;
	curch=*cur++;
	if (ltype[(int)curch] == LETTER) {
		find(1);
		whitesp();
		if(strncmp(tokat, "defined", 7) == 0 ||
			(heir == RESERVED && bvalue >= R_LINE && bvalue <= R_STDC)) {
				error("Can't redefine");
				return;
		}
		if (heir == DEFINED) {		/* kill old define	*/
			*(nameat-2)='0';		/* with zero as first letter */
			//olddef=nameat;
			search(1);
		}
	}
	else {
		cur--;
		tokit();
	}

	wp = (uintptr_t*)mfree;
	*wp = (uintptr_t)machash[hashno];
	machash[hashno] = (void*)wp;
	j=0;
	nameat = (void*)(wp + 1);
	newdef = string;
	while((*nameat++=*newdef++)) j++;

	defat = nameat;
    strudef = (stDefined*)defat;
	strudef->defcl=DEFINED;
	strudef->dnlen=j+1;
	if(blevel) {
		strudef->dchain=hash[32];
		hash[32]=defat;
    }
	strudef->dargs=255;
	newdef=&strudef->dval;
	if (*cur == '(' && *(cur-1) != ' ' && *(cur-1) != '\11') {
		strudef->dargs=0;
		cur++;
		whitesp();
		if (*cur == ')') {
		cur++;
			whitesp();
			goto no_args;
			}
		if ((maxmem - (uintptr_t)mfree) < 1200) {
			error("out of memory");
			real_exit(2);
        }
		mfree+=1000;  //Why ? <-- Sameer
		do {
			whitesp();
			tokat=cur;
			curch=*cur++;
			find(1);
			whitesp();
			if (heir && heir > 4 && heir != OPERAND && heir != UNDEF) {
				error("invalid parameter");
				skipl();
				goto cleanup;
            }
			newname();
			//strudef = (stDefined*)nameat;
			//strudef->defcl=DEFPARM;
			strudparm = (stDefparm*)nameat;
			strudparm->dpcl = DEFPARM;              //Fixed: 00065.c
			strudparm->dpnum = ++strudef->dargs;
			mfree=nameat + sizeof(stDefparm);       //mfree=nameat + 2;
        }while (*cur++==',');

		if (*(cur-1) != ')') {
			error("need closing parenthesis");
			skipl();
			goto cleanup;
        }
    }

no_args:
	lastdef=0;
	whitesp();
	while (*cur != LF) {
		wsp=0;
		tokat=cur;
		if(*cur==CR) { cur++; continue; }
		if(*cur=='#') {
			if(*++cur=='#') {
				if(!lastdef) {
					error("## can't be first");
					skipl();
					goto cleanup;
                }
				if(lastdef==DEFPARM) {
					*newdef=*(newdef-1);
					*(newdef-1)=DEFCPY;
					newdef++;
                }
				*newdef++=lastdef=DEFPAS;
				cur++;
				whitesp();
				continue;
            }
			*newdef++=lastdef=DEFSFY;
			whitesp();
			continue;
        }
		switch(ltype[(int)(curch=*cur++)]) {
			case LETTER: find(1);
						if (heir == DEFPARM) {
                            strudparm = (stDefparm*)nameat;
							*newdef++=strudparm->dpnum;
							lastdef=DEFPARM;
							continue;
                        }
						else if(heir == DEFINED && nameat == defat) {
							if(lastdef == DEFSTR) newdef--;
							else *newdef++=DEFSTR;
							lastdef=DEFSTR;
							*newdef++='$';
							while (tokat < cur) *newdef++=*tokat++;
							*newdef++=LF;
							continue;
                        }
						else {
defstring:					if(lastdef == DEFSFY) {
								error("parameter must follow #");
								newdef--;
								lastdef=0;
								goto cleanup;
                            }
							if(lastdef == DEFSTR) newdef--;
							else *newdef++=DEFSTR;
							lastdef=DEFSTR;
							if(wsp && *(newdef-1) != ' ') *newdef++=' ';
							else while (tokat < cur) *newdef++=*tokat++;
							*newdef++=LF;
							continue;
                        }
			case SPACE:
defspace:				whitesp();
						if(lastdef==DEFPARM && *cur=='#' && *(cur+1) =='#')
							continue;
						wsp++;
						goto defstring;
			case 5:
			case 8:		while(*cur++ != curch) if(*(cur-1)=='\\') cur++;
						goto defstring;
			default:	if(curch == '/' && *cur == '*') {
							cur--;
							goto defspace;
                        }
						while((curch=ltype[(int)*cur]) != LETTER && curch != SPACE
						&& *cur != '"' && *cur != '\'' && *cur != LF && *cur != '/')
									cur++;
						goto defstring;
        }

    }
cleanup:
	if(lastdef == DEFPAS || lastdef == DEFSFY) {
		error("#(#) can't be last");
		newdef--;
    }
	*newdef=DEFEND;
	mfree=newdef+1;

	for (int i=0; i < 32; i++){
		while (hash[i] > mfree){
            //hash[i] = hash[i]->word;
            unsigned int *wpx = (uintptr_t*)hash[i];
            uniData *uniwpx = (uniData*)wpx;
            hash[i] = (void*)uniwpx->word;
		}
    }
}

/*	add_define  --  add a 'n' option from command tail- a define.	*/
void add_define(void){
	char *newdef;

	tokit();			/*	name of variable */
	newname();
	strudef = (stDefined*)nameat; //defat = nameat;
	strudef->defcl = DEFINED;
	strudef->dargs = 255;
	newdef = &strudef->dval;
	*newdef++ = DEFSTR;
	if (*cur == 0) {		/* if no value, assume 1	*/
		*newdef++ = '1';
	}
	else if (*cur == '=') while (++*cur) *newdef++=*cur;
	else {
		error("illegal N option ");
		exit(2);
	}

	*newdef++ = LF;
	*newdef = DEFEND;
	mfree = newdef+1;
}

// PROCESS INCLUDED FILE : WORKING PERFECTLY
void doinc(void){
	int  i;
	FILE *ifd;
	char inc_search;
	char inctemp[66];

	if (incnext == MAXINC) { error_l("include nesting too deep"); return;}
/*	accept <> around an include name and also accept white space between include and <> */
    if(*cur == ' ') whitesp();
	if(*cur!='<' && *cur!='"') {
		tokit();					/* expand macro */
		if(curch == '"'){
			inc_search=0;
			goto inc_nest;
        }
		if(curch == '<') {
			inc_search=1;
			goto inc_scan;
        }
		goto bad_inc;
    }
	if (*cur == '<') inc_search=1;
	else if (*cur == '"') inc_search=0;
	else
bad_inc: error_l("bad include");

	cur++;
inc_scan:
	i=0;
	while (*cur != '"' && *cur != '>' && i < 200) {
		if (*cur == '\n' || *cur == CONTZ) {
			error_l("unmatched \"");
			return;
        }
		string[i++]=*cur++;
    }
	if (i == 200) { error_l("string too long"); return; }
	cur++;
	string[i]=0;
inc_nest:
	skipl();

/*	supply a drive name of i option was specified	*/

	if (istring[0]) {
		if (string[1] != ':') strcpy(&string[100],string);
		else strcpy(&string[100],&string[2]);
		strcpy(string,istring);		/*	copy in -i prefix	*/
		strcat(string,&string[100]);/*	add name to end */
    }

/*	if MS-DOS V2.0 and use include= to find file */
	//extern char _msdos2;
	if (inc_search) {
		findfile(string,inctemp);
		strcpy(string,inctemp);
    }

	if ((ifd=fopen(string,"r")) == NULL) {
		os("cannot open ");
		os(string);
		ocrlf();
		real_exit(2);
    }
    // Save the state of currently opened file
	if(incPara[(unsigned char)incnext]==0 && (incPara[(unsigned char)incnext]=(uintptr_t*)malloc(SAVSIZE))==0){
            error_l("not enough #include buffer space");
	}
	else {
        //memcpy(prevname,name,sizeof(name));    //save source file name
		if(incnext==0) lastline=cline;
		//_lmove(SAVSIZE, name, _showds(), 0, incPara[incnext++]);
		savstate();
		eofptr = NULL;
		cline = 0;
		file=ifd;
		strcpy(name,string);
		bufEnd=savEnd;
		macname[nested]=0;
		cur=nestfrom[nested]=bufEnd-1;
		lineinc=1;
		*cur=LF;
		dolf(0);
    }
}

// PROCESS INITIALIZED VARIABLES
int doinit(void){
	char isstatic,*varis,*bptr;
	int  value;
    struopr = (stOperand*)addat;

	value=0;
	if (struopr->nstor == STYPEDEF || original) {
		original=0;
		return 0;
    }

	isstatic = struopr->nstor <= SEXTERN;

	if (isstatic) {
		ctlb(1);
		if (is_big) {
			if ((was_ext || struopr->nstor == SEXTERN || struopr->nstor == SEXTONLY) &&
				(struopr->ntype[0] == CSTRUCT || struopr->ntype[0] == ARRAY))
				ctlb(struopr->nstor + 128);
			else ctlb(struopr->nstor);
        }
		else ctlb(struopr->nstor);
		ctlw(struopr->noff);

/*	make the names of statics within functions distinct	*/
		if (struopr->nstor == SSTATIC && funname) {
			bptr=funname;
			while (*bptr) ctlb(*bptr++);
			ctlb('_');
        }
		ctls((char*)(addat-struopr->nlen));
    }
	varis=&struopr->ntype[0];
	locplus=0;	/* local offset from locoff */
	if (ifch('=')) {
		if (*varis == CSTRUCT && curch != '{') {
			error("need '{' for STRUCT initilization");
			return 0;
        }
		if (struopr->nstor == SEXTONLY) {
			error("cannot initilize EXTERN");
			return 0;
        }
		value=initsome(varis,1,isstatic,0);
		if (isstatic) ctlb(INITEND);
    }
	else {
		if (isstatic && struopr->nstor != SEXTONLY) {
			if (struopr->ntype[0] != FUNCTION) {
				ctlb(INITRB);
				ctlw(dsize(varis));
            }
			else ctlb(INITFUN);
        }
    }
	return value;
}

int initsome(char *varis, int num, int isstatic, int initnode){
    int  asize,nodeo[4],inode,lasttree,swant;
	char *memat,sis,sinit,itype; //,at

	do {
        //Initialize ARRAY
		if (*varis == ARRAY) {
            unibytwrd = (uniData*)(varis + 1);
			asize = unibytwrd->word;
			if (asize < 0 && num < 0) {
				error("missing dimension");
				return 0;
            }
            //uniData *univaris = (uniData*)(varis + 1 + sizeof(int));
            unibytwrd++;
			if (ifch('{')) {
				/*	allow the improper braces if string init of char */
				if (heir == STRNG && (unibytwrd->byte == CCHAR || unibytwrd->byte == CSCHAR))
					initsome(varis,1,isstatic,initnode);
				else initnode=initsome((varis+1+sizeof(int)),asize,isstatic,initnode);
				notch('}');
            }
			else if (heir == STRNG && (unibytwrd->byte == CCHAR || unibytwrd->byte == CSCHAR)) {
				if (isstatic == 0) {
					error("sorry, no string initilization of AUTO");
					return 0;
					}
				if (asize < 0){
                    unibytwrd = (uniData*)(varis+1);
                    unibytwrd->word = 0;
				}
concatstring:
				ctlb(INITSTR);
				sis=0;
				while(string[(unsigned char)sis]) sis++;
				if (asize < 0) {
                    unibytwrd = (uniData*)(varis+1);
					unibytwrd->word+=sis;
					swant=sis;
				}
				else swant=asize-1;
				if (sis > swant) sis=swant;
				if (swant > 255) {
					error("string too long");
					swant=255;
				}
				ctlb(swant);
				int i=0;
				while (i < sis)
					ctlb(string[i++]);
				while (sis++ < swant)
					ctlb(0);
				tokit();
				if(heir==STRNG) goto concatstring;
				ctlb(0xFF);
				if (asize < 0){
                    unibytwrd = (uniData*)(varis+1);
                    unibytwrd->word++; // trailing zero
				}
			}
			else {
				if (curch == ',' || curch == '}' || curch == ';') {
					if (num < 0) {
						bptr=varis-3;
						unibytwrd = (uniData*)(bptr+1);
						unibytwrd->word=-1-num;
					}
					else {
						asize=dsize(varis)*num;
						if (isstatic) {
							ctlb(INITDB);
							ctlw(asize);
						}
						else locplus+=asize;
					}
					return initnode;
				}
				initnode=initsome((varis+1+sizeof(int)),asize,isstatic,initnode);
			}
        } // end of ARRAY initialization
        // start of STRUCTURE initialization
		else if (*varis == CSTRUCT) {
            unibytwrd = (uniData*)(varis+1);
			memat=(char*)(unibytwrd->word);
            strutag = (stStag*)memat;
			memat=(char*)(strutag->schain);
			sinit=ifch('{');
			if ((curch == '}' || curch == ';') && num < 0) {
				bptr=varis-3;
				unibytwrd = (uniData*)(bptr+1);
				unibytwrd->word=-1-num;
				return initnode;
				}
			while (memat) {
                struopr = (stOperand*)memat;
				initnode=initsome(&struopr->ntype[0],1,isstatic,initnode);
				struopr = (stOperand*)memat;    //Fix: 00047.c
				memat=struopr->nchain;
				ifch(',');
            }
			if (sinit) notch('}');
        } // end of STRUCTURE initialization

		else {
			if (curch == ',' || curch == '}' || curch == ';') {
				if (num < 0) {
					bptr=varis-3;
					while (*bptr != ARRAY)
						bptr--;
                    unibytwrd = (uniData*)(bptr+1);
					unibytwrd->word=-1-num;
					}
				else {
					asize=dsize(varis)*num;
					if (isstatic) {
						ctlb(INITDB);
						ctlw(asize);
						}
					else locplus+=asize;
					}
				return initnode;
				}
			if (ifch('{')) {
				initnode=initsome(varis,num,isstatic,initnode);
				notch('}');
				return initnode;
				}
			itype=*varis;
			if (itype == PTRTO && !is_big) itype=CUNSG;
			ininit=1;	/* needed so new variable is not allowed in init list*/
			if (isstatic) {
				lasttree=ntree;	/*	re-use space for each expression */
				inode=exprnc();
				ctlb(INITVAL);
				ctlb(itype);
				ctlw(lasttree-1);/*	supply place to chop off tree	*/
				ctlw(ntree-1);
				ctlw(inode);
				ntree=lasttree;	/*	chop of the tree	*/
				}
			else {
				nodeo[0]=typeof_(varis);
				nodeo[1]=0;
				nodeo[2]=locoff+locplus;
				locplus+=dsize(varis);
				nodeo[3]=0;
				nodeo[1]=tree4(nodeo);
				nodeo[0]=ASGN;
				nodeo[2]=exprnc();
				if (initnode) {
					nodeo[2]=tree3(nodeo);
					nodeo[1]=initnode;
					nodeo[0]=LST+2;
					}
				initnode=tree3(nodeo);
				}
			ininit=0;
			}
		if (num != 1) ifch(',');
    }while (--num); //end of do-while

	return initnode;
}

// FIND THE FILE IN LOCAL OR ENV. VARIABLE I.E. DSINC : WORKING PERFECTLY
int findfile(char *filename, char *target_buf){
	FILE *fid;
	char paths[MAXPATH], *p_ptr, *t_ptr;

	/* first check in the local directory */
	strcpy(target_buf, filename);
	fid = fopen(target_buf, "r");
	if (fid != NULL){				/* got it */
		fclose(fid);
		return (1);
    }
	//fid = _environ("DSINC", paths, 256);
	//if (fid == -1) fid = _environ("INCLUDE", paths, 256);
	//p_ptr = paths;
	if((p_ptr = getenv("DSINC")) != NULL){
        strncpy(paths, p_ptr, MAXPATH - 1);
    } else {
        fprintf(stderr, "DSINC path variable not set.\n");
    }

	while (*p_ptr != 0) {
		/* copy the directory name */
		t_ptr = target_buf;
		while (*p_ptr != ';' && *p_ptr != 0) {
			*t_ptr++ = *p_ptr++;
        }
		if (*(t_ptr-1) != '/' && *(t_ptr-1) != '\\') *t_ptr++ = '\\';
		*t_ptr = 0;
		if (*p_ptr) p_ptr++;		/* beyond the ';' */
		strcat(target_buf, filename);
		fid = fopen(target_buf, "r");
		if (fid != NULL)  {				/* got it */
			fclose(fid);
			return (1);
        }
    }
	strcpy(target_buf, filename);
	return (0);							/* can't find one */
}

// SAVE THE STATE OF CURRENTLY OPENED FILE
void savstate(void){
    stSaveArea *savestate = (stSaveArea*)incPara[(unsigned char)incnext++];

    memcpy(savestate->_name, name, MAXFILENAME);
    savestate->_cur = cur;
    savestate->_lineBeg = lineBeg;
    savestate->_lineEnd = lineEnd;
    savestate->_bufEnd = bufEnd;
    savestate->_eofptr = eofptr;
    savestate->_cline = cline;
    savestate->_lineinc = lineinc;
    savestate->_file = file;
    memcpy(savestate->_lineBuf, lineBuf, LINESIZE);
    memcpy(savestate->_fileBuf, fileBuf, FILESIZE);
}

// RESTORE THE STATE OF PREVIOUSLY SAVED FILE
void resstate(void){
    stSaveArea *reststate = (stSaveArea*)incPara[(unsigned char)incnext];

    memcpy(name, reststate->_name, MAXFILENAME);
    cur = reststate->_cur;
    lineBeg = reststate->_lineBeg;
    lineEnd = reststate->_lineEnd;
    bufEnd = reststate->_bufEnd;
    eofptr = reststate->_eofptr;
    cline = reststate->_cline;
    lineinc = reststate->_lineinc;
    file = reststate->_file;
    memcpy(lineBuf, reststate->_lineBuf, LINESIZE);
    memcpy(fileBuf, reststate->_fileBuf, FILESIZE);
}
