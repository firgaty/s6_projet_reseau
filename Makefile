NAME ?= client

CC ?= gcc
CFLAGS ?= -Wall -g
# CFLAGS ?= -Wall -Werror -Wextra
LDLIBS ?= -lreadline -lnsl -lpthread -lreadline -lm

SRC_DIR ?= ./src
BUILD_DIR ?= ./build
SRC := $(wildcard $(SRC_DIR)/*.c)

.PHONY: build clean  re

build: $(NAME)

$(NAME): $(SRC)
	@$(CC) $(CFLAGS) $(LDLIBS) $(SRC) -o $(NAME)

clean:

fclean: clean
	@rm -f $(NAME)

re: fclean all

doc:
	doxygen Doxyfile
