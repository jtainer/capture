// 
// FFT spectrum analyzer
//
// 2023, Jonathan Tainer
//

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <math.h>
#include <string.h>
#include <raylib.h>

#include "../capture.h"
#include "external/fft.h"

#define FT_SAMPLES 1024

#define SMOOTHING_CONST 0.95f	// 0.f == no smoothing, 1.f = max smoothing (no movement)

static float sampleBuffer[FT_SAMPLES];
static float outputBuffer[FT_SAMPLES];
static float resultBuffer[FT_SAMPLES];
static sem_t sampleBufferMutex;

// Callback to pull samples from miniaudio on the fly
void StreamProcessor(void* data, unsigned int frameCount) {
	sem_wait(&sampleBufferMutex);
	float* sample = (float*) data;
	for (int i = 0; i < frameCount && i < FT_SAMPLES; i++) {
		sampleBuffer[i] = sample[i * 2];
	}
	sem_post(&sampleBufferMutex);
}

// Modified sigmoid function for limiting bar height
float sigmoid(float x) {
	return 2.f / (1 + exp(-x)) - 1;
}

int main(int argc, char** argv) {

	// Window setup
	int screenWidth = 0;
	int screenHeight = 0;
	SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN | FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, argv[1]);
	SetTargetFPS(120);
	int monitor = GetCurrentMonitor();
	screenWidth = GetMonitorWidth(monitor);
	screenHeight = GetMonitorHeight(monitor);
	SetWindowSize(screenWidth, screenHeight);
	ToggleFullscreen();
	HideCursor();

	memset(sampleBuffer, 0, sizeof(float) * FT_SAMPLES);
	memset(outputBuffer, 0, sizeof(float) * FT_SAMPLES);
	memset(resultBuffer, 0, sizeof(float) * FT_SAMPLES);

	// Load music file and setup stream processor
	sem_init(&sampleBufferMutex, 0, 1);
	InitAudioCaptureDevice();
	AttachAudioCaptureProcessor(StreamProcessor);

	// Colors for background and bars
	Color bgColor = BLACK, barColor = WHITE;

	while (!WindowShouldClose()) {

		// Process most recent samples
		sem_wait(&sampleBufferMutex);
		fft_mag(sampleBuffer, outputBuffer, FT_SAMPLES);
		sem_post(&sampleBufferMutex);

		BeginDrawing();
		ClearBackground(bgColor);
		for (int i = 0; i < FT_SAMPLES; i++) {
			resultBuffer[i] = (SMOOTHING_CONST * resultBuffer[i]) + ((1.f - SMOOTHING_CONST) * outputBuffer[i]);
			Vector2 size = { (float) screenWidth / FT_SAMPLES, sigmoid(resultBuffer[i]) * screenHeight };
			Vector2 pos = { i * size.x, screenHeight - size.y };
			DrawRectangleV(pos, size, barColor);
		}
		EndDrawing();
	}

	// Cleanup
	CloseAudioCaptureDevice();
	CloseWindow();
	sem_destroy(&sampleBufferMutex);
	return 0;
}
