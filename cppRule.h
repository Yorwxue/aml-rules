extern "C" void connect();
extern "C" class Transaction;
extern "C" Transaction* NewTransaction(float amt, char * const channel, char * const behavior);
extern "C" const char *GetChannelPtr(Transaction* txPtr);
extern "C" const char *GetBehaviorPtr(Transaction* txPtr);
extern "C" class TransactionList;
extern "C" class Rule;
extern "C" Rule* NewRule(float amtThresh_in);
extern "C" void RunRule(Rule* rulePtr, TransactionList *txList);
