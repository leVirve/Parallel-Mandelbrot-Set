#include "utils.h"
using namespace std;

void initial_env(int argc, char** argv) {
    if (argc < 8)
        throw "#threads (real-range) (imag-range) width height switch-display";
    num_thread = stoi(argv[1]), width = stoi(argv[6]), height = stoi(argv[7]);
    real_min = stod(argv[2]), imag_min = stod(argv[4]);
    double imag_max = stod(argv[5]), real_max = stod(argv[3]);
    gui = argv[8] == string("enable");

    dx = (real_max - real_min) / width;
    dy = (imag_max - imag_min) / height;
}

void Timer::start() {
    s = high_resolution_clock::now();
}

unsigned int Timer::stop() {
    return duration_cast<nanoseconds>(high_resolution_clock::now() - s).count();
}
