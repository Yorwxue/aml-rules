#include<iostream>
#include<vector>
#include<string>
#include<cstring>
#include "cppRule.h"

extern "C"{
    void connect(){std::cout << "Connected to CPP extension...\n";}

    class Transaction{
        private:
            float amt;
            std::string channel;
            std::string behavior;
        public:
            Transaction(float amt, std::string channel, std::string behavior){
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
    const char *GetChannelPtr(Transaction* txPtr){return (txPtr->GetChannel()).c_str();}
    const char *GetBehaviorPtr(Transaction* txPtr){return (txPtr->GetBehavior()).c_str();}

    class TransactionList{
        private:
            std::vector<Transaction*> txVec;
        public:
            void Append(Transaction *tx){
                this->txVec.push_back(tx);
            }
            Transaction* GetByIndex(int idx){
                int txVecSize = this->txVec.size();
                if(idx<txVecSize)
                    return this->txVec.at(idx);
                else
                    std::cout << "Index out of range(" << txVecSize << "), return the last element." << std::endl;
                    return this->txVec.back();
            }
            int GetSize(){
                return this->txVec.size();
            }
    };

    class Rule{
        private:
            float amtThresh;
        public:
            Rule(){amtThresh = 0;}
            Rule(const float amtThresh){this->amtThresh = amtThresh;}
            ~Rule(){}
            void Run(TransactionList* txList){
                std::cout << "Threshold of amount:" << this->amtThresh << std::endl;
                for(int i=0;i<txList->GetSize();i++){
                    std::cout << "index " << i+1 << ", amount:" << (txList->GetByIndex(i))->GetAmt() << std::endl;
                }
            }
    };
    Rule* NewRule(float amtThresh){return new Rule(amtThresh);}
    void RunRule(Rule* rulePtr, TransactionList *txList){rulePtr->Run(txList);}
}
int main(){
    // rule
    const float value = 10.0;
    Rule *rule = NewRule(value);

    // tx
    std::string channel("IBMB");  // IBMB, Other Bank, Oversea
    std::string behavior("轉入");  // 轉入, 轉出, 轉帳, 存款, 提款
    Transaction *tx1 = new Transaction(10, channel, behavior);
    Transaction *tx2 = new Transaction(20, channel, behavior);
    std::cout << "amount:"  << tx1->GetAmt()
              << ", channel:" << tx1->GetChannel()
              << ", behavior:" << tx1->GetBehavior() << std::endl;
    TransactionList *txList = new TransactionList;
    txList->Append(tx1);
    txList->Append(tx2);
    RunRule(rule, txList);

    return 0;
}