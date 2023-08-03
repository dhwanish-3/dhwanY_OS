%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#include "y.tab.h"
    #include "abstree.h"
	
	union Constant number;
	char *variable, *p, var[10];
	struct Typetable *temp;
	int iterator, lineno = 1;
%}

%%

[0-9]+	{
			number.intval = atoi(yytext);
			temp = TLookup("integer");
			yylval.nptr = TreeCreate(temp, NODE_NUM, NULL, &number, NULL, NULL, NULL, NULL);
			return NUM;
		}
"main"	{return MAIN;}
"AND"	{return AND;}
"OR"	{return OR;}
"NOT"	{return NOT;}
"read"	{return READ;}
"write"	{return WRITE;}
"if"	{return IF;}
"else"	{return ELSE;}
"then"	{return THEN;}
"endif"	{return ENDIF;}
"while" {return WHILE;}
"do"	{return DO;}
"endwhile" {return ENDWHILE;}
"begin"	{return START;}
"end"	{return END;}
"decl"	{return DECL;}
"enddecl" {return ENDDECL;}
"return" {return RETURN;}

"null"	{
			temp = TLookup("void");
			yylval.nptr = TreeCreate(temp, NODE_NILL, NULL, NULL, NULL, NULL, NULL, NULL); 
			return NILL;
		}
"=="[ ]*"null" 	{
			temp = TLookup("void");
			yylval.nptr = TreeCreate(temp, NODE_NILL, NULL, NULL, NULL, NULL, NULL, NULL);
			return DEQNILL;
		}
"!="[ ]*"null" 	{
			temp = TLookup("void");
			yylval.nptr = TreeCreate(temp, NODE_NILL, NULL, NULL, NULL, NULL, NULL, NULL); 
			return NEQNILL;
		}
"int"	{return INT;}
"str"	{return STR;}
"type"	{return TYPE;}
"endtype"	{return ENDTYPE;}
"breakpoint" {return BRKP;}
"break"		{return BRK;}
"continue"	{return CONTINUE;}
"exposcall"	{return EXPOSCALL;}
"initialize" {return INIT;}
"alloc" 	{return ALLOC;}
"free" 		{return FREE;}
[a-zA-Z][a-zA-Z0-9]*	{
			variable = malloc(yyleng * sizeof(char));
			strcpy(variable, yytext);
			temp = TLookup("integer");  // default type for any word (variable or string) is int
			yylval.nptr = TreeCreate(temp, NODE_ID, variable, &yytext, NULL, NULL, NULL, NULL); 
			return ID;
		}
\"(\\.|[^\\\"])*\"	{
						variable = yytext + 1;
						iterator = 0;
						while(*variable != '"')
						{
							var[iterator] = *variable;
						 	variable++;
						 	iterator++;
						}
						var[iterator] = '\0';
						p = var;
						temp = TLookup("string");
						yylval.nptr = TreeCreate(temp, NODE_STRVAL, var, &p, NULL, NULL, NULL, NULL);
						return STRVAL;
					}
"//"(.)*	{}
"/*"([^*]|\*+[^*/])*\*+"/"	{}
[{}"]	{return *yytext;}
"<="	{return LE;}
">="	{return GE;}
"<"		{return LT;}
">"		{return GT;}
"=="	{return DEQ;}
"!="	{return NEQ;}
"="		{return ASGN;}
"-"		{return MINUS;}
"+"		{return PLUS;}
"*"		{return MUL;}
"/"		{return DIV;}
"%"		{return MOD;}
"."		{return *yytext;}
[ \t] 	{}
"["		{return *yytext;}
"]"		{return *yytext;}
[()] 	{return *yytext;}
[,;]	{return *yytext;}
[\n] 	{lineno++;}
. 		{
			yyerror("unknown character\n");
			exit(1);
		}

%%

int yywrap()
{
	return 1;
}
