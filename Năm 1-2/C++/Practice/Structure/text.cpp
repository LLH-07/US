#include <iostream>
#include <string>

using namespace std;

struct B
{
    int bField;
};

struct A
{
    B b;
    int aField;
};

int main()
{
    A arr[5] = {
    {{1}, 10},
    {{2}, 20},
    {{3}, 30},
    {{4}, 40},
    {{5}, 50}
    };


}