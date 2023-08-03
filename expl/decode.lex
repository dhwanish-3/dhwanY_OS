%{
	#include "instr.h"
	#include "data.h"
	#define YY_INPUT(buf,result,max_size)\
	{\
		int c=fgetc(yyin);\
		result = (c==EOF) ? YY_NULL : (buf[0]=c,1);\
	}
%}

%option noyywrap

%x 	PASS1

%%

	int i;
	if (pass == 1) BEGIN (PASS1);
	if (pass == 2) BEGIN (INITIAL);

<PASS1>{

[a-z0-9]+/[\t ]*:	{
				for (i = 0; i < symtab.label_count; i++)
				{
					if (strcmp(symtab.entry[i].label, yytext) == 0)
					{
						printf("<ERROR :%d:> Label %s already used in line no %d\n", line_count, yytext, symtab.entry[i].label);
						error = 1;
					}
				}
				if (!error)
				{
					symtab.label_count++;
					strcpy(symtab.entry[symtab.label_count].label, yytext);
					symtab.entry[symtab.label_count].position = ftell(yyin) - 1;
					symtab.entry[symtab.label_count].lineno = line_count;
				}
			}
START	{
		if (started != -1)
		{
			printf("<ERROR :%d:> Duplicate start instruction, earlier at line no %d\n", started);
			error = 1;
		}
		else
		{
			started = line_count;
			start_offset = ftell(yyin) - 1;
		}
	}

.	;
\n	line_count++;
<<EOF>>	return 0;

}

	
MOV		{ yylval.flag = 0; return(MOV); }
SP 		{ yylval.flag = SP; return(0); }
BP		{ yylval.flag = BP; return(0); }
IP		{ printf("<ERROR:%d:> IP cannot be an arguement\n", line_count); exit(0);
			yylval.flag = IP; return(0); }
ADD		{ yylval.flag = ADD; return(ARITH); }
SUB		{ yylval.flag = SUB; return(ARITH); }
MUL		{ yylval.flag = MUL; return(ARITH); }
DIV		{ yylval.flag = DIV; return(ARITH); }
MOD		{ yylval.flag = MOD; return(ARITH); }
INR		{ yylval.flag = INR; return(ARITH); }
DCR		{ yylval.flag = DCR; return(ARITH); }
LT    	{ yylval.flag = LT; return(LOGIC); }
GT    	{ yylval.flag = GT; return(LOGIC); }
EQ    	{ yylval.flag = EQ; return(LOGIC); }
NE    	{ yylval.flag = NE; return(LOGIC); }
GE    	{ yylval.flag = GE; return(LOGIC); }
LE    	{ yylval.flag = LE; return(LOGIC); }
JZ 		{ yylval.flag = JZ; return(BRANCH); }
JNZ 	{ yylval.flag = JNZ; return(BRANCH); }
JMP 	{ yylval.flag = JMP; return(BRANCH); }
PUSH	{ yylval.flag = 0; return(PUSH); }
POP		{ yylval.flag = 0; return(POP); }
CALL	{ yylval.flag = 0; return(CALL); }
RET		{ yylval.flag = 0; return(RET); }
IN    	{ yylval.flag = 0; return(IN); }
OUT		{ yylval.flag = 0; return(OUT); }
HALT	{ yylval.flag = 0; return(HALT); }
R[0-7]  	{ yylval.flag = REG; yytext++; return(atoi(yytext)); }
\[R[0-7]\]	{	
				yylval.flag = MEM_REG; 
				yytext[yyleng - 1] = '\0';
				yytext = yytext + 2;	
				return(atoi(yytext)); 
			}	
\[[0-9]+\]	{
			yylval.flag = MEM_DIR;
			yytext[yyleng - 1] = '\0';
			yytext++;
			return(atoi(yytext));
		}
\[SP\]		{ yylval.flag = MEM_SP; return(0); }
\[BP\]		{ yylval.flag = MEM_BP; return(0); }
\[IP\]		{ printf("<ERROR:%d:>IP cannot be an arguement\n", line_count); exit(0);
				yylval.flag = MEM_IP; return(0); }
-?[0-9]+  	{ yylval.flag = NUM; return(atoi(yytext)); }
[\t ]*		; 
<<EOF>>		{ printf("<ERROR> HALT instruction missing\n"); exit(0); }  
\n	        line_count++;
[A-Za-z0-9]+[\t ]*:	;
\/\/.*		;
[,:]		;
[A-Za-z0-9]+	{ strcpy(yylval.data, yytext); return(LABEL);}
.		{ printf("<ERROR:%d: Unexpected symbol %s\n", line_count, yytext); exit(0); }
%%