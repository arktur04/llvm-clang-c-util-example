#include "func.h"

class MyClass {
    public:
    MyClass() {
        int SomeLocal_1;
    }
    void foo() {
        int SomeLocal_2;
    }
    ~MyClass() {
        int SomeLocal_3;
    }
};

MyClass myClass_global;

int foo(int x) {return 0;}

int main(int argc, char** argv)
{
    int local;
    MyClass myClass_local;
    foo(argc);
    foo_ext(local);
    return 1;
}


