# Call CPP function from Python
## CPP Programming
Due to the functionality of overloading in cpp, 
compiler will mangle the function name, so that 
you cannot call function by the name you defined, so 
you have to tell compiler this line should be compiled by 
c instead of c++ by ```extern "C"```

## Compile
```bash
gcc -c -fPIC cppSource.cpp -o cppSource.o
```
## Generate liberary
```bash
g++ cppSource.o -shared -o libfoo.so
```

## Python Programming
```python
import ctypes
libFoo = ctypes.CDLL("./libfoo.so")
```
Sometimes you may need to cast python types into c types, 
like ```ctypes.c_int(1)``` or others.