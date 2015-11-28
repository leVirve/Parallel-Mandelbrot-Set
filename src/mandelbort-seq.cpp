#include "utils.h"
#include "display.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;

int main(int argc, char** argv) {
    // int width = 800, height = 800;
    int x = 0, y = 0;

    try {
        initial_env(argc, argv);
        create_display(x, y, height, width);
    } catch (char const* err) {
        cerr << err << endl;
        return 0;
    }

    ComplexNum z, c;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int repeats = 0;
            double lengthsq = 0.0;
            z.real = 0.0, z.imag = 0.0;
            c.real = i * dx + real_min;
            c.imag = j * dy + imag_min;
            while (repeats < 100000 && lengthsq < 4.0) {
                double temp = z.real * z.real - z.imag * z.imag + c.real;
                z.imag = 2 * z.real * z.imag + c.imag;
                z.real = temp;
                lengthsq = z.real * z.real + z.imag * z.imag;
                repeats++;
            }
            draw_point(i, j, repeats);
        }
    }
    flush();
    sleep(5);
    return 0;
}
