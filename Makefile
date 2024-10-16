ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LIB = -L ./lib/libft/ -lft
HEADER = -I ./include -I ./lib/libft
FLAG = -Wall -Werror -Wextra -g -MMD
OBJ = $(addprefix obj/, $(addsuffix .o,  malloc free realloc chunk zone zone_utils chunk_utils show_alloc))
OBJ_TEST = obj/unity.o tests/test.o
SRC_TEST = lib/unity/unity.c tests/tests.c
CC = gcc
SRC=$(OBJ:.o=.c)

obj_rep:
	make bonus -C lib/libft
	mkdir -p $(OBJ_DIR)

dynamic: FLAG += -D DYNAMIC=1
dynamic: ${NAME}

D_LST = $(OBJ:.o=.d) $(OBJ_TEST:.o=.d)

OBJ_DIR = obj/

all: obj_rep  $(NAME)

$(NAME): $(OBJ)
	gcc -shared -o $(NAME) -fPIC $(OBJ) $(LIB)

$(SRC):
	@echo compiling $@
	$(CC) $(HEADER) $(FLAG) -fPIC -I ./include -g3 -c src/$@ -o obj/$(@:.c=.o)

test: obj_rep $(NAME) $(OBJ_TEST)
	$(CC) tests/test.o obj/unity.o $(OBJ) -L ./ -lft_malloc_$(HOSTTYPE) $(LIB) -o tests/test 
	LD_LIBRARY_PATH=./libs:./ ./tests/test

obj/unity.o: lib/unity/unity.c
	$(CC) $(HEADER) -Wall -Werror -Wextra -g3 -c ./lib/unity/unity.c -o obj/unity.o

tests/test.o: tests/test.c
	$(CC) $(HEADER) -Wall -Werror -Wextra  -I ./lib/unity/ -I ./ -I ./include -g3 -c tests/test.c -o tests/test.o

obj/%.o: src/%.c
	$(CC) $(HEADER) $(FLAG) -fPIC -c $< -o $@

clean: 
	rm -f $(OBJ) 
	rm -rf tests/test
	rm -rf tests/*.o
	make fclean -s -C lib/libft

fclean: clean
	rm -f $(NAME)
	rm -f *.so
	rm -rf $(OBJ_DIR)

re: fclean all

.PHONY: all bonus clean fclean re test
#-include $(D_LST)
