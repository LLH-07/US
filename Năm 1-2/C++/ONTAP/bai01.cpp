#include <iostream>

using namespace std;

int countExchange(int n)
{
    int count = 0;
    for(int i = 0; i <= n / 5; i++) //To 5
        for(int j = 0; j <= n / 2; j++) // To 2
            for(int k = 0; k <= n; k++) // To 1
                if(i * 5 + j * 2 + k * 1 == n)
                    ++count;
    
    return count;
}

void Exchange(int n)
{
    for(int i = 0; i <= n / 5; i++) //To 5
    {
        for(int j = 0; j <= n / 2; j++) // To 2
        {
            for(int k = 0; k <= n; k++) // To 1
            {
                if(i * 5 + j * 2 + k * 1 == n)
                {
                    if(k != 0)
                    {
                        cout << k << " to 1 dong";
                        if(j != 0)
                            cout << " va " << j << " to 2 dong";
                        if(i != 0)
                            cout << " va " << i << " to 5 dong";
                    }
                    else
                    {
                        if(j != 0)
                        {
                            cout << j << " to 2 dong";
                            if(i != 0)
                                cout << " va " << i << " to 5 dong";
                        }
                        
                        else cout << i << " to 5 dong";
                    }
                    cout << "." << endl;
                }
            }
        }
    }
}

int main()
{
    int n;
    cout << "Nhap vao so tien: ";
    cin >> n;
    cout << "Co " << countExchange(n) << " cach doi";
    //cout << endl;
    //Exchange(n);
    return 0;
}