SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

./a.out : $(OBJ)
	gcc -o $@ $^

clean :
	rm *o ./a.out
