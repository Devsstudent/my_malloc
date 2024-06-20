
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LIB = -L ./lib/libft/ -lft
HEADER = -I ./include -I ./lib/libft
FLAG = -Wall -Werror -Wextra -g -MMD -fPIC
OBJ = $(addsuffix .o, $(addprefix obj/, malloc))

OBJ_DIR = obj/


all: $(NAME)

$(NAME): $(OBJ)
	make bonus -s -C lib/libft
	$(CC) -shared -o $(NAME) $(OBJ)

$(OBJ_DIR)%.o : src/%.c | obj_rep
	$(CC) $(FLAGS) $(HEADER) -c $< -o $@

obj_rep:
	@mkdir -p $(OBJ_DIR)

clean: 
	rm -f *.o
	make fclean -s -C lib/libft

fclean: clean
	rm -f $(NAME)


re: fclean all

.PHONY: all bonus clean fclean re

