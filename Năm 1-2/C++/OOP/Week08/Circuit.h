#pragma once
#include <iostream>
#include <vector>

using namespace std;

class Single_Circuit
{
protected:
    //double resistance;
    double voltage;
    double current;

public:
    Single_Circuit(double v, double i) : voltage(v), current(i){}

    virtual double getResistance()
    {
        return voltage / current;
    }
    virtual double getVoltage()
    {
        return this->voltage;
    }
    virtual double getCurrent()
    {
        return this->current;
    }
    virtual void addCircuit(Single_Circuit *circuit) {}
    virtual ~Single_Circuit(){}
};

class Series_Circuit : public Single_Circuit
{
private:
    vector<Single_Circuit*> circuits;
public:
    Series_Circuit(double i):Single_Circuit(0, i){}

    double getResistance()
    {
        double totalResistance = 0;
        for(int i = 0; i < circuits.size(); i++)
            totalResistance += circuits[i]->getResistance();

        return totalResistance;
    }

    double getVoltage()
    {
        double totalVoltage = 0;
        for(int i = 0; i < circuits.size(); i++)
            totalVoltage += circuits[i]->getVoltage();

        return totalVoltage;
    }

    double getCurrent()
    {
        return this->current;
    }

    void addCirCuit(Single_Circuit* circuit)
    {
        circuits.push_back(circuit);
    }

    ~Series_Circuit(){}

};


class Parallel_Circuit : public Single_Circuit
{
private:
    vector<Single_Circuit*> circuits;
public:
    Parallel_Circuit(double v):Single_Circuit(v, 0){}

    double getResistance()
    {
        double reverseTotalResistance = 0;
        for(int i = 0; i < circuits.size(); i++)
            reverseTotalResistance += (1.0 / circuits[i]->getResistance());

        return 1.0 / reverseTotalResistance;
    }

    double getVoltage()
    {
        return this->voltage;
    }

    double getCurrent()
    {
        double totalCurrent = 0;
        for(int i = 0; i < circuits.size(); i++)
            totalCurrent += circuits[i]->getCurrent();

        return totalCurrent;
    }

    void addCirCuit(Single_Circuit* circuit)
    {
        circuits.push_back(circuit);
    }

    ~Parallel_Circuit(){}
};


