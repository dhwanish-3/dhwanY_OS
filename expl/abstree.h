#define FALSE 0
#define TRUE 1
#define TYPE_INT 2
#define TYPE_BOOL 3
#define TYPE_VOID 4
#define NODE_ASGN 5
#define NODE_READ 6
#define NODE_WRITE 7
#define NODE_IF 8
#define NODE_IF_ELSE 9
#define NODE_ID 10
#define NODE_PLUS 11
#define NODE_MINUS 12
#define NODE_MUL 13
#define NODE_DIV 25
#define NODE_MOD 26
#define NODE_LT 14
#define NODE_GT 15
#define NODE_DEQ 16
#define NODE_NEQ 27
#define NODE_NUM 17
#define NODE_WHILE 18
#define NODE_T 20
#define NODE_F 21
#define NODE_LE 22
#define NODE_GE 23
#define NODE_ARRAY 24
#define NODE_ARRAY_ASGN 28
#define NODE_ARRAY_READ 29
#define NODE_AND 30
#define NODE_OR 31
#define NODE_NOT 32
#define NODE_FUNC 33
#define SYMBOL_FUNC_INT 34
#define SYMBOL_FUNC_BOOLEAN 35
#define NODE_RET 36
#define NODE_BODY 37
#define NODE_MAIN 38
#define NODE_EXPR 39
#define NODE_FIELD 40
#define NODE_ALLOC 41
#define NODE_FREE 42
#define NODE_NILL 43
#define NODE_INIT 44
#define NODE_BRK 45
#define NODE_CONTINUE 46
#define TYPE_STR 47
#define NODE_STRVAL 48
#define NODE_EXPOSCALL 49
#define NODE_BRKP 50
#define NODE_NEW 51
#define NODE_CLASS_FUNC 52

#define DEFAULT 100
#define TYPE_DEFAULT 200

union Constant {
  int intval;
  char *strval;
};

struct ASTNode
{
  struct Typetable *type;             //pointer to the type table entry
  int nodetype;                       //node type information,eg : NODETYPE_WHILE,NODETYPE_PLUS,NODETYPE_STMT etc
  char *name;                         //stores the variable/function name in case of variable/function nodes
  union Constant value;               //stores the value of the constant if the node corresponds to a constant
  struct ASTNode *arglist;            //pointer to the expression list given as arguments to a function call
  struct ASTNode *ptr1, *ptr2, *ptr3; //Subtrees of the node. (Maximum Subtrees for IF THEN ELSE)
  struct Gsymbol *Gentry;             //pointer to GST entry for global variables and functions
  struct Lsymbol *Lentry;             //pointer to the function's LST for local variables and arguements
};

struct ASTNode *TreeCreate(struct Typetable *type, int nodetype, char *name, union Constant *value, struct ASTNode *arglist, struct ASTNode *ptr1, struct ASTNode *ptr2, struct ASTNode *ptr3);

int Evaluate(struct ASTNode *root);
int codegen(struct ASTNode *root);