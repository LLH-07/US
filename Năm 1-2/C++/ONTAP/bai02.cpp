#include <iostream>

using namespace std;

void Input(int A[], int& N)
{
    cout << "Nhap vao chieu dai cua mang: ";
    cin >> N;

    for(int i = 0; i < N; i++) cin >> A[i];
}

void delAt(int A[], int& N, int pos)
{
    for(int i = pos; i < N - 1; i++)
        A[i] = A[i + 1];

    --N;
}

void reduceArray(int A[], int &N)
{
    for(int i = 0; i < N - 1; i++)
    {
        if(A[i] % 2 == 0 && A[i + 1] % 2 == 0)
        {
            A[i] = A[i] + A[i + 1];
            delAt(A, N, i + 1);
            i--;
        }
    }
}

int main()
{
    int N;
    int A[1000];
    Input(A, N);
    reduceArray(A, N);
    for(int i = 0; i < N; i++) cout << A[i] << " ";
    cout << ", N = " << N;
    return 0;
}