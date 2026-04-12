#pragma once

#include <functional>

class Dice{
private:
    int num1;
    int num2;
    std::function<int(int, int)> randFunc;

public:
    Dice();
    explicit Dice(std::function<int(int, int)> randFunc);

    void roll();
    void setManual(int x, int y);
    int  getTotal() const;
    bool isDouble() const;
    int  getDie1() const;
    int  getDie2() const;
};