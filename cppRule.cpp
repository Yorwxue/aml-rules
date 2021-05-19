#include<iostream>
#include "cppRule.h"

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

    class Rule
    {
        private:
        float amtThresh;
        public:
            Rule(){amtThresh = 0;}
            Rule(const float amtThresh_in){amtThresh = amtThresh_in;}
            ~Rule(){}
            void run()
            {
                std::cout << "Threshold of amount:" << amtThresh << std::endl;
            }
    };

    Rule* NewRule(float amtThresh_in)
    {
        return new Rule(amtThresh_in);
    }

    void RunRule(Rule* rulePtr)
    {
        rulePtr->run();
    }
}
int main()
{
    const float value = 10.0;
//    Rule *rule = new Rule(value);
    Rule *rule = NewRule(value);
//    rule->run();
    RunRule(rule);
    delete rule;
    return 0;
}