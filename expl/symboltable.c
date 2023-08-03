struct Gsymbol *GLookup(char *name)
{
    struct Gsymbol *temp;

    temp = Ghead;
    while (temp != NULL && (strcmp(temp->name, name) != 0))
    {
        temp = temp->next;
    }
    return temp;
}

void GInstall(char *name, struct Typetable *type, int size, struct Paramstruct *paramlist)
{
    struct Gsymbol *temp;

    temp = GLookup(name);
    if (temp != NULL)
    {
        yyerror("Variable re-initialized");
        printf("\"%s\"\n", name);
        exit(1);
    }

    temp = (struct Gsymbol *)malloc(sizeof(struct Gsymbol));
    temp->name = (char *)malloc(strlen(name) * sizeof(char));
    strcpy(temp->name, name);
    temp->type = type;
    temp->size = size;
    temp->paramlist = paramlist;
    temp->next = NULL;

    if (temp->size == -1)
    {
        temp->binding = fbind;
        fbind++;
    }
    else
    {
        temp->binding = totalCount;
        totalCount = totalCount + temp->size;
    }

    if (Ghead != NULL)
    {
        Gtail->next = temp;
        Gtail = temp;
    }
    else
    {
        Ghead = temp;
        Gtail = temp;
    }
    return;
}

struct Lsymbol *LLookup(char *name)
{
    struct Lsymbol *temp;
    temp = Lhead;
    while (temp != NULL && (strcmp(temp->name, name) != 0))
        temp = temp->next;
    return temp;
}

void LInstall(char *name, struct Typetable *type)
{
    struct Lsymbol *temp;
    temp = (struct Lsymbol *)malloc(sizeof(struct Lsymbol));
    temp->name = (char *)malloc(strlen(name) * sizeof(char));
    strcpy(temp->name, name);
    temp->type = type;
    temp->next = NULL;
    temp->binding = totalCount;
    totalCount = totalCount + 1;

    if (Lhead != NULL)
    {
        Ltail->next = temp;
        Ltail = temp;
    }
    else
    {
        Lhead = temp;
        Ltail = temp;
    }
    return;
}

struct Paramstruct *PLookup(char *name)
{
    struct Paramstruct *temp;
    temp = Phead;
    while (temp != NULL && (strcmp(temp->name, name) != 0))
        temp = temp->next;
    return temp;
}

void PInstall(char *name, struct Typetable *type)
{
    struct Paramstruct *temp;
    temp = (struct Paramstruct *)malloc(sizeof(struct Paramstruct));
    temp->name = (char *)malloc(strlen(name) * sizeof(char));
    strcpy(temp->name, name);
    temp->type = type;
    temp->next = NULL;

    if (Phead == NULL)
    {
        Phead = temp;
        Ptail = temp;
    }
    else
    {
        Ptail->next = temp;
        Ptail = Ptail->next;
    }
    return;
}

struct Typetable *TLookup(char *name)
{
    struct Typetable *temp;

    temp = Thead;
    while (temp != NULL && (strcmp(temp->name, name) != 0))
        temp = temp->next;
    return temp;
}

void TInstall(char *name, int size, struct Fieldlist *fields)
{
    struct Typetable *temp;
    struct Fieldlist *ftemp;
    int counter = 0;

    temp = (struct Typetable *)malloc(sizeof(struct Typetable));
    temp->name = (char *)malloc(strlen(name) * sizeof(char));
    strcpy(temp->name, name);
    temp->next = NULL;

    if (Thead == NULL)
    {
        Thead = temp;
        Ttail = temp;
    }
    else
    {
        Ttail->next = temp;
        Ttail = Ttail->next;
    }

    ftemp = fields;

    while (ftemp != NULL)
    {
        if (ftemp->type == TLookup("dummy"))
        {
            ftemp->type = TLookup(name);
        }

        ftemp->fieldIndex = counter++;
        ftemp = ftemp->next;
    }
    temp->fields = fields;
    temp->size = counter;
    Fhead = NULL;
    Ftail = NULL;
    return;
}

struct Fieldlist *FLookup(char *name, struct Fieldlist *list)
{
    struct Fieldlist *temp;
    temp = list;
    while (temp != NULL && (strcmp(temp->name, name) != 0))
        temp = temp->next;
    return temp;
}

void FInstall(struct Typetable *type, char *name)
{
    struct Fieldlist *temp;
    struct Typetable *temp1;
    temp = (struct Fieldlist *)malloc(sizeof(struct Fieldlist));
    temp->name = (char *)malloc(strlen(name) * sizeof(char));
    strcpy(temp->name, name);
    temp->type = type;
    temp->next = NULL;

    if (Fhead == NULL)
    {
        Fhead = temp;
        Ftail = temp;
    }
    else
    {
        Ftail->next = temp;
        Ftail = Ftail->next;
    }
    return;
}

void printSymbolTable()
{
    struct Gsymbol *temp;
    temp = Ghead;
    while (temp != NULL)
    {
        printf("%s----%s-----%d\n", temp->name, temp->type->name, temp->binding);
        temp = temp->next;
    }
}
