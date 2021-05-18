import os
import ctypes

libCalc = ctypes.CDLL("./librule.so")
print(libCalc)

# call C function to check connection
libCalc.connect()

# calling randNum() C function
# it returns random number
varRand = libCalc.randNum()
print("Random Number:", varRand, type(varRand))

# calling addNum() C function
# it returns addition of two numbers
varAdd = libCalc.addNum(20, 30)
print("Addition : ", varAdd)
