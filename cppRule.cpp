#include<iostream>
#include<string>
#include<cstring>
#include "cppRule.h"

extern "C"
{
    void connect(){std::cout << "Connected to CPP extension...\n";}

    class Transaction
    {
        private:
            float amt;
            std::string channel;  // IBMB, Other Bank, Oversea
            std::string behavior;  // 轉入, 轉出, 轉帳, 存款, 提款
//            int PCODE;
//            std::string accountNo;
//            std::string counterPartyAccountNo;
        public:
            Transaction(float amt, std::string channel, std::string behavior)
            {
                this->amt = amt;
                this->channel = channel;
                this->behavior = behavior;
            }
            ~Transaction(){}
            float GetAmt(){return this->amt;}
            std::string GetChannel(){return this->channel;}
            std::string GetBehavior(){return this->behavior;}
    };

    Transaction *NewTransaction(float amt, char * const channelPtr, char * const behaviorPtr){return new Transaction(0, channelPtr, behaviorPtr);}
    void ShowChannel(Transaction* txPtr){std::cout << txPtr->GetChannel();}
    const char *GetChannelPtr(Transaction* txPtr){return (txPtr->GetChannel()).c_str();}
    const char *GetBehaviorPtr(Transaction* txPtr){return (txPtr->GetBehavior()).c_str();}


    class Rule
    {
        private:
            float amtThresh;
        public:
            Rule(){amtThresh = 0;}
            Rule(const float amtThresh){this->amtThresh = amtThresh;}
            ~Rule(){}
            void run()
            {
                std::cout << "Threshold of amount:" << this->amtThresh << std::endl;
            }
    };
    Rule* NewRule(float amtThresh){return new Rule(amtThresh);}
    void RunRule(Rule* rulePtr){rulePtr->run();}
}
int main()
{
    // rule
    const float value = 10.0;
//    Rule *rule = new Rule(value);
    Rule *rule = NewRule(value);
//    rule->run();
    RunRule(rule);
    delete rule;

    // tx
    std::string channel("IBMB");  // IBMB, Other Bank, Oversea
    std::string behavior("轉入");  // 轉入, 轉出, 轉帳, 存款, 提款
    Transaction *tx = new Transaction(10, channel, behavior);
    std::cout << "amount:"  << tx->GetAmt()
              << ", channel:" << tx->GetChannel()
              << ", behavior:" << tx->GetBehavior() << std::endl;

    return 0;
}