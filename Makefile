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
LIB_DIR = lib

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRC_FILES = \
	main.c			\
	error.c			\

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
LFT_DIR = $(LIB_DIR)/libft
LFT = $(LFT_DIR)/$(LFT_NAME)

LMARG_NAME = libmarg.a
LMARG_DIR = $(LIB_DIR)/mini-arg
LMARG = $(LMARG_DIR)/$(LMARG_NAME)

CFLAGS += -I./$(LFT_DIR)/include
CFLAGS += -I./$(LMARG_DIR)/include

LDFLAGS += -L./$(LFT_DIR) -lft
LDFLAGS += -L./$(LMARG_DIR) -lmarg

LDLIBS = $(LFT) $(LMARG)

# **************************************************************************** #
#                                    RULES                                     #
# **************************************************************************** #

SILENT := all
PHONY := all
all: $(NAME)

$(NAME): $(OBJ) $(LDLIBS)
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
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)

$(LFT):
	$(MAKE) -C $(LFT_DIR)

$(LMARG):
	$(MAKE) -C $(LMARG_DIR)

PHONY += clean
clean:
	$(MAKE) fclean -C $(LFT_DIR)
	$(MAKE) fclean -C $(LMARG_DIR)
	$(RM) $(LFT_NAME)
	$(RM) $(OBJ_DIR)

PHONY += fclean
fclean: clean
	$(RM) $(NAME)

PHONY += re
re: fclean all

-include $(DEP)

.SILENT: $(SILENT)
.PHONY: $(PHONY)
