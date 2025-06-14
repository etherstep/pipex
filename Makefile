# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jpelline <jpelline@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/03 18:36:06 by jpelline          #+#    #+#              #
#    Updated: 2025/06/03 18:36:20 by jpelline         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================== CONFIGURATION =============================== #

NAME		:=	pipex
CC			:=	cc
CFLAGS		:=	-Wextra -Wall -Werror
DEBUG_FLAGS	:=	-g3 -fsanitize=address -fsanitize=undefined
OPTFLAGS	:=	-O2

SRC_DIR		:= src
OBJ_DIR		:= obj

DEP_DIR		:= $(OBJ_DIR)/.deps
DEPFLAGS	= -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

LIBFT_DIR	:=	libft
LIBFT		:=	$(LIBFT_DIR)/libft.a
INC			:= -I./include -I$(LIBFT_DIR)/include

LDFLAGS		:=	-L$(LIBFT_DIR) -lft

# ============================== VISUAL STYLING ============================== #

BOLD		:= $(shell tput bold)
GREEN		:= $(shell tput setaf 2)
YELLOW		:= $(shell tput setaf 3)
BLUE		:= $(shell tput setaf 4)
MAGENTA		:= $(shell tput setaf 5)
CYAN		:= $(shell tput setaf 6)
WHITE		:= $(shell tput setaf 7)
RESET		:= $(shell tput sgr0)

# ============================== SOURCE FILES ================================ #

SRCS		:=	main.c child_process.c cleanup_utils.c \
				command_parser.c pipeline_manager.c
				
OBJS		:=	$(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

# ============================== PROGRESS TRACKING =========================== #

TOTAL_SRCS	:=	$(words $(SRCS))
PROGRESS_FILE := $(OBJ_DIR)/.progress

# ============================== BUILD TARGETS =============================== #

all:
	@if [ -f $(NAME) ] && $(MAKE) -q $(NAME) --no-print-directory; then \
		echo ">$(BOLD)$(YELLOW)  $(NAME) is already up to date.$(RESET)"; \
	else \
		echo ">$(BOLD)$(WHITE) Starting to build $(NAME)...$(RESET)"; \
		$(MAKE) $(NAME) --no-print-directory; \
		echo ">$(BOLD)$(GREEN)  All components built successfully!$(RESET)"; \
	fi

# Debug target
debug: CFLAGS += $(DEBUG_FLAGS)
debug: OPTFLAGS := -O0
debug: clean $(NAME)
	@echo "$(BOLD)$(CYAN)  Debug build completed!$(RESET)"

# Main executable target - links all objects and libraries
$(NAME): $(OBJS) $(LIBFT)
	@echo ">$(BOLD)$(GREEN)  Linking $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(OPTFLAGS)
	@echo ">$(BOLD)$(GREEN)  $(NAME) successfully compiled!$(RESET)"
	@rm -f $(PROGRESS_FILE)

# Create necessary directories if they don't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@echo "0" > $(PROGRESS_FILE)

$(DEP_DIR): | $(OBJ_DIR)
	@mkdir -p $@

# Compilation rule for each source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(DEP_DIR)
	@touch $(PROGRESS_FILE)
	@echo ">$(BOLD)$(WHITE) Compiling $(NAME) srcs...$(RESET)"
	@if [ -f $(PROGRESS_FILE) ]; then \
		CURRENT=$$(cat $(PROGRESS_FILE)); \
		NEXT=$$((CURRENT + 1)); \
		echo "$$NEXT" > $(PROGRESS_FILE); \
		printf ">   [%3d%%] $(CYAN)Compiling $<...$(RESET)\n" \
			$$((NEXT*100/$(TOTAL_SRCS))); \
	fi
	@$(CC) $(CFLAGS) $(DEPFLAGS) $(OPTFLAGS) -c $< -o $@ $(INC)

# Include auto-generated dependency files
-include $(wildcard $(DEP_DIR)/*.d)

# build libft if needed
$(LIBFT):
	@echo ">$(MAGENTA)  Entering libft directory...$(RESET)"
	@$(MAKE) -C $(LIBFT_DIR) --no-print-directory

# Remove object files and dependency files
clean:
	@if [ -d $(OBJ_DIR) ]; then \
		echo "> [ pipex ] $(YELLOW) Cleaning object files...$(RESET)"; \
		rm -rf $(OBJ_DIR); \
		echo "            $(YELLOW) Object files cleaned!$(RESET)"; \
	else \
		echo "> [ pipex ] $(BOLD)$(YELLOW) Nothing to be done with $(RESET)$(WHITE)clean$(RESET)"; \
	fi
	@if [ -d $(LIBFT_DIR)/$(OBJ_DIR) ]; then \
		$(MAKE) -C $(LIBFT_DIR) clean --no-print-directory; \
	else \
		echo "> [ libft ] $(BOLD)$(YELLOW) Nothing to be done with $(RESET)$(WHITE)clean$(RESET)"; \
	fi

# Remove everything including the executable
fclean: clean
	@if [ -f $(NAME) ]; then \
		echo "> [ pipex ] $(YELLOW) Removing $(NAME)...$(RESET)"; \
		rm -rf $(NAME); \
		echo "            $(YELLOW) $(NAME) removed!$(RESET)"; \
	else \
		echo "> [ pipex ] $(BOLD)$(YELLOW) Nothing to be done with $(RESET)$(WHITE)fclean$(RESET)"; \
	fi
	@if [ -f $(LIBFT) ]; then \
		$(MAKE) -C $(LIBFT_DIR) fclean --no-print-directory; \
	else \
		echo "> [ libft ] $(BOLD)$(YELLOW) Nothing to be done with $(RESET)$(WHITE)fclean$(RESET)"; \
	fi

# Full rebuild from scratch
re: fclean
	@echo "> [ pipex ] $(BOLD)$(WHITE) Rebuilding from scratch...$(RESET)"
	@$(MAKE) all --no-print-directory

# Additional useful targets
help:
	@echo "$(BOLD)$(CYAN)Available targets:$(RESET)"
	@echo "  $(GREEN)all$(RESET)     - Build the project (default)"
	@echo "  $(GREEN)debug$(RESET)   - Build with debug flags and sanitizers"
	@echo "  $(GREEN)clean$(RESET)   - Remove object files"
	@echo "  $(GREEN)fclean$(RESET)  - Remove all generated files"
	@echo "  $(GREEN)re$(RESET)      - Rebuild from scratch"
	@echo "  $(GREEN)help$(RESET)    - Show this help message"

# Prevent intermediate files from being deleted
.SECONDARY: $(OBJS)
.PHONY: all debug clean fclean re help
