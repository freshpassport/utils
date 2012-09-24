#include <stdio.h>

typedef int (*FUNC_TYPE)(void *arg);

int test(void *arg)
{
	return printf("test(), arg: %8p, %d\n", arg, *((int*)arg));
}

// call func directly
FUNC_TYPE test_func = test;

// call func in array
FUNC_TYPE func_list[128];

int main()
{
	int a = 100, i;

	// set func array
	for (i=0;i<10;i++)
	{
		func_list[i] = test;
	}

	// call func array
	for (i=0; i<10; i++)
		func_list[i]((void*)&i);

	// call func dirctly
	return test_func((void*)&a);
}

