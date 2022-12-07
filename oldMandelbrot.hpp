#pragma once
#include <vector>

class Mandelbrot{
      public:
            static int calc(double re, double im, int iterations);
            static double scale(double val, double targetMin, double targetMax, double sourceMin, double sourceMax);
};



