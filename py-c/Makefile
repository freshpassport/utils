all:
	gcc -fpic -c -I/usr/include/python2.7 -I/usr/lib/python2.7/config wrap.c
	gcc -shared -o wrap.so wrap.o

clean:
	rm -fr *.o *.so
