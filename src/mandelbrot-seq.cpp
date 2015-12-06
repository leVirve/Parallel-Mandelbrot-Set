#include "mandelbrot_utils.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;

void calc()
{
    Timer timer;
    ComplexNum c;
    timer.start();
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            c.real = i * dx + real_min;
            c.imag = j * dy + imag_min;
            int repeats = calc_pixel(c);
            if (gui) draw_point(i, j, repeats);
        }
    }
    cout << "Runs in " << (double)(timer.stop()) / 1000 << " us" << endl;
    if (gui) { flush(); sleep(2); }
}

int main(int argc, char** argv) {
    int x = 0, y = 0;
    try {
        initial_env(argc, argv);
        create_display(x, y, height, width);
        calc();
    } catch (char const* err) {
        cerr << err << endl;
    }
    return 0;
}
