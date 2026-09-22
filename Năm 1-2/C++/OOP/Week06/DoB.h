#pragma once

class DoB
{
protected:
    short day;
    short month;
    short year;

public:
    DoB(): day(1), month(1), year(1){}
    DoB(short d, short m, short y) : day(d), month(m), year(y){}

    short getBirthMonth();

};

short DoB::getBirthMonth()
{
    return month;
}