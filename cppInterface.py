import os
import ctypes
import datetime
import time


libRule = ctypes.CDLL("./librule.so")

# call C function to check connection
print(libRule)
libRule.connect()

# return setting of c based function
libRule.TxGetAmount.restype = ctypes.c_float
libRule.TxGetDateTime.restype = ctypes.c_ulonglong
libRule.TxGetChannelPtr.restype = ctypes.c_char_p
libRule.TxGetBehaviorPtr.restype = ctypes.c_char_p
libRule.RuleGetAmtThresh.restype = ctypes.c_float
libRule.RunRule.restype = ctypes.c_bool


class Transaction(object):
    def __init__(self, dateTime, amt, channel, behavior):
        self.obj = libRule.NewTransaction(ctypes.c_ulonglong(dateTime), ctypes.c_float(amt), ctypes.c_char_p(channel), ctypes.c_char_p(behavior))

    def GetDateTime(self):
        return libRule.TxGetDateTime(self.obj)

    def GetAmount(self):
        return libRule.TxGetAmount(self.obj)

    def GetChannel(self):
        return libRule.TxGetChannelPtr(self.obj)

    def GetBehavior(self):
        return libRule.TxGetBehaviorPtr(self.obj)


class TransactionList(object):
    def __init__(self, txList=None):
        self.obj = libRule.NewTransactionList()
        for tx in txList:
            txPtr = tx.obj
            libRule.TxListAppend(self.obj, txPtr)

    def GetByIndex(self, idx):
        return libRule.TxListGetByIndex(self.obj, idx)


class Rule(object):
    def __init__(self, amtThresh=0., timesThresh=1):
        self.obj = libRule.NewRule(ctypes.c_float(amtThresh), ctypes.c_int(timesThresh))

    def Run(self, txList, dateTimeStart=0):
        txListPtr = txList.obj
        return libRule.RunRule(self.obj, txListPtr, ctypes.c_ulonglong(dateTimeStart))

    def GetAmtThresh(self):
        return libRule.RuleGetAmtThresh(self.obj)


def PyDateTime2C(dateTime):
    """
    param dateTime: python datetime object
    return: dateTime in int format as YYYYmmddHHMMDDSS for input parameter of c++ basefunction
    """
    return int(dateTime.strftime("%Y%m%d%H%M%S"))


def CDateTime2Py(dateTimeInInt):
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
    ruleList = [rule1, rule2, rule3]
    dateTimeStartList = [
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(days=-7)),
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(days=-2)),
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(hours=-1)),
    ]

    tx1 = Transaction(
        PyDateTime2C(datetime.datetime.now()),
        10,
        "IBMB".encode('utf-8'),
        "??????".encode('utf-8')
    )
    tx2 = Transaction(
        PyDateTime2C(datetime.datetime.now()+datetime.timedelta(days=-2, hours=3, minutes=33, seconds=15)),
        20,
        "Oversea".encode('utf-8'),
        "??????".encode('utf-8')
    )
    tx3 = Transaction(
        PyDateTime2C(datetime.datetime.now() + datetime.timedelta(minutes=-33, seconds=15)),
        20,
        "IBMB".encode('utf-8'),
        "??????".encode('utf-8')
    )
    # print("tx1.GetDateTime:", CDateTime2Py(tx1.GetDateTime()))
    # print("tx1.GetAmt:", tx1.GetAmount())
    # print("tx1.GetChannel:", tx1.GetChannel())

    txList = TransactionList([tx1, tx2, tx3]*1000000)

    # check address are the same
    # print(tx1.obj)
    # print(txList.GetByIndex(0))

    # rule test
    # ruleTest.Run(txList)
    print("Start Testing ..")
    START = time.time()
    for ruleId in range(len(ruleList)):
        rule = ruleList[ruleId]
        dateTimeStart = dateTimeStartList[ruleId]
        print("rule %s:" % (ruleId+1))
        print("Start date time of rule test: ", CDateTime2Py(dateTimeStart))
        print("Threshold of amount: %f" % rule.GetAmtThresh())
        trigger = rule.Run(txList, dateTimeStart)
        if trigger:
            print("=== Rule %s had been triggered. ===" % (ruleId+1))
    print("done")
    END = time.time()
    print("Spent %f seconds" % (END - START))
