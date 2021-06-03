#include<iostream>
#include<vector>
#include<string>
#include<cstring>
#include "cppRule.h"


/******************** ctypes not support template *******************/
template <class T>
class List{
    public:
        std::vector<T> Vec;  // technically transactions are been append by the order of date-time(FIFO like)
        List(){}
        void Append(T newData){
            this->Vec.push_back(newData);
        }
        T GetByIndex(int idx){
            int VecSize = this->Vec.size();
            if(idx<VecSize)
                return this->Vec.at(idx);
            else
                std::cout << "Index out of range(" << VecSize << "), return the last element." << std::endl;
                return this->Vec.back();
        }
        int GetSize(){
            return this->Vec.size();
        }
};
/********************************************************************
templat <class T>
void ListAppend(List<T> *list, T newData){return list->Append(newData);};
templat <class T>
T ListGetByIndex(List<T>, *list int idx){return list->GetByIndex(idx);}
/*********************************************************************/

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

    /*
    class TransactionList{
        public:
            std::vector<Transaction*> Vec;  // technically transactions are been append by the order of date-time(FIFO like)
        public:
            TransactionList(){}
            void Append(Transaction *tx){
                this->Vec.push_back(tx);
            }
            Transaction *GetByIndex(int idx){
                int VecSize = this->Vec.size();
                if(idx<VecSize)
                    return this->Vec.at(idx);
                else
                    std::cout << "Index out of range(" << VecSize << "), return the last element." << std::endl;
                    return this->Vec.back();
            }
            int GetSize(){
                return this->Vec.size();
            }
    };
    TransactionList *NewTransactionList(){return new TransactionList();}
    void TxListAppend(TransactionList *txList, Transaction *tx){return txList->Append(tx);};
    Transaction *TxListGetByIndex(TransactionList *txList, int idx){return txList->GetByIndex(idx);}
    */
    // transaction list
    void *NewTransactionList(){return (void *)(new List<Transaction *>());}
    void TxListAppend(void *list, Transaction *newData){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        txList->Append(newData);
    };
    Transaction *TxListGetByIndex(void *list, int idx){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        return txList->GetByIndex(idx);
    }

    // index list
    void *NewIntList(){return (void *)(new List<int *>());}
    void IntListAppend(void *list, int *newData){
        List<int *> *intList = static_cast<List<int *> *>(list);
        intList->Append(newData);
    }
    int *IntListGetByIndex(void *list, int idx){
        List<int *> *intList = static_cast<List<int *> *>(list);
        return intList->GetByIndex(idx);
    }

    /* The following rules are executed individually, */
    class Rule{
        protected:
            float amtThresh;
            int timesThresh;
        public:
            Rule():amtThresh(0), timesThresh(1){}
            Rule(const float amtThresh){this->amtThresh=amtThresh; this->timesThresh=1;}
            Rule(const int timesThresh){this->amtThresh=0; this->timesThresh=timesThresh;}
            Rule(const float amtThresh, const int timesThresh){this->amtThresh=amtThresh; this->timesThresh=timesThresh;}
            ~Rule(){}
            float GetAmtThresh(){return this->amtThresh;}
//            virtual bool Run(TransactionList* txList, unsigned long long dateTimeStart){
            virtual bool Run(void* list, unsigned long long dateTimeStart){
                List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
                int count = 0;
                std::vector<Transaction*>::iterator txPtr;
                int i;
                for(i=0, txPtr=(txList->Vec).begin();
                    txPtr!=(txList->Vec).end() &&
                        (*txPtr)->GetDateTime()>=dateTimeStart &&
                        (*txPtr)->GetAmount()>=this->amtThresh;
                    txPtr++, i++){
//                    std::cout << "index " << i+1
//                              << ", date time:" << (*txPtr)->GetDateTime()
//                              << ", amount:" << (*txPtr)->GetAmount()
//                              << ", channel:" << (*txPtr)->GetChannel()
//                              << std::endl;
                    count++;
                }
                if(count>=this->timesThresh)  // trigger
                    return true;
                else
                    return false;
            }
    };
    Rule* NewRule(float amtThresh, int timesThresh){return new Rule(amtThresh, timesThresh);}
//    bool RunRule(Rule* rulePtr, TransactionList *txList, unsigned long long dateTimeStart){return rulePtr->Run(txList, dateTimeStart);}
    bool RunRule(Rule* rulePtr, void *txList, unsigned long long dateTimeStart){return rulePtr->Run(txList, dateTimeStart);}
    float RuleGetAmtThresh(Rule* rulePtr){return rulePtr->GetAmtThresh();}

    /**************** inherit from Rule ***************/
    class RuleA1:public Rule{
        public:
            RuleA1(const float amtThresh, const int timesThresh):Rule(amtThresh, timesThresh){}
             bool Run(void* list, unsigned long long dateTimeStart) override{
                List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
                std::cout << "Here is RuleA1" << std::endl;
                return true;
             }
    };
    Rule* NewRuleA1(float amtThresh, int timesThresh){return new RuleA1(amtThresh, timesThresh);}
    /**************************************************/

    /* Here is a prototype of rule pipeline, each member function may be one stage for several rules *
    class RulePipeline{
        protected:
        public:
            unsigned int *DateTimeFilter(TransactionList *txList, unsigned long long dateTimeStart, Transaction *newTx){
                unsigned int *MatchedTxIndexArray = new unsigned int[5];
                std::vector<Transaction*>::iterator txPtr;
                int i, count=0;
                for(i=0, txPtr=(txList->Vec).begin();
                    txPtr!=(txList->Vec).end() &&
                        (*txPtr)->GetDateTime()>=dateTimeStart;
                    txPtr++, i++){
                        MatchedTxIndexArray[count++] = i;
                }
                return MatchedTxIndexArray;
            }
            unsigned int *AmountThreshFilter(TransactionList *txList, unsigned int *TxIndexArray, float amtThresh, Transaction *newTx){
                unsigned int *MatchedTxIndexArray = new unsigned int[5];
                std::vector<Transaction*>::iterator txPtr;
                int i, count=0;
                for(i=0, txPtr=(txList->Vec).begin();
                    txPtr!=(txList->Vec).end() &&
                        (*txPtr)->GetAmount()>=amtThresh;
                    txPtr++, i++){
                        MatchedTxIndexArray[count++] = i;
                }
                return MatchedTxIndexArray;
            }
    };
    /*************************************************************************************************/
}
int main(){
    //example of data
    const float value = 10.0;
    unsigned long long dateTime = 20210526100800;
    std::string channel("IBMB");  // IBMB, Other Bank, Oversea
    std::string behavior("轉入");  // 轉入, 轉出, 轉帳, 存款, 提款

    // rule
    Rule *rule = new Rule(dateTime, value);

    // tx
    Transaction *tx1 = new Transaction(dateTime, 10, channel, behavior);
    Transaction *tx2 = new Transaction(dateTime, 20, channel, behavior);
    std::cout << "amount:"  << tx1->GetAmount()
              << ", date time:" << std::to_string(tx1->GetDateTime())
              << ", channel:" << tx1->GetChannel()
              << ", behavior:" << tx1->GetBehavior() << std::endl;
//    TransactionList *txList = NewTransactionList();
    List<Transaction *> *txList = static_cast<List<Transaction *> *>(NewTransactionList());
    txList->Append(tx1);
    txList->Append(tx2);
    RunRule(rule, (void *)txList, dateTime);
    return 0;
}