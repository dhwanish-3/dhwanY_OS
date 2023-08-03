#define BUFF 80
#define TABSIZE 128
#define MEMSIZE 1024 * 1024
#define NUMREGS 1024
#define STKSIZE 64

struct
{
	char data[BUFF];
	int flag;
} yylval;

struct entry
{
	char label[BUFF];
	int position;
	int lineno;
};

struct
{
	struct entry entry[TABSIZE];
	int label_count;
} symtab;

int pass, started, error, start_offset, line_count;

int mem[MEMSIZE], reg[NUMREGS], sp, bp, ip;

int stack[STKSIZE], top;