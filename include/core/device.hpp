#pragma once

#include "portaudio.h"
#include "core/common.hpp"

namespace audio {

	class Context;
	class Device {
		friend class Context;
	protected:
		int index;
		Context* context = nullptr;
		PaStream* stream = nullptr;
		double time = 0.0;

		static int callback(const void *inputBuffer, void *outputBuffer,
	                       unsigned long framesPerBuffer,
	                       const PaStreamCallbackTimeInfo* timeInfo,
	                       PaStreamCallbackFlags statusFlags,
	                       void* userData);

	public:
		Device(Context* context, int index);
		StereoSignal output;

		DeviceInfo deviceInfo;
		StreamInfo streamInfo;

		StreamInfo getStreamInfo() const;
		Device& open(unsigned int channels, unsigned int sampleRate, const Format& format = Format::Float32, bool isInput = false);
		Device& start();
		Device& stop();
		Device& close();
	};
}
