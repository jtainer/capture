// 
// Simple audio capture interface made using miniaudio
//
// 2023, Jonathan Tainer
//

#include "capture.h"

#define MINIAUDIO_IMPLEMENTATION
#include "external/miniaudio.h"

#include <stdio.h>
#include <stdlib.h>

static struct {
	AudioCaptureCallback processor;
	ma_device_config config;
	ma_device device;
} core;

// API functions
void InitAudioCaptureDevice();
void AttachAudioCaptureProcessor(AudioCaptureCallback);
void CloseAudioCaptureDevice();

// Internal functions
static void DataCallback(ma_device*, void*, void*, unsigned int);

void InitAudioCaptureDevice() {
	core.processor = NULL;

	core.config = ma_device_config_init(ma_device_type_capture);
	core.config.capture.pDeviceID	= NULL;
	core.config.capture.format	= ma_format_f32;
	core.config.capture.channels	= 2;
	core.config.capture.shareMode	= ma_share_mode_shared;
	core.config.dataCallback	= (ma_device_data_proc) DataCallback;

	ma_result result = ma_device_init(NULL, &core.config, &core.device);
	if (result != MA_SUCCESS) {
		printf("Failed to initialize capture device\n");
	}

	ma_device_start(&core.device);
}

void AttachAudioCaptureProcessor(AudioCaptureCallback processor) {
	core.processor = processor;
}

void CloseAudioCaptureDevice() {
	ma_device_uninit(&core.device);
}

static void DataCallback(ma_device* device, void* output, void* input, unsigned int frameCount) {
	if (core.processor)
		core.processor(input, frameCount);
}

