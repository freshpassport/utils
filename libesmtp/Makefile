all:
	gcc -o jj esmtp-test.c -std=c99 -Wall `libesmtp-config --cflags` `libesmtp-config --libs`

clean:
	rm -fr jj core* *.o
