#include "func.h"
int global1;

int foo(int x)
{
    return x;
}

int global2;

int main(int arg)
{
    int local;
    local = arg;
    foo_ext(arg);
    return foo(local);
}

