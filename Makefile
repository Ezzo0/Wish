file = ./vector/vector.c
file += wish.c

build:
	gcc  $(file) -Wall -Werror -g -o wish

run: build
	@./wish

clean: build
	@rm wish
