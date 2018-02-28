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
* Digit is a 32-bit integer;
* You can read about division algorithm [here](https://surface.syr.edu/cgi/viewcontent.cgi?article=1162&context=eecs_techreports), it's rather interesting and a little bit faster than binary search. But for more optimization it is better to use Karatsuba algorithm(just fork me!).
* You need google test library to run tests
## Running and building
```~>> git clone https://github.com/not-potato/big-integer
~>> cd build && cmake ..
~>> make
~>> ./big_integer_testing
```

* Average time to pass tests with sunitizer and without any optimizations on my computer is 1300ms;
* The most resource-intensive operations are multiplying and divison;
