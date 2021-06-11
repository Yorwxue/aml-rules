extern "C" void connect();
extern "C" class Transaction;
// Transaction
extern "C" Transaction* NewTransaction(unsigned long long dateTime, float amt, char * const channel, char * const behavior);
extern "C" unsigned long long TxGetDateTime(Transaction* txPtr);
extern "C" float TxGetAmount(Transaction* txPtr);
extern "C" const char *TxGetChannel(Transaction* txPtr);
extern "C" const char *TxGetBehavior(Transaction* txPtr);
// TransactionList
extern "C" void *NewTransactionList();
extern "C" void TxListAppend(void *txList, Transaction *tx);
extern "C" Transaction *TxListGetDataByIndex(void *txList, int idx);
extern "C" int GetTxListSize(void *list);
extern "C" int GetTxListCapacity(void *list);
extern "C" void TxListAddCapacity(void *list, int num);
// StringList
extern "C" void *NewStringList();
extern "C" void StringListAppend(void *list, char * const newData);
extern "C" int GetStringListSize(void *list);
extern "C" const char *StringListGetDataByIndex(void *list, int idx);
// Rule
extern "C" class Rule;
extern "C" Rule* NewRule(float amtThresh_in, int timesThresh);
extern "C" bool RunRule(Rule* rulePtr, void *txList, unsigned long long dateTimeStart);
extern "C" float RuleGetAmtThresh(Rule* rulePtr);
extern "C" unsigned long long RuleGetStartDateTime(Rule* rulePtr);
extern "C" class RuleA1;
extern "C" Rule* NewRuleA1(float amtThresh_in, int timesThresh);
// Rule Pipeline
extern "C" void *RulePipelineDateTimeFilter(void *list, unsigned long long dateTimeStart, unsigned long long dateTimeEnd);
extern "C" void *RulePipelineAmountThreshFilter(void *list, unsigned int *TxIndexArray, float amtThresh, Transaction *newTx);
extern "C" void *RulePipelineConditionMatchFilter(void *list, char * const fieldName, void *condList);
