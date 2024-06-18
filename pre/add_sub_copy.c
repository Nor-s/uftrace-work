#include <stdio.h>

int add(int a, int b)
{
	return (a + b);
}

int sub(int a, int b)
{
	return (a - b);
}
int get3()
{
	return 3;
}
int get4()
{
	int a = 33;
	if (a == 33)
	{
		return &a;
	}
	return a;
}

int main()
{
	printf("hello uftrace!\n");

	int a = 5;
	int b = 3;

	int add_ret = add(a, b);
	printf("call add() func, return val : %d\n", add_ret);

	int sub_ret = sub(a, b);
	printf("call sub() func, return val : %d\n", sub_ret);

	int sub_ret2 = sub(get3(), get4());
}