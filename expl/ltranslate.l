%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#include "ltranslate.c"

	char * variable;
	int linecount = 2048, k, t = 0, g=0, m=0;	//g is used as a boolean variable for extra prints
	FILE *intermediate;
%}

%%
\".*\"			{
					/* Don't process "quoted" strings. */
						if (t)
							fprintf(intermediate, "%s", yytext);
				}
[a-zA-Z0-9]+[:]	{
					if (!t)
					{
						k = (int)yyleng-1;
						variable = malloc(k * sizeof(char));
						yyless(k);
						strcpy(variable, yytext);
						append_table(variable, linecount);
						linecount -= 2;
					}
					else
						g = 1;
				}

"JMP "[a-zA-Z0-9]+   {
						if (t)
						{
							k = find_map(yytext + 4);
							fprintf(intermediate, "JMP %d", k);
						}
					}
"CALL 0"			{								// This is for alloc
						if (t)
							fprintf(intermediate, "%s", yytext);
					}
"CALL "[a-zA-Z0-9]+   {
						if(t)
						{
							k = find_map(yytext+5);					// k is address
							fprintf(intermediate, "CALL %d",k);
						}
					}
"JZ R"[0-9]+[,][a-zA-Z0-9]+   {
								if (t)
								{
									if (yytext[5] == ',')
									{
										fprintf(intermediate, "JZ R%c,", yytext[4]);
										k = find_map(yytext + 6);					// k is address
										fprintf(intermediate, "%d", k);
									}
									else
									{
										fprintf(intermediate, "JZ R%c%c,", yytext[4], yytext[5]);
										k = find_map(yytext + 7);					// k is address
										fprintf(intermediate, "%d", k);
									}
								}
							}
"JNZ R"[0-9]+[,][a-zA-Z0-9]+   {
								if (t)
								{
									if (yytext[6] == ',')
									{
										fprintf(intermediate, "JNZ R%c,", yytext[5]);
										k = find_map(yytext + 7);					// k is address
										fprintf(intermediate, "%d", k);
									}
									else
									{
										fprintf(intermediate, "JNZ R%c%c,", yytext[5], yytext[6]);
										k = find_map(yytext + 8);					// k is address
										fprintf(intermediate, "%d", k);
									}
								}
							}

[\n] 	{
			if (!t)
			{
				if (linecount < 2056)
					linecount++;
				else
					linecount += 2;
			}
			if (t)
			{
				if (g)
					g = 0;
				else
					fprintf(intermediate, "\n");
			}
		}
. 		{
			if (t)
			{
				fprintf(intermediate, "%s", yytext);
			}
		}

%%

int yywrap()
{
	if (!m)
	{
		t = 1;
		m = 1;
	  	char *file2 = "assemblycode.asm";
	  	intermediate = fopen("assembly_code.asm", "w");
	  	yyin = fopen(file2, "r");
		return 0;
	}
	else
	{
		k = remove("assemblycode.asm");
		k = rename("assembly_code.asm", "assemblycode.xsm");
		return 1;
	}
}

int main()
{
	t = 0;
	char *file1 = "assemblycode.asm";
  	yyin = fopen(file1, "r");
  	yylex();
  	return 0;
}
