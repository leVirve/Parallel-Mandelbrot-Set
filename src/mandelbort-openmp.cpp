#include <stdio.h>
#include <omp.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include "display.h"
using namespace std;

struct ComplexNum {
    double real, imag;
};

int main(void) {
    int width = 800, height = 800;
    int x = 0, y = 0;

    try {
        create_display(x, y, height, width);
        map_window();
    } catch (string err) {
        cerr << err << endl;
        return 0;
    }

    ComplexNum z, c;
    #pragma omp parallel for schedule(static, 10) private(z, c) collapse(2)
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int repeats = 0;
            double lengthsq = 0.0;
            z.real = 0.0, z.imag = 0.0;
            c.real = ((double)i - 400.0) / 200.0;
            c.imag = ((double)j - 400.0) / 200.0;

            while (repeats < 100000 && lengthsq < 4.0) {
                double temp = z.real * z.real - z.imag * z.imag + c.real;
                z.imag = 2 * z.real * z.imag + c.imag;
                z.real = temp;
                lengthsq = z.real * z.real + z.imag * z.imag;
                repeats++;
            }
            #pragma omp critical
            draw_point(i, j, repeats);
        }
    }
    flush();
    sleep(5);
    return 0;
}
