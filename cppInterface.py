import os
import ctypes
import datetime
import time


libRule = ctypes.CDLL("./librule.so")

# call C function to check connection
print(libRule)
libRule.connect()

# return setting of c based function
# TransactionList
# libRule.GetTxListSize.restype = ctypes.c_int  # unnecessary
# libRule.GetTxListCapacity.restype = ctypes.c_int  # unnecessary
# Transaction
libRule.TxGetAmount.restype = ctypes.c_float
libRule.TxGetDateTime.restype = ctypes.c_ulonglong
libRule.TxGetChannel.restype = ctypes.c_char_p
libRule.TxGetBehavior.restype = ctypes.c_char_p
# Rule
libRule.RuleGetAmtThresh.restype = ctypes.c_float
libRule.RunRule.restype = ctypes.c_bool
# StringList
libRule.StringListGetDataByIndex.restype = ctypes.c_char_p


class Transaction(object):
    def __init__(self, dateTime=None, amt=None, channel=None, behavior=None, txPtr=None):
        if txPtr:
            self.obj = txPtr
        elif dateTime and amt and channel and behavior:
            self.obj = libRule.NewTransaction(dateTime, ctypes.c_float(amt), ctypes.c_char_p(channel.encode('utf-8')), ctypes.c_char_p(behavior.encode('utf-8')))
        else:
            raise ValueError("Invalid parameters for constructor")

    def GetDateTime(self):
        return libRule.TxGetDateTime(self.obj)

    def GetAmount(self):
        return libRule.TxGetAmount(self.obj)

    def GetChannel(self):
        return libRule.TxGetChannel(self.obj).decode("utf-8")

    def GetBehavior(self):
        return libRule.TxGetBehavior(self.obj).decode("utf-8")


class TransactionList(object):
    def __init__(self, txList=None, reserveSize=150000, txPtrList=None):
        if txPtrList:
            self.obj = txPtrList
        else:
            self.obj = libRule.NewTransactionList()
            self.reserveSize = reserveSize
            if txList:
                self.AppendMany(txList)

    def AddCapacity(self, num):
        # raise NotImplementedError("")
        libRule.TxListAddCapacity(self.obj, num)

    def Append(self, newTxPtr):
        libRule.TxListAppend(self.obj, newTxPtr)

    def AppendMany(self, txList):
        length = len(txList)
        available = self.GetCapacity() - self.GetSize()
        if length > available:
            self.AddCapacity(self.reserveSize + length)
        for tx in txList:
            txPtr = tx.obj
            self.Append(txPtr)

    def GetDataByIndex(self, idx):
        return libRule.TxListGetDataByIndex(self.obj, idx)

    def GetTxByIndex(self, idx):
        return Transaction(txPtr=self.GetDataByIndex(idx))

    def GetSize(self):
        return libRule.GetTxListSize(self.obj)

    def GetCapacity(self):
        return libRule.GetTxListCapacity(self.obj)


class Transaction2DList(object):
    def __init__(self, txPtr2DList):
        self.obj = txPtr2DList

    def GetDataByIndex(self, idx):
        return libRule.Tx2DListGetDataByIndex(self.obj, idx)

    def GetSize(self):
        return libRule.GetTx2DListSize(self.obj)


class StringList(object):
    def __init__(self, stringList=[]):
        self.obj = libRule.NewStringList()
        for string in stringList:
            self.Append(string)

    def Append(self, newData):
        libRule.StringListAppend(self.obj, ctypes.c_char_p(newData.encode('utf-8')))

    def GetDataByIndex(self, idx):
        return libRule.StringListGetDataByIndex(self.obj, idx).decode("utf-8")

    def GetSize(self):
        return libRule.GetStringListSize(self.obj)


class Rule(object):
    def __init__(self, amtThresh=0., timesThresh=0):
        self.obj = libRule.NewRule(ctypes.c_float(amtThresh), ctypes.c_ulonglong(timesThresh))

    def Run(self, txList, dateTimeStart=0):
        txListPtr = txList.obj
        return libRule.RunRule(self.obj, txListPtr, dateTimeStart)

    def GetAmtThresh(self):
        return libRule.RuleGetAmtThresh(self.obj)


class RuleA1(Rule):
    def __init__(self, amtThresh=0., timesThresh=0):
        self.obj = libRule.NewRuleA1(ctypes.c_float(amtThresh), ctypes.c_ulonglong(timesThresh))


def PyDateTime2C(dateTime):
    """
    return dateTime in unsigned-long-long format as YYYYmmddHHMMDDSS for input parameter of c++ based function
    param dateTime: python datetime object
    return:
    """
    return ctypes.c_ulonglong(int(dateTime.strftime("%Y%m%d%H%M%S")))


