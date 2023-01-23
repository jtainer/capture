// 
// Fast Fourier Transform using Cooley-Tukey algorithm
//
// 2022, Jonathan Tainer
//

#include "fft.h"
#include <math.h>
#include <stdlib.h>

// Reads signal from input buffer x
// Writes result to output buffer X
// n (num of samples) must be a power of 2
// s (stride length) must be 1
static void fft_recurse(float* x, float complex* X, unsigned int n, unsigned int s) {
	if (n == 1) {
		X[0] = x[0] + 0 * I;
	}
	else {
		// Recursively split the DFT in halves
		fft_recurse(x, X, n/2, s*2);
		fft_recurse(x+s, X+n/2, n/2, 2*s);

		// Combine halves into full DFT
		for (unsigned int k = 0; k < n/2; k++) {
			float complex p = X[k];
			float complex u = cexpf(I*-2*M_PI*k/n);
			float complex q = u * X[k+n/2];
			X[k] = p + q;
			X[k+n/2] = p - q;
		}
	}
}

// Wrapper for recursive function call
void fft(float* x, float complex* X, unsigned int n) {
	// TODO: check params for validity here (not inside fft_recurse)
	fft_recurse(x, X, n, 1);
}

void fft_mag(float* input, float* output, unsigned int n) {
	// Allocate temporary buffers to operate on
	float* tmp_input = malloc(sizeof(float) * n);
	float complex* tmp_output = malloc(sizeof(float complex) * n);

	// Apply Hann window to input buffer to reduce windowing effect
	for (unsigned int i = 0; i < n; i++) {
		tmp_input[i] = input[i] * sin(M_PI * i / n) * sin(M_PI * i / n);
	}
	
	// Perform FFT
	fft(tmp_input, tmp_output, n);
	
	// Calculate magnitude of complex freq. components
	for (unsigned int i = 0; i < n; i++) {
		output[i] = cabsf(tmp_output[i]);
	}


	free(tmp_input);
	free(tmp_output);
}
