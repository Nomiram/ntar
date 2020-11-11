all:
	gcc -o ntar ntar.c
sanitize:
	gcc -o ntar ntar.c -fsanitize=address
