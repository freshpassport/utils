all:
	gcc -fpic -c -I/usr/include/python2.7 -I/usr/lib/python2.7/config wrap.c -o wrap.so
	gcc -shared -o wrap.so wrap.o
