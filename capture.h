// 
// Simple audio capture interface made using miniaudio
//
// 2022, Jonathan Tainer
//

#ifndef LIBCAPTURE_H
#define LIBCAPTURE_H

typedef void (*AudioCaptureCallback)(void* data, unsigned int frames);

// API functions
void InitAudioCaptureDevice();
void AttachAudioCaptureProcessor(AudioCaptureCallback);
void CloseAudioCaptureDevice();

#endif
