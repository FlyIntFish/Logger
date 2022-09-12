# Logger
Simple logger for projects

## C++ standard
Logger uses *format* library (successor of *fmt*) and requires C++20.

## How it works
Each logger uses its own path and creates (if not present) new file named the same as logger with extension .txt
When buffer of the logger is almost full, then logger writes its context to the file.
Logger can also print logs to some output stream (std::cout, std::cerr, etc.)

## How to use
### 1. Creating logger
Let's take a look at the constructor:
```cpp
Log::Logger myLogger ("test_logger", true, 10240);
```

*"test_logger"* - name of logger
*true* - flag representing appending to file
*10240* - size of buffer

In this case there will be created logger with *test_logger* name (and so will be named the output file). 


### 2. Creating logs
There are three ways to create log.

#### 2.1 operator<<()
The first one is using << operator, just like with std::cout
```cpp
myLogger << Log::DEBUG << "debug message: " << 2 << " + " << 2.5 << " = " << 4.5f;
```
Of course we can also pass formatted string:
```cpp
myLogger << std::format("{} + {} = {}", 2.f, 1, 3.0);
```

#### 2.2 message()

```cpp
myLogger.message("{}Hello {}, my name is {}", Log::INFO, "world", "FlyIntFish");
```
First argument of message must be a formating string. It works just like [std::format](https://en.cppreference.com/w/cpp/utility/format/format).

#### 2.3 info() / trace() / debug() / warning() / error() / critical()
These functions are wrappers for *message()*. They insert proper tag (info, debug, etc.) at the beginning of the log.
```cpp
myLogger.error("size of the vector is lower than expected: {}", someVector.size());
```

### 3. Tags
There are 6 tags available to log:
  * info
  * trace
  * debug
  * warning
  * error
  * critical

### 4. Wait, what's unsupported type?
<UNSUPPORTED TYPE> is string placed to the log if object that we try to print using operator<< can't be easily converted to string.
```cpp
std::vector<int> vec{1, 2, 3};
myLogger << Log::TRACE << "in vector vec: " << vec;
```
