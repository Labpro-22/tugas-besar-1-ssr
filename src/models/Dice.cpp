
#include "Dice.hpp"
#include <cstdlib>
#include <ctime>

Dice::Dice() : num1(1), num2(1) {
    std::srand(std::time(nullptr));
    randFunc = [](int min, int max) { return min + (std::rand() % (max - min + 1)); };
}

Dice::Dice(std::function<int(int, int)> randFunc) : num1(1), num2(1), randFunc(randFunc) {}

void Dice::roll() {
    num1 = randFunc(1, 6);
    num2 = randFunc(1, 6);
}

void Dice::setManual(int x, int y) {
    num1 = x;
    num2 = y;
}

int Dice::getTotal() const { return num1 + num2; }
bool Dice::isDouble() const { return num1 == num2; }
int Dice::getDie1() const { return num1; }
int Dice::getDie2() const { return num2; }