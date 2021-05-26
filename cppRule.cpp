#include<iostream>
#include<vector>
#include<string>
#include<cstring>
#include "cppRule.h"

extern "C"{
    void connect(){std::cout << "Connected to CPP extension...\n";}

    class Transaction{
        private:
            unsigned long long dateTime;
            float amt;
            std::string channel;
            std::string behavior;
        public:
            Transaction(unsigned long long dateTime, float amt, std::string channel, std::string behavior){
                this->dateTime = dateTime;
                this->amt = amt;
                this->channel = channel;
                this->behavior = behavior;
            }
            ~Transaction(){}
            unsigned long long GetDateTime(){return this->dateTime;}
            float GetAmount(){return this->amt;}
            std::string GetChannel(){return this->channel;}
            std::string GetBehavior(){return this->behavior;}
    };
    Transaction *NewTransaction(unsigned long long dateTime, float amt, char * const channelPtr, char * const behaviorPtr){return new Transaction(dateTime, amt, channelPtr, behaviorPtr);}
    unsigned long long TxGetDateTime(Transaction* txPtr){return (txPtr->GetDateTime());}
    float TxGetAmount(Transaction* txPtr){return (txPtr->GetAmount());}
    const char *TxGetChannelPtr(Transaction* txPtr){return (txPtr->GetChannel()).c_str();}
    const char *TxGetBehaviorPtr(Transaction* txPtr){return (txPtr->GetBehavior()).c_str();}

    class TransactionList{
        private:
            std::vector<Transaction*> txVec;
        public:
            TransactionList(){}
            void Append(Transaction *tx){
                this->txVec.push_back(tx);
            }
            Transaction *GetByIndex(int idx){
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
    TransactionList *NewTransactionList(){return new TransactionList();}
    void TxListAppend(TransactionList *txList, Transaction *tx){return txList->Append(tx);};
    Transaction *TxListGetByIndex(TransactionList *txList, int idx){return txList->GetByIndex(idx);}

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
                    std::cout << "index " << i+1
                              << ", date time:" << (txList->GetByIndex(i))->GetDateTime()
                              << ", amount:" << (txList->GetByIndex(i))->GetAmount()
                              << ", channel:" << (txList->GetByIndex(i))->GetChannel()
                              << std::endl;
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
    unsigned long long dateTime = 20210526100800;
    Transaction *tx1 = new Transaction(dateTime, 10, channel, behavior);
    Transaction *tx2 = new Transaction(dateTime, 20, channel, behavior);
    std::cout << "amount:"  << tx1->GetAmount()
              << ", date time:" << std::to_string(tx1->GetDateTime())
              << ", channel:" << tx1->GetChannel()
              << ", behavior:" << tx1->GetBehavior() << std::endl;
    TransactionList *txList = NewTransactionList();
    txList->Append(tx1);
    txList->Append(tx2);
    RunRule(rule, txList);
    return 0;
}