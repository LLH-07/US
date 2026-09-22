#include <iostream>
#include <cmath>

using namespace std;

// 0.25, 0.1, 0.05, 0.01

bool question3(float dollar, int noc)
{
    if(dollar == 0 && noc == 0)
        return true;

    if(noc <= 0)
        return false;
 
    if(dollar >= 0.25)
        return question3(dollar - 0.25, noc - 1);

    if(dollar >= 0.1)
        return question3(dollar - 0.1, noc - 1);

    if(dollar >= 0.05)
        return question3(dollar - 0.05, noc - 1);

    if(dollar >= 0.01)
        return question3(dollar - 0.01, noc - 1);

    return false;
}