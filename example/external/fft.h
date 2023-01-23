// 
// Fast Fourier Transform using Cooley-Tukey algorithm
//
// 2022, Jonathan Tainer
//

#ifndef FFT_H
#define FFT_H

#include <complex.h>

// Reads freq. domain signal from input buffer x
// Writes time domain signal to output buffer X
// n (number of samples) must be a power of 2
void fft(float* x, float complex* X, unsigned int n);

void fft_mag(float* input, float* output, unsigned int n);

#endif
