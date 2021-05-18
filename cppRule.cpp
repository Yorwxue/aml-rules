#include<iostream>
#include "cppHeader.h"

extern "C"
{
    void connect()
    {
        std::cout << "Connected to CPP extension...\n";
    }

    //return random value in range of 0-50
    int randNum()
    {
        int nRand = rand() % 50;
        return nRand;
    }

    //add two number and return value
    int addNum(int a, int b)
    {
        int nAdd = a + b;
        return nAdd;
    }
}