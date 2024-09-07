
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LIB = -L ./lib/libft/ -lft
HEADER = -I ./include -I ./lib/libft
FLAG = -Wall  -Wextra -g -MMD
OBJ = $(addsuffix .o, $(addprefix obj/, malloc_bis))
CC = gcc

dynamic: FLAG += -D DYNAMIC=1
dynamic: ${NAME}

D_LST = $(OBJ:.o=.d)

OBJ_DIR = obj/

all: $(NAME)

$(NAME): $(OBJ)
	gcc -shared -o $(NAME) $(OBJ) $(LIB)


$(OBJ): obj_rep
	make bonus -s -C lib/libft
	$(CC) $(HEADER) $(FLAG) -fPIC -I ./include -g3 -c src/malloc_bis.c -o obj/malloc_bis.o

test: $(NAME)
	$(CC) $(HEADER) -Wall -Werror -Wextra  -I ./lib/unity/ -I ./ -g3 -c tests/test.c -o tests/test.o
	$(CC) $(HEADER) -Wall -Werror -Wextra -g3 -c ./lib/unity/unity.c -o obj/unity.o
	$(CC) tests/test.o obj/unity.o -o tests/test  -L ./ -lft_malloc_$(HOSTTYPE) $(LIB)  
	LD_LIBRARY_PATH=./libs:./ ./tests/test

obj_rep:
	@mkdir -p $(OBJ_DIR)

clean: 
	rm -f *.o
	rm -rf tests/test
	rm -rf tests/*.o
	rm -f obj/*.o
	make fclean -s -C lib/libft

fclean: clean
	rm -f $(NAME)
	rm -f *.so
	rm -rf $(OBJ_DIR)


re: fclean all

.PHONY: all bonus clean fclean re test
-include $(D_LST)
