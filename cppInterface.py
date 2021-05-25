import os
import ctypes

libRule = ctypes.CDLL("./librule.so")

# call C function to check connection
print(libRule)
libRule.connect()

# return setting of c based function
libRule.TxGetAmount.restype = ctypes.c_float
libRule.TxGetChannelPtr.restype = ctypes.c_char_p
libRule.TxGetBehaviorPtr.restype = ctypes.c_char_p


class Transaction(object):
    def __init__(self, amt, channel, behavior):
        self.obj = libRule.NewTransaction(ctypes.c_float(amt), ctypes.c_char_p(channel), ctypes.c_char_p(behavior))

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
    def __init__(self, amtThresh=0.):
        self.obj = libRule.NewRule(ctypes.c_float(amtThresh))
        self.txStructure = {
            float: ctypes.c_float,
            str: ctypes.c_char_p
        }

    def Run(self, txList):
        txListPtr = txList.obj
        libRule.RunRule(self.obj, txListPtr)


ruleTest = Rule(10.0)

tx1 = Transaction(10, "IBMB".encode('utf-8'), "轉入".encode('utf-8'))
tx2 = Transaction(20, "Oversea".encode('utf-8'), "轉帳".encode('utf-8'))
print("tx1.GetAmt:", tx1.GetAmount())
print("tx1.GetChannel:", tx1.GetChannel())

txList = TransactionList([tx1, tx2])

# check address are the same
# print(tx1.obj)
# print(txList.GetByIndex(0))

ruleTest.Run(txList)
