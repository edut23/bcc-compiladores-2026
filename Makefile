CFLAGS = -I. -g

EXEC = mybc

$(EXEC): main.o lexer.o parser.o symtab.o vmachine.o
	$(CC) -o $@ $^

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~ $(EXEC)
