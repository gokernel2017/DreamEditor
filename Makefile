# Project: libasm

CPP  = g++
CC   = gcc
OBJ      = obj/app.o obj/draw.o obj/o_button.o obj/o_edit.o obj/o_editor.o obj/o_console.o obj/menu.o obj/lang_core.o obj/lang_lex.o obj/lang_vm.o
LINKOBJ  = obj/app.o obj/draw.o obj/o_button.o obj/o_edit.o obj/o_editor.o obj/o_console.o obj/menu.o obj/lang_core.o obj/lang_lex.o obj/lang_vm.o
BIN  = libapp.a
CFLAGS = -Wall
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before libapp.a all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(LINKOBJ)
	ar r $(BIN) $(LINKOBJ)
	ranlib $(BIN)

obj/app.o: src/app.c
	$(CC) $(CFLAGS) -c src/app.c -o obj/app.o

obj/draw.o: src/draw.c
	$(CC) $(CFLAGS) -c src/draw.c -o obj/draw.o

obj/o_button.o: src/o_button.c
	$(CC) $(CFLAGS) -c src/o_button.c -o obj/o_button.o

obj/o_edit.o: src/o_edit.c
	$(CC) $(CFLAGS) -c src/o_edit.c -o obj/o_edit.o

obj/o_editor.o: src/o_editor.c
	$(CC) $(CFLAGS) -c src/o_editor.c -o obj/o_editor.o

obj/o_console.o: src/o_console.c
	$(CC) $(CFLAGS) -c src/o_console.c -o obj/o_console.o

obj/menu.o: src/menu.c
	$(CC) $(CFLAGS) -c src/menu.c -o obj/menu.o

##########  LANGUAGE  ##########

obj/lang_core.o: src/lang_core.c
	$(CC) $(CFLAGS) -c src/lang_core.c -o obj/lang_core.o

obj/lang_lex.o: src/lang_lex.c
	$(CC) $(CFLAGS) -c src/lang_lex.c -o obj/lang_lex.o

obj/lang_vm.o: src/lang_vm.c
	$(CC) $(CFLAGS) -c src/lang_vm.c -o obj/lang_vm.o


