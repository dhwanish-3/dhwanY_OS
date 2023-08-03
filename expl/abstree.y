%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "abstree.h"
    #include "symboltable.h"
    #include "symboltable.c"
    #include "aa.c"
    #include "codegen.c"
    #include "userdtype.c"
    #include "typecheck.h"
    #include "typecheck.c"

    int yylex();
    void yyerror(char const *s);
    struct ASTNode *head1, *head;
    int result;
    FILE *fp;
    extern FILE *yyin;
    extern int yylineno, lineno;
    struct Fieldlist *ftemp;
    struct Typetable *declarationType, *FdeclarationType, *functype, *temp1, *temp2;
    struct Gsymbol *Gtemp;
    struct Lsymbol *Ltemp;
    struct Paramstruct *Argtemp1, *Argtemp2;
    int indicator = 0; //for no.of arguments passed
    int declcount = 0, defcount = 0, exprcount = 0;
    int init_counter = 0, f_counter = 0;	//to check whether initialize is called or not.
    int wr = 0;
%}

%union
{
    struct ASTNode *nptr;
};

%token NUM ID PLUS MUL DIV MOD ASGN READ WRITE MINUS NEWLINE LT GT DEQ NEQ ELSE IF THEN ENDIF ENDWHILE WHILE DO
%token START END DECL ENDDECL INT STR LE GE NOT AND OR MAIN RETURN ALLOC FREE INIT BRK CONTINUE BRKP TYPE ENDTYPE
%token NILL DEQNILL NEQNILL STRVAL EXPOSCALL

%left PLUS MINUS AND OR
%left MUL DIV MOD
%right ASGN
%nonassoc LT  GT LE GE NOT
%right DEQ
%right NEQ

%type <nptr> NUM
%type <nptr> STRVAL
%type <nptr> ID
%type <nptr> PLUS
%type <nptr> MUL
%type <nptr> DIV
%type <nptr> MOD
%type <nptr> READ
%type <nptr> WRITE
%type <nptr> MINUS
%type <nptr> LT
%type <nptr> GT
%type <nptr> LE
%type <nptr> GE
%type <nptr> AND
%type <nptr> OR
%type <nptr> NOT
%type <nptr> ELSE
%type <nptr> IF
%type <nptr> THEN
%type <nptr> ENDIF
%type <nptr> WHILE
%type <nptr> DO
%type <nptr> ENDWHILE
%type <nptr> Expr
%type <nptr> Slist
%type <nptr> Stmt
%type <nptr> MAIN
%type <nptr> RETURN
%type <nptr> Body
%type <nptr> mainblock
%type <nptr> Retstmt
%type <nptr> fdef
%type <nptr> exprlist
%type <nptr> exprlist_exposcall
%type <nptr> FIELD
%type <nptr> NILL
%type <nptr> DEQNILL
%type <nptr> NEQNILL
%type <nptr> INIT
%type <nptr> ALLOC
%type <nptr> FREE
%type <nptr> BRK
%type <nptr> CONTINUE
%type <nptr> BRKP
%type <nptr> EXPOSCALL

%%
program : TypeDefBlock gdeclaration fdeflist mainblock {
			                                            fclose(intermediate);
                        								}
        | TypeDefBlock gdeclaration mainblock    {
		                                            fclose(intermediate);
                                    			 }
        ;

TypeDefBlock  : TYPE TypeDefList ENDTYPE    {}
              |                             {}
              ;

TypeDefList   : TypeDefList TypeDef
              | TypeDef
              ;

TypeDef :    ID '{' FieldDeclList '}'{
	                                    TInstall($1->name, 0, Fhead); //size is initialized to 0. actual size is calculated in TInstall
                               		}
              ;

FieldDeclList : FieldDeclList FieldDecl
              | FieldDecl
              ;

FieldDecl    : FieldType ID ';' {
	                                if(FLookup ($2->name, Fhead) != NULL)
	                                {
	                                    yyerror("Re-declaration of field element\n");
	                                    exit(1);
	                                }
						            FInstall(declarationType, $2->name);
								}

