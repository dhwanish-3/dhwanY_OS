//g -> global lookup
//l -> local lookup
//a -> Arg Lookup
//t -> type

struct Fieldlist *ftemp;

int verify(struct ASTNode *node, int g, int l, int a, struct Typetable *t)
{

    if (l)
    {
        Ltemp = LLookup(node->name);
        if (Ltemp != NULL)
        {
            yyerror("Re initialization of variable\n");
            exit(1);
        }
    }

    if (a)
    {
        Argtemp = PLookup(node->name);
        if (Argtemp != NULL)
        {
            yyerror("Re initialization of variable in paramlist\n");
            exit(1);
        }
    }

    if (g)
    {
        Gtemp = GLookup(node->name);
        if (Gtemp != NULL)
        {
            yyerror("Re initialization of identifier\n");
            exit(1);
        }
    }

    if (t != NULL)
    {
        if (!(t == TLookup("integer")))
        {
            yyerror("arrays of udt and strings are not allowed\n");
            exit(1);
        }
    }
    return 1;
}

int install_id(struct ASTNode *node, struct ASTNode *node2, struct Typetable *t)
{
    if (t == TLookup("integer"))
        GInstall(node->name, TLookup("array_integer"), node2->value.intval, NULL);
    else if (t == TLookup("string"))
        GInstall(node->name, TLookup("array_string"), node2->value.intval, NULL);
    else
    {
        yyerror("arrays of udt is not allowed\n");
        exit(1);
    }
}

int type_comp(struct Typetable *t1, struct Typetable *t2, char c)
{
    switch (c)
    {
    case 'r':
        if (t1 != t2)
        {
            yyerror("return type do not match with the function return type\n");
            exit(1);
        }
        break;
    case ' ':
        return (t1 == t2);
        break;
    case 'i':
        if (t1 != t2)
        {
            yyerror("Expected boolean , Found value in if\n");
            exit(1);
        }
        break;
    case 'e':
        if (t1 != t2)
        {
            yyerror("Expected boolean , Found value in if else\n");
            exit(1);
        }
        break;
    case 'w':
        if (t1 != t2)
        {
            yyerror("Expected boolean , Found value in while\n");
            exit(1);
        }
        break;
    case '+':
        if (t1 == TLookup("string") || t2 == TLookup("string"))
        {
            yyerror("conflict in operand types in PLUS\n");
            exit(1);
        }
        break;
    case '-':
        if (t1 == TLookup("string") || t2 == TLookup("string"))
        {
            yyerror("conflict in operand types in MINUS\n");
            exit(1);
        }
        break;
    case '*':
        if (t1 == TLookup("string") || t2 == TLookup("string"))
        {
            yyerror("conflict in operand types in MUL\n");
            exit(1);
        }
        break;
    case '/':
        if (t1 == TLookup("string") || t2 == TLookup("string"))
        {
            yyerror("conflict in operand types in DIV\n");
            exit(1);
        }
        break;
    case '%':
        if (t1 == TLookup("string") || t2 == TLookup("string"))
        {
            yyerror("conflict in operand types in MOD\n");
            exit(1);
        }
        break;
    case '<':
        if (t1 == TLookup("string") || t2 == TLookup("string"))
        {
            yyerror("conflict in operand types in LT\n");
            exit(1);
        }
        break;
    case '>':
        if (t1 == TLookup("string") || t2 == TLookup("string"))
        {
            yyerror("conflict in operand types in GT\n");
            exit(1);
        }
        break;
    case '#': // <=
        if (t1 == TLookup("string") || t2 == TLookup("string"))
        {
            yyerror("conflict in operand types in LE\n");
            exit(1);
        }
        break;
    case '$': // >=
        if (t1 == TLookup("string") || t2 == TLookup("string"))
        {
            yyerror("conflict in operand types in GE\n");
            exit(1);
        }
        break;
    case 'a':
        if (t1 != t2)
        {
            yyerror("conflict in assignment types\n");
            exit(1);
        }
        break;
    case 'd': // ==
        if (t1 != t2)
        {
            yyerror("conflict in operand types in DEQ\n");
            exit(1);
        }
        break;
    case 'n': // !=
        if (t1 != t2)
        {
            yyerror("conflict in operand types in NEQ\n");
            exit(1);
        }
        break;
    case '&': // AND
        if (!(t1 == TLookup("boolean") && t2 == TLookup("boolean")))
        {
            yyerror("conflict in operand types in AND\n");
            exit(1);
        }
        break;
    case '|': // OR
        if (!(t1 == TLookup("boolean") && t2 == TLookup("boolean")))
        {
            yyerror("conflict in operand types in OR\n");
            exit(1);
        }
        break;
    case '!': // NOT
        if (t1 != TLookup("boolean"))
        {
            yyerror("conflict in operand types in NOT\n");
            exit(1);
        }
        break;
    case '=': // == NULL
        if (t1 == TLookup("string") || t1 == TLookup("integer"))
        {
            yyerror("conflict in operand types in DEQNILL\n");
            exit(1);
        }
        break;
    case '^': // != NULL
        if (t1 == TLookup("string") || t1 == TLookup("integer"))
        {
            yyerror("conflict in operand types in NEQNILL\n");
            exit(1);
        }
    case 'x': //exposcall
        if (t2 != NULL)
        {
            if (t2 != TLookup("string"))
            {
                yyerror("invalid fun_code type in exposcall\n");
                exit(1);
            }
        }
        else
        {
            if (t1 == TLookup("string"))
            {
                yyerror("invalid return type to exposcall\n");
                exit(1);
            }
        }
        break;
    }

    return 1;
}

