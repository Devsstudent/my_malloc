ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LIB = -L ./lib/libft/ -lft
HEADER = -I ./include -I ./lib/libft
FLAG = -Wall  -Wextra -g -MMD
OBJ = $(addsuffix .o,  malloc_bis free realloc chunk zone zone_utils chunk_utils show_alloc)
CC = gcc
SRC=$(OBJ:.o=.c)


dynamic: FLAG += -D DYNAMIC=1
dynamic: ${NAME}

D_LST = $(OBJ:.o=.d)

OBJ_DIR = obj/

all: $(NAME)

$(NAME): $(OBJ)
	gcc -shared -o $(NAME) -fPIC $(addprefix obj/, $(OBJ)) $(LIB)


$(OBJ): obj_rep $(SRC)
	@echo building

$(SRC):
	@echo compiling $@
	$(CC) $(HEADER) $(FLAG) -fPIC -I ./include -g3 -c src/$@ -o obj/$(@:.c=.o)

test: $(NAME)
	$(CC) $(HEADER) -Wall -Werror -Wextra  -I ./lib/unity/ -I ./ -I ./include -g3 -c tests/test.c -o tests/test.o
	$(CC) $(HEADER) -Wall -Werror -Wextra -g3 -c ./lib/unity/unity.c -o obj/unity.o
	$(CC)  tests/test.o obj/unity.o $(addprefix obj/, $(OBJ)) -L ./ -lft_malloc_$(HOSTTYPE) $(LIB) -o tests/test 
	LD_LIBRARY_PATH=./libs:./ ./tests/test

obj_rep:
	make bonus -s -C lib/libft
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