gdeclaration : DECL GDeclList ENDDECL   {
                                            start();
                                        }
                |                       {
                                            start();
                                        }
            ;

GDeclList : GDecl             {}
        | GDeclList GDecl     {}
        ;

GDecl : TypeName Gidlist ';'    {}
     ;

Gidlist : ID            {
                            verify($1, 1, 0, 0, NULL);
                            GInstall($1->name, declarationType, 1, NULL);
                        }
        |ID '['NUM']'  {
                            verify($1, 1, 0, 0, NULL);
                            install_id($1, $3, declarationType);
                        }
        | ID '(' ParamList ')'{
                                declcount++; //  to keep track of number of functions declared
                                verify($1, 1, 0, 0, NULL);
                                GInstall($1->name, declarationType, -1, Phead);
                                Phead = NULL;
                                Ptail = NULL;
                            }
        |Gidlist ',' ID     {
                                verify($3, 1, 0, 0, NULL);
                                GInstall($3->name,declarationType,1,NULL);
                            }
        |Gidlist ',' ID '['NUM']' {
                                    verify($3, 1, 0, 0, NULL);
                                    install_id($3, $5, declarationType);
                                }
        |Gidlist ',' ID '(' ParamList ')'{
                                            declcount++; //  to keep track of number of functions declared
                                            verify($3, 1, 0, 0, NULL);
                                            GInstall($3->name, declarationType, -1, Phead);
                                            Phead = NULL;
                                            Ptail = NULL;
                                        }
        ;

FieldType :     INT {declarationType = TLookup("integer");}
            |   STR {declarationType = TLookup("string");}
            |   ID  {
                        declarationType = TLookup($1->name);
                        if (declarationType == NULL)
                            declarationType = TLookup("dummy");
                    }
        ;

TypeName :      INT {declarationType = TLookup("integer");}
            |   STR {declarationType = TLookup("string");}
            |   ID  {
                        declarationType = TLookup($1->name);
                        if (declarationType == NULL)
                        {
                            yyerror("Unknown user-defined type\n");
                            exit(1);
                        }
                    }
        ;

FType :     INT {FdeclarationType = TLookup("integer");}
        |   STR {FdeclarationType = TLookup("string");}
        |   ID  {
                   FdeclarationType = TLookup($1->name);
                    if (FdeclarationType == NULL)
                    {
                        yyerror("Unknown user-defined type\n");
                        exit(1);
                    }
                }
        ;

ParamList : Param                   {}
        |   ParamList ',' Param     {}
        |                           {}
        ;

Param   :   FType fvlist        {}
        ;

fvlist  :   ID                     {
                                        verify($1, 0, 0, 1, NULL);
                                        PInstall($1->name, FdeclarationType);
                                    }
        ;

fdeflist : fdef                 {}
            | fdeflist fdef     {}
            ;

fdef : TypeName ID '(' ParamList ')' '{' LdeclBlock Body '}'    {
                                                                defcount++; //  to keep track of number of functions defined

                                                                Gtemp = GLookup($2->name);

                                                                if (Gtemp == NULL)
                                                                {
                                                                    printf("Function not declared in GST\n");
                                                                    exit(1);
                                                                }

                                                                //storing arguments in LST

                                                                Argtemp1 = Phead;
                                                                Argtemp2 = Gtemp->paramlist;

                                                                while ((Argtemp1 != NULL) && (Argtemp2 != NULL))
                                                                {
                                                                    if (strcmp(Argtemp1->name, Argtemp2->name) == 0)
                                                                    {
                                                                        if (Argtemp1->type != Argtemp2->type)
                                                                        {
                                                                            printf("Conflict in argument types \n");
                                                                            exit(1);
                                                                        }
                                                                    }
                                                                    else
                                                                    {
                                                                        printf("Conflict in argument names \n");
                                                                        exit(1);
                                                                    }

                                                                    Argtemp1 = Argtemp1->next;
                                                                    Argtemp2 = Argtemp2->next;
                                                                }

                                                                if ((Argtemp1 != NULL) || (Argtemp2 != NULL))
                                                                {
                                                                    printf("Unequal number of arguments \n");
                                                                    exit(1);
                                                                }

                                                                $2->Lentry = Lhead;

                                                                fprintf(intermediate, "F%d:\n",Gtemp->binding);
                                                                fprintf(intermediate, "PUSH BP\n");
                                                                fprintf(intermediate, "MOV BP,SP\n");

                                                                Ltemp = Lhead;

                                                                while(Ltemp != NULL)
                                                                {
                                                                    fprintf(intermediate, "PUSH R0\n");
                                                                    Ltemp = Ltemp->next;
                                                                }

                                                                codegen($8);

                                                                Phead = NULL;
                                                                Ptail = NULL;
                                                                Lhead = NULL;
                                                                Ltail = NULL;
                                                            }
     ;

