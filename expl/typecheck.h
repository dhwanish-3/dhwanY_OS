int verify(struct ASTNode *node, int g, int l, int a, struct Typetable *t);
int type_comp(struct Typetable *t1, struct Typetable *t2, char c);
int type_assign(struct ASTNode *node, struct ASTNode *node2, int udt, int fr, int al, int fd, int rd);
int type_assign_arr(struct ASTNode *node, struct ASTNode *node2, int func);
int alloc_assign(struct ASTNode *node);
struct ASTNode *get_last(struct ASTNode *head);
int free_id(struct ASTNode *node);
int start();
int install_id(struct ASTNode *node, struct ASTNode *node2, struct Typetable *t);