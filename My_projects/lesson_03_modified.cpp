// Halide tutorial lesson 3: Inspecting the generated code

// This lesson demonstrates how to inspect what the Halide compiler is producing.

// On linux, you can compile and run it like so:
// g++ lesson_03*.cpp -g -I <path/to/Halide.h> -L <path/to/libHalide.so> -lHalide -lpthread -ldl -o lesson_03 -std=c++17
// LD_LIBRARY_PATH=<path/to/libHalide.so> ./lesson_03

#include "Halide.h"
#include <stdio.h>


using namespace Halide;

int main(int argc, char **argv) {

    Func gradient("gradient");
    Var x("x"), y("y");
    gradient(x, y) = x + y;

    Buffer<int> output = gradient.realize({32, 16});

    gradient.compile_to_lowered_stmt("gradient.html", {}, HTML);

    printf("Success!\n");
    return 0;
}