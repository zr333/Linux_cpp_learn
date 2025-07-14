all: dlsym_hook macro_hook

dlsym_hook:dlsym_hook.c
	gcc -g -o dlsym_hook dlsym_hook.c

macro_hook:macro_hook.c
	gcc -g -o macro_hook macro_hook.c


clean:
	rm -f dlsym_hook macro_hook
