#include <stdio.h>
#include "my_first_gen.h"
#include "Halide.h"
#include "halide_image_io.h"
using namespace Halide::Tools;
int main(){
	struct halide_buffer_t  img_buffer;
	struct halide_buffer_t  out_buffer;
	Halide::Buffer<uint8_t> input = load_image("Resources/stairs_low_contrast.jpg");
	my_first_gen(3,&input,&out_buffer);
	
	return 0;
}