//assign type for id and id deqnill and neqnill
// fr = free
// al = alloc
// fd = field
// rd = read (id)

int type_assign(struct ASTNode *node, struct ASTNode *node2, int udt, int fr, int al, int fd, int rd)
{
    Ltemp = LLookup(node->name);

    if (Ltemp != NULL)
    {
        if (udt)
        {
            if (Ltemp->type == TLookup("integer") || Ltemp->type == TLookup("string"))
            {
                if (fr)
                    yyerror("cannot free a non udt\n");
                else if (al)
                    yyerror("cannot ALLOC a non udt\n");
                else if (fd)
                    yyerror(" . operation over integer/string type is not allowed\n");
                else
                    yyerror("cannot assign null to non-udt\n");
                exit(1);
            }
        }
        node->type = Ltemp->type;

        if (fd)
        {
            ftemp = FLookup(node2->name, node->type->fields);
            if (ftemp != NULL)
            {
                node2->type = ftemp->type;
                node->ptr2 = node2;
            }
            else
            {
                yyerror("Un-declared field variable\n");
                exit(1);
            }
        }
    }

    else
    {
        Argtemp = PLookup(node->name);
        if (Argtemp != NULL)
        {
            if (!rd)
            {
                if (udt)
                {
                    if (Argtemp->type == TLookup("integer") || Argtemp->type == TLookup("string"))
                    {
                        if (fr)
                            yyerror("cannot free a non udt\n");
                        else if (al)
                            yyerror("cannot ALLOC a non udt\n");
                        else if (fd)
                            yyerror(" . operation over integer/string type is not allowed\n");
                        else
                            yyerror("cannot assign null to non-udt\n");
                        exit(1);
                    }
                }
                node->type = Argtemp->type;

                if (fd)
                {
                    ftemp = FLookup(node2->name, node->type->fields);
                    if (ftemp != NULL)
                    {
                        node2->type = ftemp->type;
                        node->ptr2 = node2;
                    }
                    else
                    {
                        yyerror("Un-declared field variable\n");
                        exit(1);
                    }
                }
            }

            else
            {
                if (Argtemp->type == TLookup("integer"))
                    node->type = TLookup("integer");
                else if (Argtemp->type == TLookup("string"))
                    node->type = TLookup("string");
            }
        }

        else
        {
            Gtemp = GLookup(node->name);
            if (Gtemp == NULL)
            {
                yyerror("Un-declared variable\n");
                printf("%s\n", node->name);
                exit(1);
            }
            else
            {
                if (!rd)
                {
                    if (Gtemp->type == TLookup("array_integer") || Gtemp->type == TLookup("array_string"))
                    {
                        if (fd)
                        {
                            yyerror(" . operation over arrays not allowed\n");
                        }
                        else
                        {
                            yyerror("conflict in ID NodeType : Expected Variable . Found Array\n");
                        }

                        printf("%s\n", node->name);
                        exit(1);
                    }
                }
            }
            if (udt)
            {
                if (Gtemp->type == TLookup("integer") || Gtemp->type == TLookup("string"))
                {
                    if (fr)
                    {
                        yyerror("cannot free a non udt\n");
                    }
                    else if (al)
                    {
                        yyerror("cannot ALLOC a non udt\n");
                    }
                    else if (fd)
                    {
                        yyerror(" . operation over integer/string type is not allowed\n");
                    }
                    else
                    {
                        yyerror("cannot assign null to non-udt\n");
                    }
                    exit(1);
                }
            }
            if (!fr)
            {
                node->Gentry = Gtemp;
            }
            node->type = Gtemp->type;

            if (fd)
            {
                ftemp = FLookup(node2->name, node->type->fields);
                if (ftemp != NULL)
                {
                    node2->type = ftemp->type;
                    node->ptr2 = node2;
                }
                else
                {
                    yyerror("Un-declared field variable\n");
                    exit(1);
                }
            }
        }
    }
    return 1;
}

