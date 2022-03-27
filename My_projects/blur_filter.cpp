#include "Halide.h"
#include "halide_image_io.h"
#include "HalideBuffer.h"
#include <stdio.h>
#include "blur_filter.h"

#define filt_len =5;
#define SSE_width = 128;
#define AVX_width = 128;
#define neon_width = 128;

using namespace Halide;
using namespace Halide::Tools;

int main(int argc,char **argv){
	
	
	
	Var x("x"),y("y"),c("c");
	
/* 	{	
		//Simple implementation
	   
		Buffer<uint8_t> in_img = load_image("Resources/chessBoard.png");
		Func clamped("clamped");
		Expr clamped_x = clamp(x, 0, in_img.width() - 1);
		Expr clamped_y = clamp(y, 0, in_img.height() - 1);
	    //auto start = high_resolution_clock::now();
		
	    Func input16("img_16t"), blurX("blurX"), blurY("blurY");
		blurY.trace_stores();
		clamped(x,y,c) = in_img(clamped_x, clamped_y, c);
		input16(x,y,c) = cast<uint16_t>(clamped(x,y,c));
		
		blurX(x,y,c) = (c0*input16(x-2,y,c)+
		                c1*input16(x-1,y,c)+
					    c2*input16(x,  y,c)+
					    c3*input16(x+1,y,c)+
					    c4*input16(x+2,y,c))/5;
		blurY(x,y,c) = (d0*blurX(x,y-2,c)+
		                d1*blurX(x,y-1,c)+
					    d2*blurX(x,y  ,c)+
					    d3*blurX(x,y+1,c)+
					    d4*blurX(x,y+2,c))/5;
		
		Func output("output");
		output(x,y,c) = cast<uint8_t>(blurY(x,y,c));
		Buffer<uint8_t> result = output.realize({in_img.width(), in_img.height(), 3});
		//auto stop = high_resolution_clock::now()
		
		//print("Execution time = %f",(stop-start));
		save_image(result, "Resources/chessBoard1.png");
		printf("simple implementation\n");
		blurY.print_loop_nest();
        printf("\n");
	}
	
	{	
		//compute_root() (remove inlining)
	   
		Buffer<uint8_t> in_img = load_image("Resources/chessBoard.png");
		Func clamped("clamped");
		Expr clamped_x = clamp(x, 0, in_img.width() - 1);
		Expr clamped_y = clamp(y, 0, in_img.height() - 1);
	    //auto start = high_resolution_clock::now();
		
	    Func input16("img_16t"), blurX("blurX"), blurY("blurY");
		blurY.trace_stores();
		clamped(x,y,c) = in_img(clamped_x, clamped_y, c);
		input16(x,y,c) = cast<uint16_t>(clamped(x,y,c));
		
		blurX(x,y,c) = (1*input16(x-2,y,c)+
		                1*input16(x-1,y,c)+
					    1*input16(x,  y,c)+
					    1*input16(x+1,y,c)+
					    1*input16(x+2,y,c))/5;
		blurY(x,y,c) = (1*blurX(x,y-2,c)+
		                1*blurX(x,y-1,c)+
					    1*blurX(x,y  ,c)+
					    1*blurX(x,y+1,c)+
					    1*blurX(x,y+2,c))/5;
		
		blurX.compute_root();
		
		Func output("output");
		output(x,y,c) = cast<uint8_t>(blurY(x,y,c));
		Buffer<uint8_t> result = output.realize({in_img.width(), in_img.height(), 3});
		//auto stop = high_resolution_clock::now()
		
		//print("Execution time = %f",(stop-start));
		save_image(result, "Resources/chessBoard2.png");
		printf("Compute_root() for blurX stage\n");
		blurY.print_loop_nest();
        printf("\n");
	} */	
	{	
		//blurX.compute_at(blurY,y);compute blurX when required by blurY
	   
		Buffer<uint8_t> in_img = load_image("Resources/chessBoard.png");
		Func clamped("clamped");
		Expr clamped_x = clamp(x, 0, in_img.width() - 1);
		Expr clamped_y = clamp(y, 0, in_img.height() - 1);
	    //auto start = high_resolution_clock::now();
		
	    Func input16("img_16t"), blurX("blurX"), blurY("blurY");
		//blurY.trace_stores();
		clamped(x,y,c) = in_img(clamped_x, clamped_y, c);
		input16(x,y,c) = cast<float>(clamped(x,y,c));
		
		blurX(x,y,c) = (1*input16(x-2,y,c)+
		                1*input16(x-1,y,c)+
					    1*input16(x,  y,c)+
					    1*input16(x+1,y,c)+
					    1*input16(x+2,y,c))/5;
					
		blurY(x,y,c) = (1*blurX(x,y-2,c)+
		                1*blurX(x,y-1,c)+
					    1*blurX(x,y  ,c)+
					    1*blurX(x,y+1,c)+
					    1*blurX(x,y+2,c))/5;
		
		Var xo, yo, xi, yi;
		
		//blurY.tile(x,y,xo,yo,xi,yi,120,32).parallel(yo);
		//blurX.store_root().compute_at(blurY,yi);
		blurX.compute_at(blurY,y);
		
		Func output("output");
		output(x,y,c) = cast<uint8_t>(blurY(x,y,c));
		Buffer<uint8_t> result = output.realize({in_img.width(), in_img.height(), 3});
		
		save_image(result, "Resources/chessBoard2.png");
		printf("Compute_at() for blurX stage\n");
		blurY.print_loop_nest();
        printf("\n");
	}	
	
	
	printf("Success\n");
	return 0;
}

