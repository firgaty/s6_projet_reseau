NAME ?= client

CC ?= gcc
CFLAGS ?= -Wall -g 
# CFLAGS ?= -Wall -Werror -Wextra
LDLIBS ?= -lreadline

SRC_DIR ?= ./src
BUILD_DIR ?= ./build
SRC := $(wildcard $(SRC_DIR)/*.c)

.PHONY: all clean  re

all: $(NAME)

$(NAME): $(SRC)
	@$(CC) $(CFLAGS) $(LDLIBS) $(SRC) -o $(NAME)
	doxygen Doxyfile

clean:

fclean: clean
	@rm -f $(NAME)

re: fclean all