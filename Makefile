# Sourse files
FM_SRC=$(wildcard src/file_manager/*.c)
TE_SRC=$(wildcard src/text_editor/*.c)
#Object files
FM_OBJ=$(patsubst src/file_manager/%.c,%.o,$(FM_SRC))
TE_OBJ=$(patsubst src/text_editor/%.c,%.o,$(TE_SRC))

#Build File Manager
all: te fm_lib
	gcc src/file_manager.c -o file_manager -L./lib/ -lfmanager -lncursesw -lm
fm_lib: $(FM_OBJ)
	ar -cr ./lib/libfmanager.a $(FM_OBJ)
	rm -rf *.o
$(FM_OBJ): $(FM_SRC)
	gcc $(FM_SRC) -c

#Build Text Editor
te: te_lib
	gcc src/text_editor.c -o bin/text_editor -L./lib/ -lteditor -lncursesw
te_lib: $(TE_OBJ)
	ar -cr ./lib/libteditor.a $(TE_OBJ)
	rm -rf *.o
$(TE_OBJ): $(TE_SRC)
	gcc $(TE_SRC) -c
#Over
clean:
	rm -rf *.o file_manager ./lib/* ./bin/*

run:
	./file_manager
