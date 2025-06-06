# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jpelline <jpelline@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/22 16:14:45 by jpelline          #+#    #+#              #
#    Updated: 2025/05/07 14:16:18 by jpelline         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Colors and formatting
BOLD		:= $(shell tput bold)
GREEN		:= $(shell tput setaf 2)
YELLOW		:= $(shell tput setaf 3)
BLUE		:= $(shell tput setaf 4)
MAGENTA		:= $(shell tput setaf 5)
CYAN		:= $(shell tput setaf 6)
WHITE		:= $(shell tput setaf 7)
RESET		:= $(shell tput sgr0)

SRCS		:=	ft_isalpha.c ft_isdigit.c ft_isalnum.c ft_isascii.c \
				ft_isprint.c ft_strlen.c ft_memset.c ft_bzero.c ft_memcpy.c \
				ft_memmove.c ft_strlcpy.c ft_strlcat.c ft_toupper.c \
				ft_tolower.c ft_strchr.c ft_strrchr.c ft_strncmp.c ft_atoi.c \
				ft_memchr.c ft_memcmp.c ft_strnstr.c ft_atof.c ft_calloc.c \
				ft_strdup.c ft_substr.c ft_strjoin.c ft_strtrim.c ft_split.c \
				ft_itoa.c ft_strmapi.c ft_striteri.c ft_putchar_fd.c \
				ft_putstr_fd.c ft_putendl_fd.c ft_putnbr_fd.c ft_uputnbr_fd.c \
				ft_reverse_string.c ft_lstnew_bonus.c ft_lstadd_front_bonus.c \
				ft_lstsize_bonus.c ft_lstlast_bonus.c ft_lstadd_back_bonus.c \
				ft_lstdelone_bonus.c ft_lstclear_bonus.c ft_lstiter_bonus.c \
				ft_lstmap_bonus.c ft_printf.c

NAME		:= libft.a

CC			:= cc
CFLAGS		:= -Wall -Wextra -Werror -I.

OBJ_DIR		:= obj
DEP_DIR		:= $(OBJ_DIR)/.deps

DEPFLAGS	:= -MMD -MP -MF $(DEP_DIR)/$*.d
MAKEFLAGS	:= --no-print-directory -j$(nproc)

OBJS		:= $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

INC			:= libft.h

all:
	@if	[ -f $(NAME) ] && $(MAKE) -q $(NAME); then \
		echo "$(BOLD)$(YELLOW)🔄 $(NAME) is already up to date.$(RESET)"; \
	else \
		echo "$(BOLD)$(WHITE)🌀 Starting to build $(NAME)...$(RESET)"; \
		$(MAKE) -s $(NAME) $(MAKEFLAGS); \
		echo "$(BOLD)$(GREEN)✅ $(NAME) created successfully!$(RESET)"; \
	fi

$(NAME): $(OBJS)
	@ar rcs $@ $^

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(DEP_DIR):
	@mkdir -p $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR) $(DEP_DIR)
	@mkdir -p $(@D) $(DEP_DIR)/$(*D)
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(wildcard $(DEP_DIR)/*.d)

clean:
	@rm -rf $(OBJ_DIR)

fclean:	clean
	@rm -f $(NAME)

re:	fclean
	@$(MAKE) -s all

.SECONDARY: $(OBJS)
.PHONY: all clean fclean re
