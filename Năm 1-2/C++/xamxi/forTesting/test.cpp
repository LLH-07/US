#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<vector <int>> a;
    a[0].push_back(5);
    a[0].push_back(4);
    cout << a[0][0] << endl << a[0][1];
    return 0;
}