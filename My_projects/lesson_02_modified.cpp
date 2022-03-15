// Halide tutorial lesson 2: Processing images

// On linux, you can compile and run it like so:
// g++ lesson_02*.cpp -g -I <path/to/Halide.h> -I <path/to/tools/halide_image_io.h> -L <path/to/libHalide.so> -lHalide `libpng-config --cflags --ldflags` -ljpeg -lpthread -ldl -o lesson_02 -std=c++17
// LD_LIBRARY_PATH=<path/to/libHalide.so> ./lesson_02

#include "Halide.h"

// Include some support code for loading pngs.
#include "halide_image_io.h"
using namespace Halide::Tools;

int main(int argc, char **argv) {
     
    
    Halide::Buffer<uint8_t> input = load_image("Resources/stairs_low_contrast.jpg");
    Halide::Func brighter;  
    Halide::Var x, y, c;

    Halide::Expr value = input(x, y, c);    
    value = Halide::cast<float>(value);       
    value = value * 4.0f;
    value = Halide::min(value, 255.0f);    
    value = Halide::cast<uint8_t>(value);
    // Define the function.
    brighter(x, y, c) = value;
    // The equivalent one-liner to all of the above is:
    // brighter(x, y, c) = Halide::cast<uint8_t>(min(input(x, y, c) * 1.5f, 255));
    
    Halide::Buffer<uint8_t> output =
        brighter.realize({input.width(), input.height(), input.channels()});
    
    save_image(output, "Resources/lesson02_out.jpg");
    printf("Success!\n");
    return 0;
}
