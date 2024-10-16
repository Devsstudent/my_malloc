ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LIB = -L ./lib/libft/ -lft
HEADER = -I ./include -I ./lib/libft
FLAG = -Wall -Werror -Wextra -g -MMD
OBJ = $(addprefix obj/, $(addsuffix .o,  malloc free realloc chunk zone zone_utils chunk_utils show_alloc))
OBJ_TEST = obj/unity.o tests/test.o tests/gpttest.o
SRC_TEST = lib/unity/unity.c tests/tests.c tests/gpttest.c
CC = gcc
SRC=$(OBJ:.o=.c)

obj_rep:
	make bonus -C lib/libft
	mkdir -p $(OBJ_DIR)

dynamic: FLAG += -D DYNAMIC=1
dynamic: ${NAME}

D_LST = $(OBJ:.o=.d) $(OBJ_TEST:.o=.d)

OBJ_DIR = obj/

all: $(NAME)

$(NAME): $(OBJ)
	gcc -shared -o $(NAME) -fPIC $(OBJ) $(LIB)

$(SRC):
	@echo compiling $@
	$(CC) $(HEADER) $(FLAG) -fPIC -I ./include -g3 -c src/$@ -o obj/$(@:.c=.o)

test: obj_rep $(NAME) $(OBJ_TEST)
	$(CC) tests/test.o obj/unity.o $(OBJ) -L ./ -lft_malloc_$(HOSTTYPE) $(LIB) -o tests/test 
	LD_LIBRARY_PATH=./libs:./ ./tests/test
	$(CC) tests/gpttest.o -L ./ -lft_malloc_$(HOSTTYPE) $(LIB) -o ./big_test

obj/unity.o: lib/unity/unity.c | obj_rep
	$(CC) $(HEADER) -Wall -Werror -Wextra -g3 -c ./lib/unity/unity.c -o obj/unity.o

tests/test.o: tests/test.c | obj_rep
	$(CC) $(HEADER) -Wall -Werror -Wextra -g3 -I ./lib/unity/ -I ./ -I ./include -c tests/test.c -o tests/test.o

tests/gpttest.o: tests/gpttest.c | obj_rep
	$(CC) -Wall -Werror -Wextra -g3 $(HEADER) -c $< -o $@ 

obj/%.o: src/%.c | obj_rep
	$(CC) $(HEADER) $(FLAG) -fPIC -c $< -o $@

clean: 
	rm -f $(OBJ) 
	rm -rf tests/test
	rm -rf tests/*.o
	rm -rf ./big_test
	make fclean -s -C lib/libft

fclean: clean
	rm -f $(NAME)
	rm -f *.so
	rm -rf $(OBJ_DIR)

re: fclean all

.PHONY: all bonus clean fclean re test
#-include $(D_LST)
