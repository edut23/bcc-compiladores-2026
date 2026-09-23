CFLAGS = -I. -g

EXEC = mybc

mybc: main.o lexer.o parser.o
	$(CC) -o $@ $^

# depolution targets:

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~
