// Halide tutorial lesson 7: Multi-stage pipelines

// On linux, you can compile and run it like so:
// g++ lesson_07*.cpp -g -std=c++17 -I <path/to/Halide.h> -I <path/to/tools/halide_image_io.h> -L <path/to/libHalide.so> -lHalide `libpng-config --cflags --ldflags` -ljpeg -lpthread -ldl -o lesson_07
// LD_LIBRARY_PATH=<path/to/libHalide.so> ./lesson_07

// If you have the entire Halide source tree, you can also build it by
// running:
//    make tutorial_lesson_07_multi_stage_pipelines
// in a shell with the current directory at the top of the halide
// source tree.

#include "Halide.h"
#include <stdio.h>

using namespace Halide;

#include "halide_image_io.h"

using namespace Halide::Tools;

int main(int argc, char **argv) {
    
    Var x("x"), y("y"), c("c");

    {
        
        Buffer<uint8_t> input = load_image("Resources/parrot.jpg");
      
        Func input_16("input_16");
        input_16(x, y, c) = cast<uint16_t>(input(x, y, c));

        
        Func blur_x("blur_x");
        blur_x(x, y, c) = (input_16(x - 1, y, c) +
                           input_16(x, y, c) +
                           input_16(x + 1, y, c)) / 4;

        
        Func blur_y("blur_y");
        blur_y(x, y, c) = (blur_x(x, y - 1, c) +
                           blur_x(x, y, c) +
                           blur_x(x, y + 1, c)) / 4;

        
        Func output("output");
        output(x, y, c) = cast<uint8_t>(blur_y(x, y, c));

 
        Buffer<uint8_t> result(input.width() - 2, input.height() - 2, 3);
        result.set_min(1, 1);
        output.realize(result);

        save_image(result, "Resources/parrot1.jpg");

        
    }

    // The same pipeline, with a boundary condition on the input.
    {
        
        Buffer<uint8_t> input = load_image("Resources/tiger.jpg");

       
        Func clamped("clamped");
        Expr clamped_x = clamp(x, 0, input.width() - 1);
       
        Expr clamped_y = clamp(y, 0, input.height() - 1);
        
        clamped(x, y, c) = input(clamped_x, clamped_y, c);
       
        
        // clamped = BoundaryConditions::repeat_edge(input);
        
        Func input_16("input_16");
        input_16(x, y, c) = cast<uint16_t>(clamped(x, y, c));

        Func blur_x("blur_x");
        blur_x(x, y, c) = (input_16(x - 1, y, c) +
                           2 * input_16(x, y, c) +
                           input_16(x + 1, y, c)) / 4;

        // Blur it vertically:
        Func blur_y("blur_y");
        blur_y(x, y, c) = (blur_x(x, y - 1, c) +
                           2 * blur_x(x, y, c) +
                           blur_x(x, y + 1, c)) / 4;
        
        Func output("output");
        output(x, y, c) = cast<uint8_t>(blur_y(x, y, c));
        Buffer<uint8_t> result = output.realize({input.width(), input.height(), 3});

        save_image(result, "Resources/tiger2.jpg");
    }

    printf("Success!\n");
    return 0;
}