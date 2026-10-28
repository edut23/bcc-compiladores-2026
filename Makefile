CFLAGS = -I$(include) -g

EXEC = mypas

include = ./include

$(EXEC): main.o lexer.o parser.o symtab.o keywords.o
	$(CC) -o $@ $^

test-iskeyword: test_iskeyword.o keywords.o
	$(CC) -o $@ $^
	./$@

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~ $(EXEC) test-iskeyword
