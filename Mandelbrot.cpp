#include "Mandelbrot.hpp"


int Mandelbrot::calc(double re, double im, int max_iterations) {
      int iterations = 0;
      double zRe = 0.0;
      double zIm = 0.0;
      double buffer;

      while(zRe*zRe + zIm*zIm <= 4.0 && iterations < max_iterations){
            buffer = zRe*zRe - zIm*zIm  + re;
            zIm =  2.0 * zRe * zIm + im;
            zRe = buffer;
            iterations++;
      }
      
      return iterations;
}


double Mandelbrot::scale(double val, double targetMin, double targetMax, double sourceMin, double sourceMax) {
      return (targetMax - targetMin)*(val - sourceMin)/(sourceMax - sourceMin) + targetMin;
}
