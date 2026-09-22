#include <iostream>
#include <cmath>
#include <string.h>
#include <cstring>
#include <cctype>

#define MAX 1000
#define MAXROW 100
#define MAXCOL 100

using namespace std;


//Mang 1 chieu

void setArray(int a[], int &n)
{
    cout << "Nhap vao so luong phan tu: ";
    cin >> n;
    for(int i = 0; i < n; i++)
    {
        cout << "Nhap vao a[" << i << "] : ";
        cin >> a[i];
    }
}

void printArray(int a[], int n)
{
    for(int i = 0; i < n; i++)
    {
        cout << "a[" << i << "] = " << a[i];
    }
}

int sumArray(int a[], int n)
{
    int sum = 0;
    for(int i = 0; i < n; i++) sum += a[i];
    return sum;
}

int findMin(int a[], int n)
{
    int mn = 1000000000;
    for(int i = 0; i < n; i++)
    {
        if(mn > a[i]) mn = a[i];
    }
    return mn;
}

int checkPrime(int n)
{
    if(n < 2) return 0;
    for(int i = 2; i <= sqrt(n); i++)
    {
        if(n % i == 0) return 0;
    }
    return 1;
}

int countNumberofPrime(int a[], int n)
{
    int prime = 0;
    for(int i = 0; i < n; i++)
    {
        if(checkPrime(a[i]) == 1) ++prime;
    }
    return prime;
}

int isAscendingOrder(int a[], int n)
{
    for(int i = 0; i < n - 1; i++)
    {
        if (a[i] > a[i + 1]) return 0;
    }
    return 1;
}

int countOccurrences(int a[], int n, int x)
{
    int count = 0;
    for(int i = 0; i < n; i++)
    {
        if(a[i] == x) ++count;
    }
    return count;
}

void deleteItems(int a[], int &n, int x)
{
    int count = 0;
    for(int i = 0; i < n; i++)
    {
        if(a[i] == x)
        {
            ++count;
            for(int j = i; j < n - count; j++)
            {
                a[j] = a[j + 1];
            }
            --i;
        }
    }
    n -= count;
}

void deleteItemsFromIndex(int a[], int &n, int index, int no_items)
{
    for (int i = index; i < n - no_items; i++) a[i] = a[i + no_items];
    n -= no_items;
}
void concatTwoArrays(int a[], int n, int b[], int m, int c[])
{
    for(int i = 0; i < n; i++) c[i] = a[i];
    for(int i = n; i < m + n; i++) c[i] = b[i];
}

void concatInterspersedlyTwoArray(int a[], int n, int b[], int m, int c[])
{
    for(int i = 0; i < m + n; i+=2)
    {
        c[i] = a[i];
        c[i + 1] = b[i];
    }
}

/*chuoi*/
void reverseCharArray(char a[MAX])
{
    int n = strlen(a);
    for(int i = 0; i < n / 2; i++) swap(a[i], a[n - 1 - i]);
}

int checkSymmetricArray(char a[MAX])
{
    char s[MAX];
    strcpy(s,a);
    reverseCharArray(a);
    if(strcmp(s, a) == 0) return 1;
    else return 0;
}

int countOccurrencesChar(char a[1000], char c){
	int n = strlen(a);
	int count = 0;
	for (int i = 0; i < n; i++)
		if (a[i] == c)
			count ++;
	return count;
}

void findMostFrequentChar(char a[MAX], char &chr)
{
    int len = strlen(a);
    int count[MAX] = {0};
    for(int i = 0; i < len; i++)
        ++count[a[i]];
    int mx = 0;
    for(int i = 0; i < len; i++)
    {
        if(mx < count[a[i]])
        {
            mx = count[a[i]];
            chr = a[i];
        }
    }
}

void insertCharAtPosition(char str[], char chr, int pos)
{
    int len = strlen(str);
    char a[MAX];
    ++len;
    for(int i = len - 1; i > pos; i-- )
    {
        str[i] = str[i - 1];
    }
    str[pos] = chr;
}
// hello friends
// chr = 'K'
// pos = 8
// a[13] = a[12] = 's'
// a[12] = a]11] = 'd'
// a[11] = a[10] = 'n'
// a[10] = a[9]  = 'e'
// a[9]  = a[8]  = 'i'
//a[8] = chr = 'K'

void formatNumberWithCommas(char str[])
{
    int len = strlen(str);
    for(int i = len - 3; i >= 0; i -= 3)
    {
            insertCharAtPosition(str, ',', i);
    }
}

//MANG 2 CHIEU

void set2dArray (int a[][MAXCOL], int m, int n)
{
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            cin >> a[i][j];
        }
    }
}

void print2dArray(int a[][MAXCOL], int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            cout << a[i][j] << " ";
        }
        cout << endl;
    }
}

int sumArray (int a[][MAXCOL], int m, int n)
{
    int sum = 0;
    for (int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            sum += a[i][j];
        }
    }
    return sum;
}

int sumDiagonal(int a[][MAXCOL], int m, int n)
{
    int sum = 0, mn = m;
    if (m > n) mn = n;
    for(int i = 0; i < mn; i++) sum += a[i][i];
    return sum;
}

int findMin(int a[][MAX], int m, int n)
{
    int mn = 10e8;
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if(mn > a[i][j]) mn = a[i][j];
        }
    }
    return mn;
}

int countNumberofPrime(int a[][MAX], int m, int n)
{
    int dem = 0;
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if(checkPrime(a[i][j]) == 1) ++dem;
        }
    }
    return dem;
}

int countNumberofUniquePrime(int a[][MAXCOL], int m, int n)
{
    int dem[MAX] ={0}; //cach nay de tran so neu so lon do tu lam
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if(checkPrime(a[i][j])) ++dem[a[i][j]];
        }
    }
}

int countOccurrences(int a[][MAX], int m, int n, int x)
{
    int dem = 0;
    for(int i = 0; i < m; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if((a[i][j]) == x) ++dem;
        }
    }
    return dem;
}

void delete2dArr_row(int a[][MAX], int& m, int n, int pos)
{
	for (int i = pos; i < m - 1; i++)
		for (int j = 0; j < n; j++)
			a[i][j] = a[i+1][j];
	m = m - 1;
}

void delete2dArr_col(int a[][MAX], int m, int& n, int pos)
{
	for (int i = 0; i < m ; i++)
		for (int j = pos; j < n - 1; j++)
			a[i][j] = a[i][j + 1];
	n = n - 1;
}

int findXin2dArr(int a[][MAX], int m, int n, int X, int& delRow, int& delCol)
{
	for (int i = 0; i < m ; i++)
		for (int j = 0; j < n; j++)
			if (a[i][j] == X){
				delRow = i;
				delCol = j;
				return 1;
			}
	return 0;
}
void deleteItems(int a[][MAX], int &m, int &n, int X)
{
	int delRow = -1, delCol = -1, isExist;
	do{
		isExist = findXin2dArr(a, m, n, X, delRow, delCol);
		if (isExist == 1)
        {
			delete2dArr_row(a, m, n, delRow);
			delete2dArr_col(a, m, n, delCol);
		}
		else break;
	}while (isExist != 0);
}
int main()
{
    char a[MAX];
    int b[MAXROW][MAXCOL];
    cout << "Nhap a: ";
    cin >> a;
}
