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
        ~List(){}
        void Append(T newData){
            this->Vec.push_back(newData);
        }
        T GetDataByIndex(int idx){
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
        int GetCapacity(){
            return this->Vec.capacity();
        }
        void AppCapacity(int num){
            this->Vec.reserve(num+GetCapacity());
        }
        void PopBack(){this->vec.pop_back();}
        void PopFront(){this->vec.pop_front();}
        void Drop(int idx){
            this->Vec.erase(this->Vec.begin()+idx);
        }
        void Drops(int startIdx, int endIdx){
            this->Vec.erase(this->Vec.begin()+startIdx, this->Vec.begin()+endIdx);
        }
};

/************************ interface for ctypes ****************************/
extern "C"{
    void connect(){std::cout << "Connected to CPP extension...\n";}

    class Transaction{
        private:
            unsigned long long dateTime;
            float amt;
            std::string channel;
            std::string behavior;
        public:
        Transaction(){this->amt=0;}
        Transaction(float amt){this->amt=amt;}
            Transaction(unsigned long long dateTime, float amt, std::string channel, std::string behavior){
                this->dateTime = dateTime;
                this->amt = amt;
                this->channel = channel;
                this->behavior = behavior;
            }
            ~Transaction(){}
            Transaction operator+(const Transaction&);
            unsigned long long GetDateTime(){return this->dateTime;}
            float GetAmount(){return this->amt;}
            std::string GetChannel(){return this->channel;}
            std::string GetBehavior(){return this->behavior;}
    };
    Transaction Transaction::operator+(const Transaction &that){
        return Transaction(this->amt+that.amt);
    }
    Transaction *NewTransaction(unsigned long long dateTime, float amt, char * const channelPtr, char * const behaviorPtr){return new Transaction(dateTime, amt, channelPtr, behaviorPtr);}
    unsigned long long TxGetDateTime(Transaction* txPtr){return (txPtr->GetDateTime());}
    float TxGetAmount(Transaction* txPtr){return (txPtr->GetAmount());}
    const char *TxGetChannel(Transaction* txPtr){return (txPtr->GetChannel()).c_str();}
    const char *TxGetBehavior(Transaction* txPtr){return (txPtr->GetBehavior()).c_str();}


    /*************** transaction list ***************/
    void *NewTransactionList(){return (void *)(new List<Transaction *>());}
    void TxListAppend(void *list, Transaction *newData){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        txList->Append(newData);
    }
    Transaction *TxListGetDataByIndex(void *list, int idx){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        return txList->GetDataByIndex(idx);
    }
    int GetTxListSize(void *list){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        return txList->GetSize();
    }
    int GetTxListCapacity(void *list){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        return txList->GetCapacity();
    }
    void TxListAddCapacity(void *list, int num){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        txList->AppCapacity(num);
    }
    Transaction *TxListSum(void *list){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        Transaction txTemp, *txSum = new Transaction();
        std::vector<Transaction*>::iterator txPtr;
        for(txPtr=(txList->Vec).begin(); txPtr!=(txList->Vec).end(); txPtr++){
            txTemp = *txSum + (*(*txPtr));
            txSum = std::addressof(txTemp);
        }
        return txSum;
    }
    void TxListDrop(void *list, int idx){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        Transaction *txPtr = txList->GetDataByIndex(idx);
        delete txPtr;
        txList->Drop(idx);
    }
    void TxListDrops(void *list, int startIdx, int endIdx){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        if(endIdx>txList->GetSize()){endIdx=txList->GetSize();}
        for(std::vector<Transaction *>::iterator txPtr=txList->Vec.begin()+startIdx; txPtr!=txList->Vec.begin()+endIdx; txPtr++){
            delete *txPtr;
        }
        txList->Drops(startIdx, endIdx);
    }

    /*************** transaction 2D-list ***************/
    void *Tx2DListGetDataByIndex(void *list, int idx){
        List<List<Transaction *> *> *tx2DList = static_cast<List<List<Transaction *> *> *>(list);
        return tx2DList->GetDataByIndex(idx);
    }
    int GetTx2DListSize(void *list){
        List<List<Transaction *> *> *tx2DList = static_cast<List<List<Transaction *> *> *>(list);
        return tx2DList->GetSize();
    }

    /*************** string list ***************/
    void *NewStringList(){return (void *)(new List<std::string>());}
    void StringListAppend(void *list, char * const newData){
        List<std::string> *stringList = static_cast<List<std::string> *>(list);
        stringList->Append(newData);
    }
    int GetStringListSize(void *list){
        List<std::string> *stringList = static_cast<List<std::string> *>(list);
        return stringList->GetSize();
    }
    const char *StringListGetDataByIndex(void *list, int idx){
        List<std::string> *stringList = static_cast<List<std::string> *>(list);
        return (stringList->GetDataByIndex(idx)).c_str();
    }

    /* The following rules are executed individually */
    /**************** Rule class: a prototype for rules ***************/
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
    float RuleGetAmtThresh(Rule* rulePtr){return rulePtr->GetAmtThresh();}

    /**************** common function for rule ***************/
    bool RunRule(Rule* rulePtr, void *txList, unsigned long long dateTimeStart){return rulePtr->Run(txList, dateTimeStart);}

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

    /* a prototype of rule pipeline, each function may be one stage for several rules */
    void *RulePipelineDateTimeFilter(void *list, unsigned long long dateTimeStart, unsigned long long dateTimeEnd){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        List<Transaction *> *MatchedTxList = new List<Transaction *>();
        std::vector<Transaction*>::iterator txPtr;
        for(txPtr=(txList->Vec).begin();
            txPtr!=(txList->Vec).end() &&
                (*txPtr)->GetDateTime()<=dateTimeEnd;  // technically transactions are been append by the order of date-time(FIFO like)
            txPtr++){
            if((*txPtr)->GetDateTime()>=dateTimeStart)
                MatchedTxList->Append(*txPtr);
        }
        return (void *)MatchedTxList;
    }
    void *RulePipelineAmountThreshFilter(void *list, unsigned int *TxIndexArray, float amtThresh, Transaction *newTx){
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        List<Transaction *> *MatchedTxList = new List<Transaction *>();
        std::vector<Transaction*>::iterator txPtr;
        for(txPtr=(txList->Vec).begin();
            txPtr!=(txList->Vec).end() &&
                (*txPtr)->GetAmount()>=amtThresh;
            txPtr++){
                MatchedTxList->Append(*txPtr);
        }
        return (void *)MatchedTxList;
    }
    void *RulePipelineConditionMatchFilter(void *list, char * const fieldName, void *condList){
        ////////////////////////////////////////////////////////////////
        // :param void *condList: condList is a List<std::string> object with string element which should be matched as conditions
        // :param char * const fieldName: a string to select field for comparing with conditions
        // :return: list of list of Transaction, number of elements in first layer is decided by number of elements in
        //          condList, and the number of elements in second layer is decided by how many transactions fit the condition
        ////////////////////////////////////////////////////////////////
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        List<std::string> *condStringList = static_cast<List<std::string> *>(condList);
        std::string fieldNameString(fieldName);
        // create list
        List<List<Transaction *> *> *MatchedTx2DList = new List<List<Transaction *> *>();
        int numConditions = condStringList->GetSize();
        for(int i=0; i<numConditions; i++){
            MatchedTx2DList->Append(new List<Transaction *>());
        }
        //go through transactions and compare
        if(fieldNameString=="channel"){}
        else if(fieldNameString=="behavior"){
            std::vector<Transaction*>::iterator txPtr;
            for(txPtr=(txList->Vec).begin();
                txPtr!=(txList->Vec).end();
                txPtr++)
            {
                for(int i=0; i<numConditions; i++){
                    if((*txPtr)->GetBehavior()==(condStringList->GetDataByIndex(i))){
//                            std::cout << "Found transaction match behavior:" << condStringList->GetDataByIndex(i)
//                                      << ", DateTime:" << (*txPtr)->GetDateTime()
//                                      << ", Amount:" << (*txPtr)->GetAmount()
//                                      << ", Channel:" << (*txPtr)->GetChannel()
//                                      << std::endl;
                        MatchedTx2DList->GetDataByIndex(i)->Append(*txPtr);
                    }
                }
            }
        }
        else
            std::cout << "Invalid Field Name" << ", should be \"channel\" or \"behavior\""
                      << ", but got \"" << fieldNameString << "\"" << std::endl;

        return (void *)MatchedTx2DList;
    }
    void *RulePipelineConditionExcludeFilter(void *list, char * const fieldName, void *condList){
        ////////////////////////////////////////////////////////////////
        // :param void *condList: condList is a List<std::string> object with string element which should be excluded as conditions
        // :param char * const fieldName: a string to select field for comparing with conditions
        // :return: list of Transactions without particular element in specific field
        ////////////////////////////////////////////////////////////////
        List<Transaction *> *txList = static_cast<List<Transaction *> *>(list);
        List<std::string> *condStringList = static_cast<List<std::string> *>(condList);
        std::string fieldNameString(fieldName);
        // create list
        List<Transaction *> *MatchedTxList = new List<Transaction *>();
        int numConditions = condStringList->GetSize();
        //go through transactions and compare
        if(fieldNameString=="channel"){}
        else if(fieldNameString=="behavior"){
            std::vector<Transaction*>::iterator txPtr;
            for(txPtr=(txList->Vec).begin(); txPtr!=(txList->Vec).end(); txPtr++){
                bool matchFlag = true;
                for(int i=0; i<numConditions; i++){
                    if((*txPtr)->GetBehavior()==(condStringList->GetDataByIndex(i))){
                        matchFlag = false;
                    }
                }
                if(matchFlag)
                    MatchedTxList->Append(*txPtr);
            }
        }
        else
            std::cout << "Invalid Field Name" << ", should be \"channel\" or \"behavior\""
                      << ", but got \"" << fieldNameString << "\"" << std::endl;

        return (void *)MatchedTxList;
    }
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
    Transaction tx3_ = *tx1 + *tx2;
    Transaction *tx3 = std::addressof(tx3_);
    std::cout << "tx1:: amount:"  << tx1->GetAmount()
              << ", date time:" << std::to_string(tx1->GetDateTime())
              << ", channel:" << tx1->GetChannel()
              << ", behavior:" << tx1->GetBehavior() << std::endl;
    std::cout << "tx2:: amount:"  << tx2->GetAmount()
              << ", date time:" << std::to_string(tx2->GetDateTime())
              << ", channel:" << tx2->GetChannel()
              << ", behavior:" << tx2->GetBehavior() << std::endl;
    std::cout << "tx1+tx2:: amount:" << tx3->GetAmount() << std::endl;
    std::cout << "===================================" << std::endl;

    // create list
    List<Transaction *> *txList = static_cast<List<Transaction *> *>(NewTransactionList());
    txList->Append(tx1);
    txList->Append(tx2);

    // print all list
    std::cout << "List all" << std::endl;
    int i = 1;
    for(std::vector<Transaction*>::iterator txPtr=(txList->Vec).begin(); txPtr!=(txList->Vec).end(); txPtr++){
        std::cout << "tx"<< i++ <<":: amount:"  << (*txPtr)->GetAmount()
                  << ", date time:" << std::to_string((*txPtr)->GetDateTime())
                  << ", channel:" << (*txPtr)->GetChannel()
                  << ", behavior:" << (*txPtr)->GetBehavior() << std::endl;
    }

    RunRule(rule, (void *)txList, dateTime);
    std::cout << "Sum of txList:" << TxListSum(txList)->GetAmount() << std::endl;

    // test drop
    std::cout << "List all after drop element in index:0" << std::endl;
//    TxListDrop(txList, 0);
    TxListDrops(txList, 0, 1);
    i = 1;
    for(std::vector<Transaction*>::iterator txPtr=(txList->Vec).begin(); txPtr!=(txList->Vec).end(); txPtr++){
        std::cout << "tx"<< i++ <<":: amount:"  << (*txPtr)->GetAmount()
                  << ", date time:" << std::to_string((*txPtr)->GetDateTime())
                  << ", channel:" << (*txPtr)->GetChannel()
                  << ", behavior:" << (*txPtr)->GetBehavior() << std::endl;
    }

    return 0;
}