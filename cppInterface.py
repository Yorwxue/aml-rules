import os
import ctypes

libRule = ctypes.CDLL("./librule.so")

# call C function to check connection
print(libRule)
libRule.connect()

# return setting of c based function
libRule.GetChannelPtr.restype = ctypes.c_char_p
libRule.GetBehaviorPtr.restype = ctypes.c_char_p


class Transaction(object):
    def __init__(self, amt, channel, behavior):
        self.obj = libRule.NewTransaction(ctypes.c_float(amt), ctypes.c_char_p(channel), ctypes.c_char_p(behavior))

    def GetChannel(self):
        return libRule.GetChannelPtr(self.obj)

    def GetBehavior(self):
        return libRule.GetBehaviorPtr(self.obj)


class Rule(object):
    def __init__(self, amtThresh=0.):
        self.obj = libRule.NewRule(ctypes.c_float(amtThresh))
        self.txStructure = {
            float: ctypes.c_float,
            str: ctypes.c_char_p
        }

    def run(self, txList):
        print("NUmber of transactions: %d" % len(txList))
        # libRule.RunRule(self.obj, (libRule.Transaction) *txList, ctypes.c_int(len(txList)))


ruleTest = Rule(10.0)

tx1 = Transaction(10, "IBMB".encode('utf-8'), "轉入".encode('utf-8'))
tx2 = Transaction(20, "Oversea".encode('utf-8'), "轉帳".encode('utf-8'))
txList = [tx1, tx2]
print("tx:", tx1.GetChannel())
# ruleTest.run(txList)
