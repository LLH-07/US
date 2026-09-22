#include "Ball.h"
#include "Circuit.h"

using namespace std;

int main()
{

    // Bai 02
    Single_Circuit c1(1, 2);
    Single_Circuit c2(3, 4);
    Single_Circuit c3(5, 6);
    Single_Circuit c4(7, 8);
    Single_Circuit c5(9, 10);

    Series_Circuit series(5);
    series.addCirCuit(&c1);
    series.addCirCuit(&c2);
    series.addCirCuit(&c3);
    series.addCirCuit(&c4);
    series.addCirCuit(&c5);

    Parallel_Circuit parallel(10);
    parallel.addCirCuit(&c1);
    parallel.addCirCuit(&c2);
    parallel.addCirCuit(&c3);
    parallel.addCirCuit(&c4);
    parallel.addCirCuit(&c5);

    cout << "Series circuit:\n";
    cout << "Voltage: " << series.getVoltage() << endl;
    cout << "Current: " << series.getCurrent() << endl;
    cout << "Resistance: " << series.getResistance() << endl;

    cout << endl;;

    cout << "Parallel circuit:\n";
    cout << "Voltage: " << parallel.getVoltage() << endl;
    cout << "Current: " << parallel.getCurrent() << endl;
    cout << "Resistance: " << parallel.getResistance() << endl;
}