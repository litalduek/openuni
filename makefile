all:
	gcc	-Wall -ansi assembler.c file_management.c parser.c data.c -pedantic -o assembler -g
