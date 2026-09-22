#include <iostream>
#include <time.h>
#include <cmath>
#include <fstream>

using namespace std;

struct circle
{
    int x, y;
    int r;
};

struct listCircle
{
    circle c1;
    circle c2;
};

void write(int n)
{
    fstream out("circles.bin", ios::out | ios::binary);
    if (!out.is_open())
    {
        cout << "File could not be opened!";
        return;
    }

    circle *C = new circle[n];
    for (int i = 0; i < n; i++)
    {
        C[i].x = rand();
        C[i].y = rand();
        C[i].r = rand();
        out.write((char *)&C[i], sizeof(C[i]));
    }
    out.close();
    delete[] C;
}

void read(circle c[], int n)
{
    fstream in("circles.bin", ios::in | ios::binary);
    if (!in.is_open())
    {
        cout << "File could not be opened!";
        return;
    }

    for (int i = 0; i < n; i++)
        in.read((char *)&c[i], sizeof(c[i]));

    in.close();
}

double distance(circle c1, circle c2)
{
    return sqrt((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y));
}

bool isCut(circle c1, circle c2)
{
    double d = distance(c1, c2);
    return ((c1.r + c2.r == d) || abs(c1.r - c2.r) == d || abs(c1.r - c2.r) < d && d < (c1.r + c2.r));
}

void saveToList(circle c[], listCircle l[], int n, int &num)
{
    num = 0;
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (isCut(c[i], c[j]))
            {
                l[num].c1 = c[i];
                l[num].c2 = c[j];
                num++;
            }
        }
    }
}

void saveToTXT(listCircle l[], int num)
{
    ofstream out("cuts.txt");
    if (!out.is_open())
    {
        cout << "File could not be opened!";
        return;
    }

    for (int i = 0; i < num; i++)
    {
        out << l[i].c1.x << ' ' << l[i].c1.y << ' ' << l[i].c1.r << " , ";
        out << l[i].c2.x << ' ' << l[i].c2.y << ' ' << l[i].c2.r << endl;
    }
}

int main()
{
    int n;
    srand(time(NULL));
    n = rand() % 900 + 101;

    write(n);

    circle *C = new circle[n];
    int num;
    listCircle *l = new listCircle[n * n];

    read(C, n);
    saveToList(C, l, n, num);
    saveToTXT(l, num);

    delete[] C;
    delete[] l;
    return 0;
}