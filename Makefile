
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LIB = -L ./lib/libft/ -lft
HEADER = -I ./include -I ./lib/libft
FLAG = -Wall -Werror -Wextra -g -MMD
OBJ = $(addsuffix .o, $(addprefix obj/, malloc))
CC = gcc

D_LST = $(OBJ:.o=.d)

OBJ_DIR = obj/

all: $(NAME)

$(NAME): $(OBJ)
	make bonus -s -C lib/libft
	$(CC) -shared -fPIC -o $(NAME) $(OBJ)

$(OBJ): obj_rep
	$(CC) $(HEADER) $(FLAGS)   -I ./include -g3 -c src/malloc.c -o obj/malloc.o

%.so : $(OBJ)
	$(CC) $(FLAGS) $(HEADER) $(LIB) -c $< -o $@

test: $(NAME)
	$(CC) $(HEADER) -Wall -Werror -Wextra  -I ./lib/unity/ -I ./ -g3 -c tests/test.c -o tests/test.o
	$(CC) $(HEADER) -Wall -Werror -Wextra -g3 -c ./lib/unity/unity.c -o obj/unity.o
	$(CC) $(LIB) tests/test.o obj/unity.o -o tests/test  -L ./ -lft_malloc_x86_64_Linux 
	./tests/test

obj_rep:
	@mkdir -p $(OBJ_DIR)

clean: 
	rm -f *.o
	rm -f tests/*.o
	make fclean -s -C lib/libft

fclean: clean
	rm -f $(NAME)
	rm -f *.so
	rm -rf $(OBJ_DIR)


re: fclean all

.PHONY: all bonus clean fclean re
-include $(D_LST)
