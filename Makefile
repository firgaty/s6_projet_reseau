NAME ?= miaouchat

CC ?= gcc
CFLAGS ?= -g
# CFLAGS ?= -Wall -g
# CFLAGS ?= -Wall -Werror -Wextra
LDLIBS ?= -O0 -rdynamic -lreadline -lnsl -lpthread -lreadline -pipe -lm
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

SRC_DIR ?= ./src
BUILD_DIR ?= ./build
SRC := $(wildcard $(SRC_DIR)/*.c)

.PHONY: build clean  re

build: $(NAME)

$(NAME): $(SRC)
	@$(CC) $(CFLAGS) $(LDLIBS) $(GTKLIB) $(SRC) -o $(NAME)

clean:
	rm -f *.o $(NAME)

fclean: clean
	@rm -f $(NAME)

re: fclean $(NAME)

doc:
	doxygen Doxyfile
