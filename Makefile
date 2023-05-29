# **************************************************************************** #
#                                   PROGRAM                                    #
# **************************************************************************** #

NAME = ft_ping

# **************************************************************************** #
#                                     VARS                                     #
# **************************************************************************** #

UNAME_S := $(shell uname -s)

MAKE = make

CP = cp
MV = mv
MKDIR = mkdir -p
RM = rm -rf

# **************************************************************************** #
#                                   COMPILER                                   #
# **************************************************************************** #

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -Werror -Wpedantic -Wshadow

# **************************************************************************** #
#                                     DIRS                                     #
# **************************************************************************** #

SRC_DIR = src
OBJ_DIR = build
INC_DIR = include

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRC_FILES = \
	main.c			\

OBJ_FILES = $(SRC_FILES:%.c=%.o)

SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

OBJ = $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))
DEP = $(OBJ:%.o=%.d)

# **************************************************************************** #
#                                    FLAGS                                     #
# **************************************************************************** #

CFLAGS += -I./$(INC_DIR)

# **************************************************************************** #
#                                     LIBS                                     #
# **************************************************************************** #

LFT_NAME = libft.a
LFT_DIR = libft
LFT = $(LFT_DIR)/$(LFT_NAME)

LMINIARG_NAME = libminiarg.a
LMINIARG_DIR = lib/mini-arg
LMINIARG = $(LMINIARG_DIR)/$(LMINIARG_NAME)

CFLAGS += -I./$(LFT_DIR)/include
CFLAGS += -I./$(LMINIARG_DIR)/include

LDFLAGS += -L./$(LFT_DIR) -lft
LDFLAGS += -L./$(LMINIARG_DIR) -lminiarg

# **************************************************************************** #
#                                    RULES                                     #
# **************************************************************************** #

PHONY := all
all: $(NAME)

$(NAME): $(OBJ) $(LFT) $(LMINIARG)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

PHONY += sanitize
ifeq ($(UNAME_S),Linux)
sanitize: CFLAGS += -g3 -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=bounds -fsanitize=null
endif
ifeq ($(UNAME_S),Darwin)
sanitize: CFLAGS += -g3 -fsanitize=address
endif
sanitize: $(NAME)

PHONY += thread
thread: CFLAGS += -g3 -fsanitize=thread
thread: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)

$(LFT):
	$(MAKE) -C $(LFT_DIR)

$(LMINIARG):
	$(MAKE) -C $(LMINIARG_DIR)

PHONY += clean
clean:
	$(MAKE) fclean -C $(LFT_DIR)
	$(RM) $(LFT_NAME)
	$(RM) $(OBJ_DIR)

PHONY += fclean
fclean: clean
	$(RM) $(NAME)

PHONY += re
re: fclean all

-include $(DEP)

.PHONY: $(PHONY)
