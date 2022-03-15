// Halide tutorial lesson 4: Debugging with tracing, print, and print_when

// This lesson demonstrates how to follow what Halide is doing at runtime.

// On linux, you can compile and run it like so:
// g++ lesson_04*.cpp -g -I <path/to/Halide.h> -L <path/to/libHalide.so> -lHalide -lpthread -ldl -o lesson_04 -std=c++17
// LD_LIBRARY_PATH=<path/to/libHalide.so> ./lesson_04


#include "Halide.h"
#include <stdio.h>
using namespace Halide;

int main(int argc, char **argv) {

    Var x("x"), y("y");

    
    {
        
        Func gradient("gradient");
        gradient(x, y) = x + y;

        gradient.trace_stores();

        printf("Evaluating gradient\n");
        Buffer<int> output = gradient.realize({8, 8});
        
        Func parallel_gradient("parallel_gradient");
        parallel_gradient(x, y) = x + y;
        parallel_gradient.trace_stores();
        parallel_gradient.parallel(y);
        
		printf("\nEvaluating parallel_gradient\n");
        parallel_gradient.realize({8, 8});
        // Click to show output ...
    }

    // Printing individual Exprs.
    {
        // trace_stores() can only print the value of a
        // Func. Sometimes you want to inspect the value of
        // sub-expressions rather than the entire Func. The built-in
        // function 'print' can be wrapped around any Expr to print
        // the value of that Expr every time it is evaluated.

        // For example, say we have some Func that is the sum of two terms:
        Func f;
        f(x, y) = sin(x) + cos(y);

        // If we want to inspect just one of the terms, we can wrap
        // 'print' around it like so:
        Func g;
        g(x, y) = sin(x) + print(cos(y));

        printf("\nEvaluating sin(x) + cos(y), and just printing cos(y)\n");
        g.realize({4, 4});
        // Click to show output ...
    }

    // Printing additional context.
    {
        // print can take multiple arguments. It prints all of them
        // and evaluates to the first one. The arguments can be Exprs
        // or constant strings. This can be used to print additional
        // context alongside the value:
        Func f;
        f(x, y) = sin(x) + print(cos(y), "<- this is cos(", y, ") when x =", x);

        printf("\nEvaluating sin(x) + cos(y), and printing cos(y) with more context\n");
        f.realize({4, 4});
        // Click to show output ...

        // It can be useful to split expressions like the one above
        // across multiple lines to make it easier to turn on and off
        // printing certain values while debugging.
        Expr e = cos(y);
        // Uncomment the following line to print the value of cos(y)
        // e = print(e, "<- this is cos(", y, ") when x =", x);
        Func g;
        g(x, y) = sin(x) + e;
        g.realize({4, 4});
    }

    // Conditional printing
    {
        // Both print and trace_stores can produce a lot of output. If
        // you're looking for a rare event, or just want to see what
        // happens at a single pixel, this amount of output can be
        // difficult to dig through. Instead, the function print_when
        // can be used to conditionally print an Expr. The first
        // argument to print_when is a boolean Expr. If the Expr
        // evaluates to true, it returns the second argument and
        // prints all of the arguments. If the Expr evaluates to false
        // it just returns the second argument and does not print.

        Func f;
        Expr e = cos(y);
        e = print_when(x == 37 && y == 42, e, "<- this is cos(y) at x, y == (37, 42)");
        f(x, y) = sin(x) + e;
        printf("\nEvaluating sin(x) + cos(y), and printing cos(y) at a single pixel\n");
        f.realize({640, 480});
        // Click to show output ...

        // print_when can also be used to check for values you're not expecting:
        Func g;
        e = cos(y);
        e = print_when(e < 0, e, "cos(y) < 0 at y ==", y);
        g(x, y) = sin(x) + e;
        printf("\nEvaluating sin(x) + cos(y), and printing whenever cos(y) < 0\n");
        g.realize({4, 4});
        // Click to show output ...
    }

    // Printing expressions at compile-time.
    {
        // The code above builds up a Halide Expr across several lines
        // of code. If you're programmatically constructing a complex
        // expression, and you want to check the Expr you've created
        // is what you think it is, you can also print out the
        // expression itself using C++ streams:
        Var fizz("fizz"), buzz("buzz");
        Expr e = 1;
        for (int i = 2; i < 100; i++) {
            if (i % 3 == 0 && i % 5 == 0) {
                e += fizz * buzz;
            } else if (i % 3 == 0) {
                e += fizz;
            } else if (i % 5 == 0) {
                e += buzz;
            } else {
                e += i;
            }
        }
        std::cout << "Printing a complex Expr: " << e << "\n";
		
        // Click to show output ...
    }
    //gradient.compile_to_lowered_stmt("gradient.html", {}, HTML);
    printf("Success!\n");
    return 0;
}