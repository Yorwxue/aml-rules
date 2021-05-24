extern "C" void connect();
extern "C" class Rule;
extern "C" Rule* NewRule(float amtThresh_in);
extern "C" void RunRule(Rule* rulePtr);
extern "C" class Transaction;
extern "C" Transaction* NewTransaction(float amt, char * const channel, char * const behavior);
extern "C" void ShowChannel(Transaction* txPtr);
extern "C" const char *GetChannelPtr(Transaction* txPtr);
extern "C" const char *GetBehaviorPtr(Transaction* txPtr);