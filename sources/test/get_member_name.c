#include <stdio.h>

//#define _STR(x) #x
//#define STR(x) _STR(x)

#define STR(x) #x

struct abc {
	int member1;
	int mbmber2;
};

int main()
{
	struct abc test;
	printf("%s\n", STR(member1));
	printf("%s\n", STR(smart/read_err));
}

