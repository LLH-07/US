#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

void input(int (*a))
{
    srand(time(NULL));
    for ( int i = 0; i < 9; i++ )
            a[i] = rand() % 100;
}

void print(int (*a))
{
    for ( int i = 0; i < 9; i++ )
    {
        cout << a[i] << " ";

        if( ( i + 1 ) % 3 == 0 ) 
            cout << endl;
    }
}

void sort(int (*a))
{
    for( int k = 0; k < 9; k++ )
        for ( int i = 0; i < 3; i++ )
            for ( int j = 0; j < 3; j++ )
                if( *( a + i * 3 + j ) > *( a + i * 3 + ( j + 1 ) ) )
                    swap( *( a + i * 3 + j ), *( a + i * 3 + ( j + 1 ) ));
}

int main()
{
    int a[9];
    input(a);
    print(a);
    cout << endl;
    sort(a);
    print(a);
}