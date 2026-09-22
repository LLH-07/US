#include <fstream>
#include <iostream>

using namespace std;

int main()
{
    ifstream in("file.txt");
    ofstream out("out.txt");

    if (in.fail() || out.fail())
    {
        cout << 0;
        return 0;
    }

    char next, pre = -1;
    int n = 0;
    in.get(next);
    while (!in.eof())
    {
        if (next == ' ' && isalnum(pre))
            n++;
        else if (next == '\n')
        {
            n++;
            out << ' ' << n;
            n = 0;
        }
        n++;
        out << " " << n;
        in.close();
        out.close();
        return 0;
    }

    // ifstream in("file.txt", ios::binary);
    // char st[100] = "here egg salad\0";
    // int i{0};
    // in.read(st + 5, 4);
    // cout << st << endl;
    // in.close();

    // ifstream fin("file.txt", ios::binary);
    // ofstream fout("file.txt", ios::binary);

    // fin.close();
    // fout.close();
}