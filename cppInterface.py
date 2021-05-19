import os
import ctypes

libRule = ctypes.CDLL("./librule.so")
print(libRule)

# call C function to check connection
libRule.connect()

# calling randNum() C function
# it returns random number
varRand = libRule.randNum()
print("Random Number:", varRand, type(varRand))

# calling addNum() C function
# it returns addition of two numbers
varAdd = libRule.addNum(20, 30)
print("Addition : ", varAdd)

print("================================")


class Rule(object):
    def __init__(self, amtThresh=0.):
        self.obj = libRule.NewRule(ctypes.c_float(amtThresh))

    def run(self):
        libRule.RunRule(self.obj)


ruleTest = Rule(10.0)
ruleTest.run()
