SRCS := *.c

donut: $(SRCS)
	clang $(SRCS) -o donut -lSDL2 -lm -g

web: $(SRCS) web/shell.html
	emcc -o web/donut.html $(SRCS) -Os -s USE_SDL=2 --shell-file web/shell.html

