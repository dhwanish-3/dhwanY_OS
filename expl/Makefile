CC = gcc
CFLAGS = -g -fcommon
LEX = lex
YACC = yacc

default: expl-bin ltranslate

ltranslate: ltranslate.o
		$(CC) $(CFLAGS) -o ltranslate ltranslate.lex.o

ltranslate.o: ltranslate.lex.c
		$(CC) $(CFLAGS) -w -c ltranslate.lex.c

ltranslate.lex.c: ltranslate.l
		$(LEX) -o ltranslate.lex.c ltranslate.l

expl-bin: lex.yy.o y.tab.o
		$(CC) $(CFLAGS) -o expl-bin lex.yy.o y.tab.o

y.tab.o: y.tab.c y.tab.h abstree.h symboltable.h symboltable.c aa.c codegen.c userdtype.c typecheck.h typecheck.c
		$(CC) $(CFLAGS) -w -c y.tab.c

lex.yy.o: lex.yy.c y.tab.c abstree.h
		$(CC) $(CFLAGS) -w -c lex.yy.c

lex.yy.c: abstree.l
		$(LEX) abstree.l

y.tab.c: abstree.y
		$(YACC) -d abstree.y

clean:
		$(RM) expl-bin ltranslate *.o lex.yy.c y.* ltranslate.lex.c assemblycode.xsm
