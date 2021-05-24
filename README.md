# Call CPP function from Python
## Tips for CPP Programming
### Mangle
Due to the functionality of overloading in cpp, 
compiler will mangle the function name, so that 
you cannot call function by the name you defined, so 
you have to tell compiler this line should be compiled by 
c instead of c++ by ```extern "C"```
### ctypes API
ctypes only support c api(not c++), so class with 
initializer and destructor or other member functions aren't
supported. If you still want to call c++-based class object, 
you need to implement functions as proxy, for example:
```C++
class Rule
{
    private:
        float amtThresh;
    public:
        Rule(){amtThresh = 0;}
        Rule(const float amtThresh_in){amtThresh = amtThresh_in;}
        ~Rule(){}
        void run()
        {
            std::cout << "Threshold of amount:" << amtThresh << std::endl;
        }
};
Rule* NewRule(float amtThresh_in)
{
    return new Rule(amtThresh_in);
}

void RunRule(Rule* rulePtr)
{
    rulePtr->run();
}
```
### Notations for Types
#### String
 - python -> c++: 
   Before sent string from python to c++, you must change format of python-string, for example:
    ```python
    inString = "hello".encode('utf-8')
    ctypes.c_char_p(inString)
    ```
   In c++ side, you need to accept the string by point of char,
   then youn consider it as std::string
   ```c++
   char * const inCharPtr;
   std::string inputString = inCharPtr;
   ```
 - c++ -> python:
   Just like sent string from python to c++, changing format is necessary:
   ```c++
   const char *GetCharPtr(std::string outString){return outString.c_str();}
   ```
   In python side, you only need to define format at the first time, then
   you can get return string directly
   ```python
   libFoo.GetCharPtr.restype = ctypes.c_char_p
   ```
## Compile
```bash
gcc -c -fPIC cppSource.cpp -o cppSource.o
```
## Generate liberary
```bash
g++ cppSource.o -shared -o libfoo.so
```

## Tips for Python Programming
```python
import ctypes
libFoo = ctypes.CDLL("./libfoo.so")
```
Sometimes you may need to cast python types into c types, 
like ```ctypes.c_int(1)``` or others.