//type assign for arrays and functions
int type_assign_arr(struct ASTNode *node, struct ASTNode *node2, int func)
{
    Gtemp = GLookup(node->name);
    if (Gtemp == NULL)
    {
        printf("Un-declared identifier\n");
        printf("%s\n", node->name);
        exit(1);
    }
    if (func)
    {
        if (Gtemp->size != -1)
        {
            yyerror("conflict in ID NodeType : Expected Function \n");
            printf("%s\n", node->name);
            exit(1);
        }
        else
        {
            node->Gentry = Gtemp;
            node->type = Gtemp->type;
        }
        return 0;
    }
    else if (Gtemp->type == TLookup("string") || Gtemp->type == TLookup("integer"))
    {
        yyerror("conflict in ID NodeType : Expected Variable , Found Array\n");
        printf("%s\n", node->name);
        exit(1);
    }
    else if (node2->type != TLookup("integer"))
    {
        yyerror("Expected value \n");
        printf("%s\n", node->name);
        exit(1);
    }
    else
    {
        node->Gentry = Gtemp;
        if (Gtemp->type == TLookup("array_integer"))
            node->type = TLookup("integer");
        else if (Gtemp->type == TLookup("array_string"))
            node->type = TLookup("string");
    }
    return 1;
}

int start()
{
    intermediate = fopen("assemblycode.asm", "w+");
    if (intermediate == NULL)
    {
        printf("Unable to locate file\n");
        exit(1);
    }

    fprintf(intermediate, "0\n2056\n0\n0\n0\n0\n0\n0\n");
    fprintf(intermediate, "MOV SP,%d\n", totalCount - 1);
    fprintf(intermediate, "MOV BP,%d\n", totalCount);
    fprintf(intermediate, "PUSH R0\n");
    fprintf(intermediate, "CALL MAIN\n");
    fprintf(intermediate, "INT 10\n"); //need to push registers
    return 0;
}

struct ASTNode *get_last(struct ASTNode *head)
{
    while (head->ptr2 != NULL)
        head = head->ptr2;

    return head;
}