def IntDateTime2Py(dateTimeInInt):
    """
    param dateTimeInInt: dateTime in int format as YYYYmmddHHMMDDSS
    return: python datetime object
    """
    return datetime.datetime.strptime(str(dateTimeInInt), "%Y%m%d%H%M%S")


if __name__ == "__main__":
    rule1 = Rule(
        amtThresh=10.0,
        timesThresh=2
    )
    rule2 = Rule(
        amtThresh=100.0
    )
    rule3 = Rule(
        amtThresh=10.0,
        timesThresh=1
    )
    ruleA1 = RuleA1(
        amtThresh=10.0,
        timesThresh=1
    )
    ruleList = [rule1, rule2, rule3, ruleA1]
    dateTimeStartList = [
        (datetime.datetime.now() + datetime.timedelta(days=-7)),
        (datetime.datetime.now() + datetime.timedelta(days=-2)),
        (datetime.datetime.now() + datetime.timedelta(hours=-1)),
        (datetime.datetime.now() + datetime.timedelta(hours=-1)),
    ]

    tx1 = Transaction(
        PyDateTime2C(datetime.datetime.now()+datetime.timedelta(days=-2, hours=3, minutes=33, seconds=15)),
        10,
        "IBMB",
        "轉入"
    )
    tx2 = Transaction(
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(minutes=-33, seconds=15)),
        20,
        "Oversea",
        "轉帳"
    )
    tx3 = Transaction(
        PyDateTime2C(datetime.datetime.now()),
        20,
        "IBMB",
        "存入"
    )

    START = time.time()
    txList = TransactionList([*([tx1, tx2] * 1), tx3])
    print("Collect tranactions spent %f seconds" % (time.time() - START))
    print("Size: %d" % txList.GetSize())
    print("Capacity: %d" % txList.GetCapacity())

    print("Start Testing ..")
    START = time.time()
    for ruleId in range(len(ruleList)):
        rule = ruleList[ruleId]
        dateTimeStart = dateTimeStartList[ruleId]
        print("rule %s:" % (ruleId+1))
        print("Start date time of rule test: ", dateTimeStart)
        print("Threshold of amount: %f" % rule.GetAmtThresh())
        trigger = rule.Run(txList, PyDateTime2C(dateTimeStart))
        if trigger:
            print("=== Rule %s had been triggered. ===" % (ruleId+1))
    print("done")
    END = time.time()
    print("Spent %f seconds" % (END - START))

    print("rule pipeline ..")
    # stage 1
    print("=== Stage 1 ===")
    selectedTxPtrList = libRule.RulePipelineDateTimeFilter(
        txList.obj,
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(minutes=-5)),
        PyDateTime2C(datetime.datetime.now())
    )
    selectedTxList = TransactionList(txPtrList=selectedTxPtrList)
    numMatched = selectedTxList.GetSize()
    print("Found %d transactions matched conditions" % numMatched)
    for i in range(numMatched):
        selectedTx = selectedTxList.GetTxByIndex(i)
        print("DateTime: %s, Amount: %s, Channel: %s, Behavior: %s" % (
            selectedTx.GetDateTime(),
            selectedTx.GetAmount(),
            selectedTx.GetChannel(),
            selectedTx.GetBehavior()
        ))

    # stage 2
    print("=== Stage 2 ===")
    fieldStringList = StringList(stringList=["轉入", "存入"])
    condStringList = StringList(stringList=["轉入", "存入"])
    for i in range(condStringList.GetSize()):
        print("condition %d: %s" % (i, condStringList.GetDataByIndex(i)))
    selectedTxPtr2DList = libRule.RulePipelineConditionMatchFilter(
        txList.obj,
        ctypes.c_char_p("behavior".encode('utf-8')),
        condStringList.obj
    )
    selectedTx2DList = Transaction2DList(txPtr2DList=selectedTxPtr2DList)
    numCondtions = selectedTx2DList.GetSize()
    for i in range(numCondtions):
        selectedTxList = TransactionList(txPtrList=selectedTx2DList.GetDataByIndex(i))
        print("Found %d transactions matched condition:%s" % (numMatched, condStringList.GetDataByIndex(i)))
        for j in range(numMatched):
            selectedTx = selectedTxList.GetTxByIndex(j)
            print("DateTime: %s, Amount: %s, Channel: %s, Behavior: %s" % (
                selectedTx.GetDateTime(),
                selectedTx.GetAmount(),
                selectedTx.GetChannel(),
                selectedTx.GetBehavior()
            ))
