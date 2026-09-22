#pragma once
#include <iostream>
#include <vector>

using namespace std;

// Implemented by devB
class Bar
{
};

class Ball
{
private:
    static Ball *ball;
    Ball() {}

public:
    static Ball *getBall()
    {
        if (ball == NULL)
            ball = new Ball();

        return ball;
    }

    static void deleteBall()
    {
        if (ball)
        {
            delete ball;
            ball = NULL;
        }
    }
};

class Brick
{
};

class Game
{
    vector<Brick *> bricks;
    Bar *bar;
    Ball *ball;
};