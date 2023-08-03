struct ASTNode *TreeCreate(struct Typetable *type, int nodetype, char *name, union Constant *value, struct ASTNode *arglist, struct ASTNode *ptr1, struct ASTNode *ptr2, struct ASTNode *ptr3)
{
	struct ASTNode *temp;
	temp = (struct ASTNode *)malloc(sizeof(struct ASTNode));
	temp->type = type;
	temp->nodetype = nodetype;
	if (name != NULL)
	{
		temp->name = (char *)malloc(strlen(name) * sizeof(char));
		strcpy(temp->name, name);
	}
	else
	{
		temp->name = NULL;
	}
	if (value != NULL)
	{
		temp->value.intval = value->intval;
	}
	else
	{
		temp->value.strval = NULL;
	}
	temp->arglist = arglist;
	temp->ptr1 = ptr1;
	temp->ptr2 = ptr2;
	temp->ptr3 = ptr3;
	return temp;
}
