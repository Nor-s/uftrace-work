#include <stdlib.h>

int qcomp(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

__attribute__((noinline)) void test_qsort()
{
    int arr[] = {4, 1, 3, 7, 2, 6, 5};
    qsort(arr, 7, sizeof(int), qcomp);
}

int main()
{
    test_qsort();
}