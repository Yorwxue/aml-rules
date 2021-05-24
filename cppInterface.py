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

    def ShowChannel(self):
        libRule.ShowChannel(self.obj)

    def GetChannel(self):
        return libRule.GetChannelPtr(self.obj)

    def GetBehavior(self):
        return libRule.GetBehaviorPtr(self.obj)


class Rule(object):
    def __init__(self, amtThresh=0.):
        self.obj = libRule.NewRule(ctypes.c_float(amtThresh))

    def run(self):
        libRule.RunRule(self.obj)


ruleTest = Rule(10.0)
ruleTest.run()

tx = Transaction(10, "IBMB".encode('utf-8'), "轉入".encode('utf-8'))
# tx.ShowChannel()
print("tx:", tx.GetChannel())