LdeclBlock : DECL ldlist ENDDECL    {}
              |                     {}
            ;

ldlist : ldec             {}
         | ldlist ldec    {}
        ;

ldec : FType lvlist ';'    {}
    ;

lvlist : ID              {
                            verify($1, 0, 1, 0, NULL);
                            LInstall($1->name, FdeclarationType);
                         }
        |lvlist ',' ID   {
                            verify($3, 0, 1, 0, NULL);
                            LInstall($3->name, FdeclarationType);
                        }
        ;

mainblock : TypeName MAIN '(' ')' '{' LdeclBlock Body '}'
                            {
                                if (declcount != defcount)
                                {
                                    yyerror("All functions declared need to be defined\n");
                                    exit(1);
                                }
                                if (declarationType != TLookup("integer"))
                                {
                                    yyerror("Main return type should be of integer type\n");
                                    exit(1);
                                }

                                fprintf(intermediate, "MAIN:\n");
                                fprintf(intermediate, "PUSH BP\n");
                                fprintf(intermediate, "MOV BP,SP\n");

                                Ltemp = Lhead;

                                while (Ltemp != NULL)
                                {
                                    fprintf(intermediate, "PUSH R0\n");
                                    Ltemp = Ltemp->next;
                                }

                                codegen($7);

                                Lhead = NULL;
                                Ltail = NULL;
                            }
          ;

Body : START Slist Retstmt END  {
                                    $$ = TreeCreate(TLookup("void"), DEFAULT, NULL, NULL, NULL, $2, $3, NULL);
                                }
        | START Retstmt END     {
                                    $$ = TreeCreate(TLookup("void"), DEFAULT, NULL, NULL, NULL, NULL, $2, NULL);
                                }
    ;

Retstmt : RETURN Expr ';'   {
                                if (declarationType == $2->type)
                                    $$ = TreeCreate($2->type, NODE_RET, NULL, NULL, NULL, NULL, $2, NULL);
                                else
                                {
                                    yyerror("return type do not match with the function return type\n");
                                    exit(1);
                                }
                            }
        ;

Slist : Slist Stmt   	{
                            head = (struct ASTNode *)malloc(sizeof(struct ASTNode));
                            head->nodetype = DEFAULT;
                            head->ptr1 = $1;
                            head->ptr2 = $2;
                            $$ = head;
                        }
        | Stmt          {
                            $$ = $1;
                        }
        ;

