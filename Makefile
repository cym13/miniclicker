
all: miniclicker

miniclicker: miniclicker.c
	$(CC) $(FLAGS) -lX11 -o miniclicker miniclicker.c
