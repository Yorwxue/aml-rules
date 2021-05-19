extern "C" void connect();
extern "C" int randNum();
extern "C" int addNum(int a, int b);
extern "C" class Rule;
extern "C" Rule* NewRule(float amtThresh_in);
extern "C" void RunRule(Rule* rulePtr);