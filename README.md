# big-integer
C++ implementation of long arifmetic class Big Integer with smart division algorithm.

big_integer class located at big_integer.h and big_integer.cpp files and implements long arithmetic.

## Description
* Default constructor initialies number with 0;
* After the copy constructor, the original object and the copy can be modified independently;
* It's constructor from int;
* It's explicit constructor from std::string
* You can use assignment and comporation operators
* All arithmetic operations supported: addition, subtraction, unary plus and minus, prefix/postfix increment/decrement;
* Multiplication works in O(nm), where n and m is a bit length of the multipliers;
* Devision and module of division works in O(nm), where n and m is a bit length of divisor and divident, respectively;
* Binary operations: and, or, xor, not, bit shifts
Also it's a global function to_string and ostream<< operator for output and other stuff.

## Some words about implementation
* In this implementation I wrote **Small Object Optimization** and **Copy On Write** optimization. First one means that if the stored number is rather small class will work in 'small' mode and will not allocate any heap memory. The second one optimization means that the actual copying of the object is delayed and happens only if we modify the copy.
* Digit is a 32-bit integer;
* You can read about division algorithm [here](https://surface.syr.edu/cgi/viewcontent.cgi?article=1162&context=eecs_techreports), it's rather interesting and a little bit faster than binary search. But for more optimization it is better to use Karatsuba algorithm(just fork me!).
* You need google test library to run tests
## Running and building
```~>> git clone https://github.com/not-potato/big-integer
~>> make dir build
~>> cd build && cmake ..
~>> make
~>> ./big_integer_testing
```

* Average time to pass tests with sanitizer and without any optimizations on my computer is 1000ms;
* But with **-O2** compile flag and without any sanitizer average time to pass tests is 250ms on my computer;
* The most resource-intensive operations are multiplying and divison;
* Because of **Small Object Optimization** operations with small numbers should now be faster, but it's not so visibly on my tests;
