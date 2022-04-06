// Halide tutorial lesson 13: Tuples

// This lesson describes how to write Funcs that evaluate to multiple
// values.

// On linux, you can compile and run it like so:
// g++ lesson_13*.cpp -g -I <path/to/Halide.h> -L <path/to/libHalide.so> -lHalide -lpthread -ldl -o lesson_13 -std=c++17
// LD_LIBRARY_PATH=<path/to/libHalide.so> ./lesson_13


#include "Halide.h"
#include <algorithm>
#include <stdio.h>
using namespace Halide;
/*
int main(int argc, char **argv) {
	


	

   
    Func single_valued;
    Var x, y;
    single_valued(x, y) = x + y;

    Func color_image;
    Var c;
    color_image(x, y, c) = select(c == 0, 245,  // Red value
                                  c == 1, 42,   // Green value
                                  132);         // Blue value

    
    Func brighter;
    brighter(x, y, c) = color_image(x, y, c) + 10;

    Func func_array[3];
    func_array[0](x, y) = x + y;
    func_array[1](x, y) = sin(x);
    func_array[2](x, y) = cos(y);

    
    Func multi_valued;
    multi_valued(x, y) = Tuple(x + y, sin(x * y));

    {
        Realization r = multi_valued.realize({80, 60});
        assert(r.size() == 2);
        Buffer<int> im0 = r[0];
        Buffer<float> im1 = r[1];
        assert(im0(30, 40) == 30 + 40);
        assert(im1(30, 40) == sinf(30 * 40));
    }

    
    // equivalent C++ code to the above is:
    {
        int multi_valued_0[80 * 60];
        float multi_valued_1[80 * 60];
        for (int y = 0; y < 80; y++) {
            for (int x = 0; x < 60; x++) {
                multi_valued_0[x + 60 * y] = x + y;
                multi_valued_1[x + 60 * y] = sinf(x * y);
            }
        }
    }

    // When compiling ahead-of-time, a Tuple-valued Func evaluates
    // into multiple distinct output halide_buffer_t structs. These appear in
    // order at the end of the function signature:
    // int multi_valued(...input buffers and params...,
    //                  halide_buffer_t *output_1, halide_buffer_t *output_2);

    // You can construct a Tuple by passing multiple Exprs to the
    // Tuple constructor as we did above. Perhaps more elegantly, you
    // can also take advantage of initializer lists and just
    // enclose your Exprs in braces:
    Func multi_valued_2;
    multi_valued_2(x, y) = {x + y, sin(x * y)};

    
    Expr integer_part = multi_valued_2(x, y)[0];
    Expr floating_part = multi_valued_2(x, y)[1];
    Func consumer;
    consumer(x, y) = {integer_part + 10, floating_part + 10.0f};

    // Tuple reductions.
    {
        
        Func input_func;
        input_func(x) = sin(x);
        Buffer<float> input = input_func.realize({100});

        Func arg_max;

        // Pure definition.
        arg_max() = {0, input(0)};

        // Update definition.
        RDom r(1, 99);
        Expr old_index = arg_max()[0];
        Expr old_max = arg_max()[1];
        Expr new_index = select(old_max < input(r), r, old_index);
        Expr new_max = max(input(r), old_max);
        arg_max() = {new_index, new_max};

        // The equivalent C++ is:
        int arg_max_0 = 0;
        float arg_max_1 = input(0);
        for (int r = 1; r < 100; r++) {
            int old_index = arg_max_0;
            float old_max = arg_max_1;
            int new_index = old_max < input(r) ? r : old_index;
            float new_max = std::max(input(r), old_max);
            
            arg_max_0 = new_index;
            arg_max_1 = new_max;
        }

        // Let's verify that the Halide and C++ found the same maximum
        // value and index.
        {
            Realization r = arg_max.realize();
            Buffer<int> r0 = r[0];
            Buffer<float> r1 = r[1];
            assert(arg_max_0 == r0(0));
            assert(arg_max_1 == r1(0));
        }

       
    }

    // Tuples for user-defined types.
    {
        
        struct Complex {
            Expr real, imag;

            // Construct from a Tuple
            Complex(Tuple t)
                : real(t[0]), imag(t[1]) {
            }

            // Construct from a pair of Exprs
            Complex(Expr r, Expr i)
                : real(r), imag(i) {
            }

            // Construct from a call to a Func by treating it as a Tuple
            Complex(FuncRef t)
                : Complex(Tuple(t)) {
            }

            // Convert to a Tuple
            operator Tuple() const {
                return {real, imag};
            }

            // Complex addition
            Complex operator+(const Complex &other) const {
                return {real + other.real, imag + other.imag};
            }

            // Complex multiplication
            Complex operator*(const Complex &other) const {
                return {real * other.real - imag * other.imag,
                        real * other.imag + imag * other.real};
            }

            // Complex magnitude, squared for efficiency
            Expr magnitude_squared() const {
                return real * real + imag * imag;
            }

            // Other complex operators would go here. The above are
            // sufficient for this example.
        };

        
        Func mandelbrot;

        Complex initial(x / 15.0f - 1.5f, y / 6.0f - 1.0f);

        Var t;
        mandelbrot(x, y, t) = Complex(0.0f, 0.0f);

        RDom r(1, 20);
        Complex current = mandelbrot(x, y, r - 1);

        mandelbrot(x, y, r) = current * current + initial;
     

        Expr escape_condition = Complex(mandelbrot(x, y, r)).magnitude_squared() < 16.0f;
        Tuple first_escape = argmin(escape_condition);

        Func escape;
        escape(x, y) = first_escape[0];

        Buffer<int> result = escape.realize({61, 25});
        const char *code = " .:-~*={}&%#@";
        for (int y = 0; y < result.height(); y++) {
            for (int x = 0; x < result.width(); x++) {
                printf("%c", code[result(x, y)]);
            }
            printf("\n");
        }
    }

    printf("Success!\n");

    return 0;
}

*/
int main(int argc,char **argv){
	Func multi_func;
	Var x,y,c;
	multi_func(x,y) = Tuple(x*y, sin(x-y));
	
	{
	  Realization r = multi_func.realize({100,200});	
	  assert(r.size()== 2);
	  Buffer<int>out_1 = r[0];
      Buffer<float>out_2 = r[1]; 
	  assert(out_1(45,57)== 45*57);
	  assert(out_2(45,57)== sinf(45-57));
	  printf("\n %f",out_2(45,57));
      printf("\n %f",sin(45-57));
	}
	
/* 	Func multi_func2,consumer;
	multi_func2(x,y) = Tuple(x+y,y-x);
	Var func2_out1 = multi_func2(x,y)[0];
	Var func2_out2 = multi_func2(x,y)[1];
	consumer(x,y) = {func2_out1+11.1, func2_out2+3.14}; */
   {
	 Func in_Func("input_func"),max_Func("max_func");
	 
	 
	 in_Func(x) = cos(x);
	 Buffer<float>input = in_Func.realize({200});
	 
	 //Original definition
	 max_Func() = {0,input(0)};
	 RDom r(1,199); 
	 
	 Expr idx     = max_Func()[0];
	 Expr max_val = max_Func()[1];
	 Expr new_max = select(max_val<input(r),input(r),max_val);
	 Expr new_idx = select(max_val<input(r),r,idx);
     max_Func()   = {new_idx,new_max};	 
	 
	 
	 // The equivalent C++ is:
     int arg_max_0 = 0;
     float arg_max_1 = input(0);
     for (int r = 1; r < 100; r++) {
         int old_index = arg_max_0;
         float old_max = arg_max_1;
         int new_index = old_max < input(r) ? r : old_index;
         float new_max = std::max(input(r), old_max);
         // In a tuple update definition, all loads and computation
         // are done before any stores, so that all Tuple elements
         // are updated atomically with respect to recursive calls
         // to the same Func.
         arg_max_0 = new_index;
         arg_max_1 = new_max;
     }
	{
	 Realization r = max_Func.realize();
     Buffer<int> r0 = r[0];
     Buffer<float> r1 = r[1];
     assert(arg_max_0 == r0(0));
     assert(arg_max_1 == r1(0));  
    }
   }
   
   {
	   
   }
   printf("\nSuccess!\n");
	return 0;
}