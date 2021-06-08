import os
import ctypes
import datetime
import time


libRule = ctypes.CDLL("./librule.so")

# call C function to check connection
print(libRule)
libRule.connect()

# return setting of c based function
# libRule.GetTxListSize.restype = ctypes.c_int  # unnecessary
# libRule.GetTxListCapacity.restype = ctypes.c_int  # unnecessary
libRule.TxGetAmount.restype = ctypes.c_float
libRule.TxGetDateTime.restype = ctypes.c_ulonglong
libRule.TxGetChannelPtr.restype = ctypes.c_char_p
libRule.TxGetBehaviorPtr.restype = ctypes.c_char_p
libRule.RuleGetAmtThresh.restype = ctypes.c_float
libRule.RunRule.restype = ctypes.c_bool


class Transaction(object):
    def __init__(self, dateTime=None, amt=None, channel=None, behavior=None, txPtr=None):
        if txPtr:
            self.obj = txPtr
        elif dateTime and amt and channel and behavior:
            self.obj = libRule.NewTransaction(dateTime, ctypes.c_float(amt), ctypes.c_char_p(channel), ctypes.c_char_p(behavior))
        else:
            raise ValueError("Invalid parameters for constructor")

    def GetDateTime(self):
        return libRule.TxGetDateTime(self.obj)

    def GetAmount(self):
        return libRule.TxGetAmount(self.obj)

    def GetChannel(self):
        return libRule.TxGetChannelPtr(self.obj)

    def GetBehavior(self):
        return libRule.TxGetBehaviorPtr(self.obj)


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

    def GetPtrByIndex(self, idx):
        return libRule.TxListGetPtrByIndex(self.obj, idx)

    def GetTxByIndex(self, idx):
        return Transaction(txPtr=self.GetPtrByIndex(idx))

    def GetSize(self):
        return libRule.GetTxListSize(self.obj)

    def GetCapacity(self):
        return libRule.GetTxListCapacity(self.obj)


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
        "IBMB".encode('utf-8'),
        "轉入".encode('utf-8')
    )
    tx2 = Transaction(
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(minutes=-33, seconds=15)),
        20,
        "Oversea".encode('utf-8'),
        "轉帳".encode('utf-8')
    )
    tx3 = Transaction(
        PyDateTime2C(datetime.datetime.now()),
        20,
        "IBMB".encode('utf-8'),
        "轉帳".encode('utf-8')
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
    selectedTxPtrList = libRule.RulePipelineDateTimeFilter(
        txList.obj,
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(minutes=-5)),
        PyDateTime2C(datetime.datetime.now())
    )
    selectedTxList = TransactionList(txPtrList=selectedTxPtrList)
    if selectedTxList.GetSize():
        selectedTx = selectedTxList.GetTxByIndex(0)
        print("DateTime: %s, Amount: %s, Channel: %s" % (
            selectedTx.GetAmount(),
            selectedTx.GetDateTime(),
            selectedTx.GetChannel()
        ))
    else:
        print("No transactions match the condition")
