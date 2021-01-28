# cpp_recipes

Miscellaneous C++ scripts meant to help me learn the language with practical applications.

* `Fraction.h`/`Fraction.cpp` files -- the former defines a Fraction template class with support for real numbers and ± infinity, and the latter is a corresponding test script. One issue revealed by the test script is that the class currently yields +inf when dividing a negative fraction by 0. Other tests pass.
* `RollingWindow.h`/`RollingWindow.cpp` -- efficient way of taking a rolling window over a vector; header file with template class definition and accompanying script with example usage.