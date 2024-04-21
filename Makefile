LIBFT_DIR = libft

HEADER_SRC = a_pipex.h
HEADER_SRC_BONUS = pipex_bonus.h

SRC = ./a_main.c \
      ./a_children.c \
      ./a_path.c \
      ./a_clean.c \
      ./a_utils.c
SRC_BONUS = ./main_bonus.c \
            ./children_bonus.c \
            ./path_bonus.c \
            ./exec_bonus.c \
            ./clean_bonus.c \
            ./utils_bonus.c \
            ./here_doc_bonus.c \
            ./get_next_line_bonus.c

OBJ = $(SRC:%.c=%.o)
OBJ_BONUS = $(SRC_BONUS:%.c=%.o)

CC = cc
CPPFLAGS = -Werror -Wextra -Wall -I ./include -I $(LIBFT_DIR)
LDFLAGS = -L$(LIBFT_DIR)

NAME = pipex
NAME_BONUS = pipex_bonus

all:$(NAME)

$(NAME):$(OBJ) $(LIBFT_DIR)/libft.a
	$(CC) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

bonus:$(NAME_BONUS)

$(NAME_BONUS):$(OBJ_BONUS) $(LIBFT_DIR)/libft.a
	$(CC) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

$(LIBFT_DIR)/libft.a:
	make -C $(LIBFT_DIR)
	
$(OBJ): $(HEADER_SRC) $(LIBFT_DIR)/libft.h

$(OBJ_BONUS): $(HEADER_SRC_BONUS) $(LIBFT_DIR)/libft.h

clean:
	rm -f $(OBJ) $(OBJ_BONUS)
	make -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME) $(NAME_BONUS)
	make -C $(LIBFT_DIR) fclean

re: fclean all

re_bonus: fclean bonus

.PHONY: all bonus clean fclean re