Stmt :  ID ASGN Expr ';'         {
                                    type_assign($1, NULL, 0, 0, 0, 0, 0);
                                    if (($1->nodetype == NODE_FIELD) || ($3->nodetype == NODE_FIELD))
                                    {
                                    	head = get_last($1);
                                    	head1 = get_last($3);
                                    	type_comp(head->type, head1->type, 'a');
                                    }
                                    else
                                    	type_comp($1->type, $3->type, 'a');
                                    $$ = TreeCreate(TLookup("void"), NODE_ASGN, NULL, NULL, NULL, $1, $3, NULL);
                                }
        | ID '[' Expr ']' ASGN Expr ';'   {
                                            type_assign_arr($1, $3, 0);
                                            type_comp($1->type, $6->type, 'a');
                                            $$ = TreeCreate(TLookup("void"), NODE_ARRAY_ASGN, NULL, NULL, NULL, $1, $3, $6);
                                        }
        | READ '(' ID ')' ';'   {
                                    type_assign($3, NULL, 0, 0, 0, 0, 1);
                                    $$ = TreeCreate(TLookup("void"), NODE_READ, NULL, NULL, NULL, NULL, $3, NULL);
                                }
        | READ '(' ID '[' Expr ']' ')' ';'   {
                                                type_assign_arr($3, $5, 0);
                                                $$ = TreeCreate(TLookup("void"), NODE_ARRAY_READ, NULL, NULL, NULL, NULL, $3, $5);
                                            }
        | WRITE '(' Expr ')' ';'  {
                                    if ($3->type == TLookup("integer") || $3->type == TLookup("string"))
                                    {
                                        $$ = TreeCreate(TLookup("void"), NODE_WRITE, NULL, NULL, NULL, NULL, $3, NULL);
                                    }
                                    else
                                    {
                                        head = get_last($3);
                                        if (head->type == TLookup("integer") || head->type == TLookup("string"))
                                        {
                                            $$ = TreeCreate(TLookup("void"), NODE_WRITE, NULL, NULL, NULL, NULL, $3, NULL);
                                        }
                                        else
                                        {
                                            yyerror("Cannot write a udt type\n");
                                            exit(1);
                                        }
                                    }

                                }
        |IF '(' Expr ')' THEN Slist ENDIF ';'
                               {
                               		type_comp($3->type, TLookup("boolean"), 'i');
                                    $$ = TreeCreate(TLookup("void"), NODE_IF, NULL, NULL, NULL, $3, $6, NULL);
                               }
        |IF '(' Expr ')' THEN Slist ELSE Slist ENDIF ';'
                                {
                               		type_comp($3->type, TLookup("boolean"), 'e');
                                    $$ = TreeCreate(TLookup("void"), NODE_IF_ELSE, NULL, NULL, NULL, $3, $6, $8);
                                }
        |WHILE '(' Expr ')' DO Slist ENDWHILE ';'
                                {
                               		type_comp($3->type, TLookup("boolean"), 'w');
                                    $$ = TreeCreate(TLookup("void"), NODE_WHILE, NULL, NULL, NULL, $3, $6, NULL);
                                }
        | ID ASGN ALLOC'(' ')' ';'  {
        								type_assign($1, NULL, 1, 0, 1, 0, 0);
                                        head = TreeCreate(TLookup("integer"), NODE_ALLOC, NULL, NULL, NULL, NULL, NULL, NULL);
                                        $$ = TreeCreate(TLookup("void"), NODE_ASGN, NULL, NULL, NULL, $1, head, NULL);
                                    }
        | FIELD ASGN ALLOC'(' ')' ';'  {
                                            head = get_last($1);
                                            if ((head->type == TLookup("integer")) || (head->type == TLookup("string")))
                                            {
                                                yyerror("Cannot ALLOC to string or integer variable\n");
                                                exit(1);
                                            }
                                            else
                                            {
                                                head = TreeCreate(TLookup("integer"), NODE_ALLOC, NULL, NULL, NULL, NULL, NULL, NULL);
                                                $$ = TreeCreate(TLookup("void"), NODE_ASGN, NULL, NULL, NULL, $1, head, NULL);
                                            }
                                        }
        | FIELD ASGN FIELD ';'          {
                                            head = get_last($1);
                                            head1 = get_last($3);
                                            if (head1->type == head->type)
                                                $$ = TreeCreate(TLookup("void"), NODE_ASGN, NULL, NULL, NULL, $1, $3, NULL);
                                            else
                                            {
                                                yyerror("conflicting assignment types in field1\n");
                                                exit(1);
                                            }
                                        }
		| FIELD ASGN Expr ';'			{
                                            head = get_last($1);
                                            if ($3->type == head->type)
                                                $$ = TreeCreate(TLookup("void"), NODE_ASGN, NULL, NULL, NULL, $1, $3, NULL);
                                            else
                                            {
                                                yyerror("conflicting assignment types in field2\n");
                                                exit(1);
                                            }
                                        }
        | FREE '(' ID ')' ';'           {
                                            type_assign($3, NULL, 1, 1, 0, 0, 0);
                                            $$ = TreeCreate(TLookup("void"), NODE_FREE, NULL, NULL, NULL, NULL, $3, NULL);
                                        }
	    | FREE '(' FIELD ')' ';'        {
                                            head = get_last($3);
                                            if ((head->type == TLookup("integer")) || (head->type == TLookup("string")))
                                            {
                                                yyerror("cannot FREE non-udt\n");
                                                exit(1);
                                            }
                                            $$ = TreeCreate(TLookup("void"), NODE_FREE, NULL, NULL, NULL, NULL, $3, NULL);
	                                    }
        | READ '(' FIELD ')' ';'    	{
                                            head = get_last($3);

                                            if ((head->type != TLookup("integer")) && (head->type != TLookup("string")))
                                            {
                                                yyerror("cannot READ a udt type\n");
                                                exit(1);
                                            }
                                            $$ = TreeCreate(TLookup("void"), NODE_READ, NULL, NULL, NULL, NULL, $3, NULL);
	                                    }
		| ID ASGN NILL ';' 				{
			                                type_assign($1, NULL, 1, 0, 0, 0, 0);
			                                $$ = TreeCreate(TLookup("void"), NODE_ASGN, NULL, NULL, NULL, $1, $3, NULL);
			        					}
      	| FIELD ASGN NILL ';'{
                                head = get_last($1);

                                if (head->type == TLookup("integer") || head->type == TLookup("string")
                                    || head->type == TLookup("array_integer") || head->type == TLookup("array_string"))
                                {
                                    yyerror("cannot assign null to a non-udt\n");
                                    exit(1);
                                }
								$$ = TreeCreate(TLookup("void"), NODE_ASGN, NULL, NULL, NULL, $1, $3, NULL);
						    }

		| ID ASGN EXPOSCALL '(' exprlist_exposcall ')' ';'	{
                                                        if ((exprcount < 1) || (exprcount > 4))
                                                        {
                                                           yyerror("Incorrect number of arguments to exposcall function\n");
                                                           exit(1);
                                                        }

                                                        type_comp(NULL, $5->type, 'x');
                                                        type_assign($1, NULL, 0, 0, 0, 0, 0);
                                                        type_comp($1 -> type, NULL, 'x');

                                                        head1 = TreeCreate(TLookup("void"), NODE_EXPOSCALL, "exposcall", NULL, NULL, NULL, NULL, $5);
                                                        $$ = TreeCreate(TLookup("void"), NODE_ASGN, NULL, NULL, NULL, $1, head1, NULL);

													}
		| ID ASGN INIT '(' ')' ';'	{
                                        head = TreeCreate(TLookup("void"), NODE_INIT, NULL, NULL, NULL, NULL, NULL, NULL);
        								$$ = TreeCreate(TLookup("void"), NODE_ASGN, NULL, NULL, NULL, $1, head, NULL);
							        }

        | BRK ';'       {
                            $$ = TreeCreate(TLookup("void"), NODE_BRK, NULL, NULL, NULL, NULL, NULL, NULL);
                        }

        | CONTINUE ';'  {
                            $$ = TreeCreate(TLookup("void"), NODE_CONTINUE, NULL, NULL, NULL, NULL, NULL, NULL);
                        }
        | BRKP ';'  {
                        $$ = TreeCreate(TLookup("void"), NODE_BRKP, NULL, NULL, NULL, NULL, NULL, NULL);
                    }
        ;

