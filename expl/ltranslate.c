struct LTable
{
	char *name;
	int addr;
	struct LTable *next;
};
struct LTable *LThead, *LTtail;

void append_table(char *label, int addr)
{
	struct LTable *temp;
	temp = (struct LTable *)malloc(sizeof(struct LTable));
	temp->name = label;
	temp->addr = addr;

	if (LThead == NULL)
	{
		LThead = temp;
		LTtail = temp;
	}
	else
	{
		LTtail->next = temp;
		LTtail = LTtail->next;
	}
	return;
}

int find_map(char *name)
{
	struct LTable *temp;

	temp = LThead;
	while (temp != NULL)
	{
		if (strcmp(name, temp->name) == 0)
			return temp->addr;

		temp = temp->next;
	}
	return -1;
}

void print_table()
{
	struct LTable *temp;

	temp = LThead;
	while (temp != NULL)
	{
		printf("%s : %d\n", temp->name, temp->addr);
		temp = temp->next;
	}
	return;
}