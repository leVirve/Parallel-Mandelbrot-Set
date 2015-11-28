#include <stdio.h>
#include <unistd.h>
#include <iostream>

struct ComplexNum {
    double real, imag;
};

extern int num_thread, width, height;
extern double real_min, imag_min, dx, dy;
extern bool gui;

void initial_env(int argc, char** argv);