FIELD     : ID '.' ID   {
                            $1->nodetype = NODE_FIELD;
                            type_assign($1, $3, 1, 0, 0, 1, 0);
                            $$ = $1;
                        }
          | FIELD '.' ID {
                            head1 = head = get_last($1);

                            temp1 = head->type;
                            if(temp1 != TLookup("string") && temp1 != TLookup("integer")
                                && temp1 != TLookup("array_integer") && temp1 != TLookup("array_string"))
                            {
                                ftemp = FLookup($3->name, temp1->fields);
                                if(ftemp != NULL)
                                {
                                    $3->type = ftemp->type;
                                    head->ptr2 = $3;
                                    $$ = $1;
                                }
                                else
                                {
                                    yyerror("Un-declared field variable\n");
                                    exit(1);
                                }
                            }
                            else
                            {
                                yyerror("Illegal access of identifier\n");
                                exit(1);
                            }
                        }
          ;

exprlist : Expr                 {
                                    exprcount = 1;
                                    $$ = $1;
                                }
        |   exprlist ',' Expr   {
                                    indicator = 1;
                                    exprcount++;
                                    head = (struct ASTNode *)malloc(sizeof(struct ASTNode));
                                    head->nodetype = NODE_EXPR;
                                    head->ptr1 = $3;
                                    head->ptr2 = $1;
                                    $$ = head;
                                }
        ;

