#include <stdio.h>
#include <setjmp.h>

jmp_buf JumpBuffer;

#define try if(!setjmp(JumpBuffer))
#define catch else
#define throw longjmp(JumpBuffer, 1)

int test(int t)
{
  if (t>100)
    throw;
  else
    puts("OK");
  return 0;
}

int test_t(int t)
{
  test(t);
  return 0;
}

int main()
{
  int t;

  try {
    puts("input a value: ");
    scanf("%d", &t);
    test_t(t);
  } catch {
    puts("input error");
  }

  return 0;
}

