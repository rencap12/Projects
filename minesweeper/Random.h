#ifndef UNTITLED12_RANDOM_H
#define UNTITLED12_RANDOM_H
#include <random>


class Random {
    static std::mt19937 random;
public:
    static int Int(int min, int max);
};


#endif //UNTITLED12_RANDOM_H
