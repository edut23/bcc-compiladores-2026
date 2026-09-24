CFLAGS = -I. -g

EXEC = mybc

$(EXEC): main.o lexer.o parser.o
	$(CC) -o $@ $^

# depolution targets:

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~ $(EXEC)