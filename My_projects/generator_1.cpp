#include "Halide.h"
#include <stdio.h>

using namespace Halide;

class myGenerator: public Halide::Generator<myGenerator>
{
	public:
	Input<uint8_t>offset{"offset"};
	Input<Buffer<uint8_t>>input{"input",2};
	Output<Buffer<uint8_t>>brighter{"brighter",2};
	
	Var x,y;
	
	void generate(){
		//Algorithm
		brighter(x,y)= input(x,y)+ offset;
		//Schedule
		brighter.vectorize(x,8).parallel(y);
	}
	
};
HALIDE_REGISTER_GENERATOR(myGenerator,my_first_gen)

