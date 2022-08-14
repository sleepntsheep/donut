all: donut
LIBS := -lraylib -lm

donut: *.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

web: *.c
	emcc -o web/donut.html $^ -Os ./raylib/src/libraylib.a -I./raylib/src -L./raylib/src -s USE_GLFW=3 --shell-file web/shell.html

