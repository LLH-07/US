#include <iostream>
#include <cctype>
#include <cstring>
#include <string.h>

using namespace std;


// Problem 4
// int main()
// {
//     char s[] = "A string";
//     char* ptr;
//     ptr = s;
//     cout << *ptr << " " << &ptr << " " << *(ptr + 3) << endl;
//     ptr += 2;
//     cout << ptr << " " << *(ptr + 2) << " " << *(ptr + 5);
// }


// // Problem 5
// int main()
// {
//     char s[10] ="abcde";
//     char* cptr;
//     cptr = s;
//     while ( *cptr != '\0')
//         cptr++;

//     cptr--;
//     while ( cptr >= s)
//     {
//         cout << *cptr;
//         cptr--;
//     }
//     return 0;
// }


// Problem 6
int countEven (int* a, int n)
{
    int ans = 0;
    for ( int i = 0; i < n; i++ )
        if ( *(a + i) % 2 == 0 )
            ++ans;

    return ans;
}


// Problem 7
double* maximum(double* a, int size)
{
    if ( size == 0 ) return NULL;

    double* mx;
    mx = a;
    for ( int i = 0; i < size; i++ )
        if ( *(a + i) > *mx )
            mx = (a + i);

    return mx;
}


// Problem 8
int myStrlen (char* c)
{
    int len = 0;
    while ( *c != '\0' )
    {
        len++;
        c++;
    }

    return len;
}


// Problem 9
bool myStrContains(char* str, char c)
{
    int len = myStrlen(str);
    for ( int i = 0; i  < len; i++ )
        if ( *(str + i) == c )
            return true;

    return false;
}

void revString(char* ptr)
{
    int len = myStrlen(ptr);
    char s[len];
    int k = 0;

    for ( int i = len - 1; i >= 0; i-- )
    {
        *(s + k) = *(ptr + i);
        k++;
    }

    for( int i = 0; i < len; i++ )
        *(ptr + i) = *(s + i);

}