exprlist_exposcall : Expr {
                                exprcount = 1;
                                $$ = $1;
                            }
                    | exprlist_exposcall ',' Expr {
                                                    exprcount++;
                                                    head = $1;

                                                    while (head->ptr1 != NULL)
                                                        head = head->ptr1;

                                                    head->ptr1 = $3;
                                                    $$ = $1;
                                                }
                    ;

Expr : Expr PLUS Expr       {
                                type_comp($1->type, $3->type, '+');
                                $$ = TreeCreate(TLookup("integer"), NODE_PLUS , NULL, NULL, NULL, $1, $3, NULL);
                            }
        |Expr MINUS Expr    {
                                type_comp($1->type, $3->type, '-');
                                $$ = TreeCreate(TLookup("integer"), NODE_MINUS , NULL, NULL , NULL, $1, $3, NULL);
                            }
        |Expr MUL Expr      {
                                type_comp($1->type, $3->type, '*');
                                $$ = TreeCreate(TLookup("integer"), NODE_MUL , NULL, NULL , NULL, $1, $3, NULL);
                            }
        |Expr DIV Expr      {
                                type_comp($1->type, $3->type, '/');
                                $$ = TreeCreate(TLookup("integer"), NODE_DIV , NULL, NULL , NULL, $1, $3, NULL);
                            }
        |Expr MOD Expr      {
                                type_comp($1->type, $3->type, '%');
                                $$ = TreeCreate(TLookup("integer"), NODE_MOD , NULL, NULL , NULL, $1, $3, NULL);
                            }
        |Expr LT Expr       {
                                type_comp($1->type, $3->type, '<');
                                $$ = TreeCreate(TLookup("boolean"), NODE_LT , NULL, NULL, NULL, $1, $3, NULL);
                            }
        |Expr LE Expr       {
                                type_comp($1->type, $3->type, '#');
                                $$ = TreeCreate(TLookup("boolean"), NODE_LE , NULL, NULL, NULL, $1, $3, NULL);
                            }
        |Expr GT Expr       {
                                type_comp($1->type, $3->type, '>');
                                $$ = TreeCreate(TLookup("boolean"), NODE_GT , NULL, NULL, NULL, $1, $3, NULL);
                            }
        |Expr GE Expr       {
                                type_comp($1->type, $3->type, '$');
                                $$ = TreeCreate(TLookup("boolean"), NODE_GE , NULL, NULL, NULL, $1, $3, NULL);
                            }
        |Expr DEQ Expr      {
                                type_comp($1->type, $3->type, 'd');
                                $$ = TreeCreate(TLookup("boolean"), NODE_DEQ, NULL, NULL, NULL, $1, $3, NULL);
                            }
        |Expr NEQ Expr      {
                                type_comp($1->type, $3->type, 'n');
                                $$ = TreeCreate(TLookup("boolean"), NODE_NEQ, NULL, NULL, NULL, $1, $3, NULL);
                            }
        |Expr AND Expr      {
                                type_comp($1->type, $3->type, '&');
                                $$ = TreeCreate(TLookup("boolean"), NODE_AND, NULL, NULL, NULL, $1, $3, NULL);
                            }
        |Expr OR Expr       {
                                type_comp($1->type, $3->type, '|');
                                $$ = TreeCreate(TLookup("boolean"), NODE_OR, NULL, NULL, NULL, $1, $3, NULL);
                            }
        |NOT Expr           {
                                type_comp($2->type, NULL, '!');
                                $$ = TreeCreate(TLookup("boolean"), NODE_NOT, NULL, NULL, NULL, NULL, $2, NULL);
                            }
        | '(' Expr ')'      {
                                $$ = $2;
                            }
        | FIELD             {
                                $$ = $1;
                            }
        | NUM               {
                                $$ = $1;
				                $$->type = TLookup("integer");
                            }
        | MINUS NUM         {
                                $2->value.intval = ($2->value.intval) * -1;   //negating the value
                                $$ = $2;
				                $$->type = TLookup("integer");
                            }
        | STRVAL			{
        						$$ = $1;
							    $$->type = TLookup("string");
        					}
		| ID    			{
				    			type_assign($1, NULL, 0, 0, 0, 0, 0);
				    			$$ = $1;
				            }
        | ID '[' Expr ']'   {
								type_assign_arr($1, $3, 0);
								$$ = TreeCreate($1->type, NODE_ARRAY, NULL, NULL, NULL, $1, $3, NULL);
                            }
        |ID '(' exprlist ')'{
        						type_assign_arr($1, $3, 1);

                                $3->ptr3 = Gtemp->paramlist;
                                Arg_callee = Gtemp->paramlist;
                                while (Arg_callee != NULL)
                                {
                                    exprcount--;
                                    Arg_callee = Arg_callee->next;
                                }
                                if (exprcount)
                                {
                                    yyerror("Unequal number of arguments\n");
                                    exit(1);
                                }
                                
                                //differentiating b/w one and more than one arguments
                                if (indicator == 1)
                                {
                                    $$ = TreeCreate($1->type, NODE_FUNC, $1->name, NULL, Gtemp->paramlist, NULL, $3, NULL);
                                    indicator = 0;
                                }

                                else
                                    $$ = TreeCreate($1->type, NODE_FUNC, $1->name, NULL, Gtemp->paramlist, NULL, NULL, $3);
                            }
        |ID '(' ')' 		{
    							type_assign_arr($1, NULL, 1);
                                $$ = TreeCreate($1->type, NODE_FUNC, $1->name, NULL, Gtemp->paramlist, NULL, NULL, NULL);
                            }
	    | ID DEQNILL 		{
	   						    type_assign($1, NULL, 1, 0, 0, 0, 0);
		                    	$$ = TreeCreate(TLookup("boolean"), NODE_DEQ, NULL, NULL, NULL, $1, $2, NULL);
			    			}
        | ID NEQNILL 		{
	   						    type_assign($1, NULL, 1, 0, 0, 0, 0);
	                        	$$ = TreeCreate(TLookup("boolean"), NODE_NEQ, NULL, NULL, NULL, $1, $2, NULL);
	        				}
        | FIELD DEQNILL 	{
	                            type_comp($1, NULL, '=');
	                            $$ = TreeCreate(TLookup("boolean"), NODE_DEQ, NULL, NULL, NULL, $1, $2, NULL);
	        				}
        | FIELD NEQNILL 	{
	                            type_comp($1, NULL, '^');
	                            $$ = TreeCreate(TLookup("boolean"), NODE_NEQ, NULL, NULL, NULL, $1, $2, NULL);
	        				}
        ;

%%

void yyerror(char const *s)
{
    printf("%d: %s\n", lineno, s);
    return ;
}

int main(int argc, char *argv[])
{
    TInstall("integer", 1, NULL);
    TInstall("string", 1, NULL);
    TInstall("boolean", 1, NULL);
    TInstall("array_integer", 1, NULL);
    TInstall("array_string", 1, NULL);
    TInstall("void", 0, NULL);
    TInstall("dummy", 0, NULL); // This is for creating the fieldlist in case of udt

    if (argc < 2)
    {
        printf("Please provide an input filename\n");
        exit(1);
    }
    else
    {
        fp = fopen(argv[1], "r");
        if (!fp)
        {
            printf("Invalid input file specified\n");
            exit(1);
        }
        else
            yyin = fp;
    }
    yyparse();
    return 0;
}