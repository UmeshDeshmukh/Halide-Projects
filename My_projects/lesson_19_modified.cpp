// Halide tutorial lesson 19: Wrapper Funcs

// This lesson demonstrates how to use Func::in and ImageParam::in to
// schedule a Func differently in different places, and to stage loads
// from a Func or an ImageParam.

// On linux, you can compile and run it like so:
// g++ lesson_19*.cpp -g -I <path/to/Halide.h> -L <path/to/libHalide.so> -lHalide -lpthread -ldl -o lesson_19 -std=c++17
// LD_LIBRARY_PATH=<path/to/libHalide.so> ./lesson_19

// On os x:
// g++ lesson_19*.cpp -g -I <path/to/Halide.h> -L <path/to/libHalide.so> -lHalide -o lesson_19 -std=c++17
// DYLD_LIBRARY_PATH=<path/to/libHalide.dylib> ./lesson_19

// If you have the entire Halide source tree, you can also build it by
// running:
//    make tutorial_lesson_19_wrapper_funcs
// in a shell at the top of the halide source tree.

// The only Halide header file you need is Halide.h. It includes all of Halide.
#include "Halide.h"

// We'll also include stdio for printf.
#include <stdio.h>

using namespace Halide;

int main(int argc, char **argv) {
    
    Var x("x"), y("y"), xo("xo"), yo("yo"), xi("xi"), yi("yi");

    // Func::in and ImageParam::in directives
    {
        // Consider a simple two-stage pipeline:
        Func f("f_local"), g("g_local");
        f(x, y) = x + y;
        g(x, y) = 2 * f(x, y) + 3;

        f.compute_root();

        Func f_in_g = f.in(g);
        f_in_g.compute_root();

        g.realize({5, 5});

        {
            Func f_in_g("f_in_g"), f("f"), g("g");
            f(x, y) = x + y;
            f_in_g(x, y) = f(x, y);
            g(x, y) = 2 * f_in_g(x, y) + 3;

            f.compute_root();
            f_in_g.compute_root();
            g.compute_root();
        }

    }

    {
        Func f("f_global"), g("g_global"), h("h_global");
        f(x, y) = x + y;
        g(x, y) = 2 * f(x, y);
        h(x, y) = 3 + g(x, y) - f(x, y);
        f.compute_root();
        g.compute_root();
        h.compute_root();

        f.in().compute_root();

        h.realize({5, 5});


    }

    {

        Func f("f_unique"), g("g_unique"), h("h_unique");
        f(x, y) = x + y;
        g(x, y) = 2 * f(x, y);
        h(x, y) = 3 + g(x, y) - f(x, y);

        f.compute_root();
        g.compute_root();
        h.compute_root();

        f.in(g).compute_at(g, y);
        f.in(h).compute_at(h, y);


        h.realize({5, 5});
        

    }

    {
        Func f("f_sched"), g("g_sched"), h("h_sched");
        f(x, y) = x + y;
        g(x, y) = 2 * f(x, y);
        // h will use a far-away region of f
        h(x, y) = 3 + g(x, y) - f(x + 93, y - 87);

        // This time we'll inline f.
        // f.compute_root();
        g.compute_root();
        h.compute_root();

        f.in(g).compute_at(g, y);
        f.in(h).compute_at(h, y);

        f.in(g).vectorize(x, 4);
        f.in(h).split(x, xo, xi, 2).reorder(xo, xi);


        h.realize({8, 8});
        
    }

    {
        

        Func f("f_transpose"), g("g_transpose");
        f(x, y) = sin(((x + y) * sqrt(y)) / 10);
        f.compute_root();

        g(x, y) = f(y, x);


        Func f_tile = f.in(g);

        g.tile(x, y, xo, yo, xi, yi, 4, 4)
            .vectorize(xi)
            .unroll(yi);

        
        f_tile.compute_at(g, xo)
            .reorder_storage(y, x)
            .vectorize(x)
            .unroll(y);


        g.realize({16, 16});
        // See below for a visualization

         

    }

    {
        // ImageParam::in behaves the same way as Func::in, and you
        // can use it to stage loads in similar ways. Instead of
        // transposing again, we'll use ImageParam::in to stage tiles
        // of an input image into GPU shared memory, effectively using
        // shared/local memory as an explicitly-managed cache.

        ImageParam img(Int(32), 2);

        // We will compute a small blur of the input.
        Func blur("blur");
        blur(x, y) = (img(x - 1, y - 1) + img(x, y - 1) + img(x + 1, y - 1) +
                      img(x - 1, y) + img(x, y) + img(x + 1, y) +
                      img(x - 1, y + 1) + img(x, y + 1) + img(x + 1, y + 1));

        blur.compute_root().gpu_tile(x, y, xo, yo, xi, yi, 8, 8);

        // The wrapper Func created by ImageParam::in has pure vars
        // named _0, _1, etc. Schedule it per tile of "blur", and map
        // _0 and _1 to gpu threads.
        img.in(blur).compute_at(blur, xo).gpu_threads(_0, _1);

        // Without Func::in, computing an 8x8 tile of blur would do
        // 8*8*9 loads to global memory. With Func::in, the wrapper
        // does 10*10 loads to global memory up front, and then blur
        // does 8*8*9 loads to shared/local memory.

        // Select an appropriate GPU API, as we did in lesson 12
        Target target = get_host_target();
        if (target.os == Target::OSX) {
            target.set_feature(Target::Metal);
        } else {
            target.set_feature(Target::OpenCL);
        }

        // This check isn't strictly necessary, but it allows a more graceful
        // exit if running on a system that doesn't have the expected drivers
        // and/or hardware present.
        if (!host_supports_target_device(target)) {
            printf("Requested GPU is not supported; skipping this test. (Do you have the proper hardware and/or driver installed?)\n");
            return 0;
        }

        // Create an interesting input image to use.
        Buffer<int> input(258, 258);
        input.set_min(-1, -1);
        for (int y = input.top(); y <= input.bottom(); y++) {
            for (int x = input.left(); x <= input.right(); x++) {
                input(x, y) = x * 17 + y % 4;
            }
        }

        img.set(input);
        blur.compile_jit(target);
        Buffer<int> out = blur.realize({256, 256});

        // Check the output is what we expected
        for (int y = out.top(); y <= out.bottom(); y++) {
            for (int x = out.left(); x <= out.right(); x++) {
                int val = out(x, y);
                int expected = (input(x - 1, y - 1) + input(x, y - 1) + input(x + 1, y - 1) +
                                input(x - 1, y) + input(x, y) + input(x + 1, y) +
                                input(x - 1, y + 1) + input(x, y + 1) + input(x + 1, y + 1));
                if (val != expected) {
                    printf("out(%d, %d) = %d instead of %d\n",
                           x, y, val, expected);
                    return -1;
                }
            }
        }
    }

    {
        // Func::in can also be used to group multiple stages of a
        // Func into the same loop nest. Consider the following
        // pipeline, which computes a value per pixel, then sweeps
        // from left to right and back across each scanline.
        Func f("f_group"), g("g_group"), h("h_group");

        // Initialize f
        f(x, y) = sin(x - y);
        RDom r(1, 7);

        // Sweep from left to right
        f(r, y) = (f(r, y) + f(r - 1, y)) / 2;

        // Sweep from right to left
        f(7 - r, y) = (f(7 - r, y) + f(8 - r, y)) / 2;

        // Then we do something with a complicated access pattern: A
        // 45 degree rotation with wrap-around
        g(x, y) = f((x + y) % 8, (x - y) % 8);

        // f should be scheduled compute_root, because its consumer
        // accesses it in a complicated way. But that means all stages
        // of f are computed in separate loop nests:

        // for y:
        //   for x:
        //     f(x, y) = sin(x - y)
        // for y:
        //   for r:
        //     f(r, y) = (f(r, y) + f(r - 1, y)) / 2
        // for y:
        //   for r:
        //     f(7 - r, y) = (f(7 - r, y) + f(8 - r, y)) / 2
        // for y:
        //   for x:
        //     g(x, y) = f((x + y) % 8, (x - y) % 8);

        // We can get better locality if we schedule the work done by
        // f to share a common loop over y. We can do this by
        // computing f at scanlines of a wrapper like so:

        f.in(g).compute_root();
        f.compute_at(f.in(g), y);

        // f has the default schedule for a Func with update stages,
        // which is to be computed at the innermost loop of its
        // consumer, which is now the wrapper f.in(g). This therefore
        // generates the following loop nest, which has better
        // locality:

        // for y:
        //   for x:
        //     f(x, y) = sin(x - y)
        //   for r:
        //     f(r, y) = (f(r, y) + f(r - 1, y)) / 2
        //   for r:
        //     f(7 - r, y) = (f(7 - r, y) + f(8 - r, y)) / 2
        //   for x:
        //     f_in_g(x, y) = f(x, y)
        // for y:
        //   for x:
        //     g(x, y) = f_in_g((x + y) % 8, (x - y) % 8);

        // We'll additionally vectorize the initialization of, and
        // then transfer of pixel values from f into its wrapper:
        f.vectorize(x, 4);
        f.in(g).vectorize(x, 4);

        g.realize({8, 8});
        // See below for a visualization.

         

    }

    printf("Success!\n");

    return 0;
}