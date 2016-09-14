#include "device.hpp"
#include "context.hpp"
#include "common.hpp"

#include <iostream>

namespace audio {

	int Device::callback(const void *inputBuffer, void *outputBuffer,
	                   unsigned long framesPerBuffer,
	                   const PaStreamCallbackTimeInfo* timeInfo,
	                   PaStreamCallbackFlags statusFlags,
	                   void* userData)
	{
		auto& self = *(Device*) userData;
	    auto out = (float*) outputBuffer;
	    
	    // Working only with stereo for now
	    for (unsigned long i = 0; i < framesPerBuffer; i++) {
	    	self.context->update(self.streamInfo.sampleRate);
	    	*out++ = self.output.left;
	    	*out++ = self.output.right;
	    }
	    return 0;
	}

	Device::Device(Context* context, int index)
		: index(index)
		, context(context)
	{
		deviceInfo = context->getDeviceInfo(index);
	}

	Device& Device::open(unsigned int channels, unsigned int sampleRate, const Format& format, bool isInput) {
		PaStreamParameters parameters;
		parameters.device = index;
		parameters.channelCount = channels;
		parameters.sampleFormat = paFloat32;	// TODO convert Format to paFormat
		parameters.suggestedLatency = deviceInfo.output.defaultLowLatency;	// TODO suggest latency
		parameters.hostApiSpecificStreamInfo = nullptr;

		int err = Pa_OpenStream(&stream,
			isInput ? &parameters : nullptr,
			isInput ? nullptr : &parameters,
			sampleRate, 64,
			paNoFlag, callback,
			this);

		if (err < 0)
			std::cerr << "Error, couldn't open stream: " << Pa_GetErrorText(err) << std::endl;

		streamInfo = getStreamInfo();

		return *this;
	}

	StreamInfo Device::getStreamInfo() const {
		const PaStreamInfo* inf = Pa_GetStreamInfo(stream);
		StreamInfo result;
		result.inputLatency = inf->inputLatency;
		result.outputLatency = inf->outputLatency;
		result.sampleRate = inf->sampleRate;
		return result;
	}

	Device& Device::start() {
		int err = Pa_StartStream(stream);
		if (err < 0)
			std::cerr << "Error, couldn't start device: " << Pa_GetErrorText(err) << std::endl;

		return *this;
	}

	Device& Device::stop() {
		int err = Pa_StopStream(stream);
		if (err < 0)
			std::cerr << "Error, couldn't stop device: " << Pa_GetErrorText(err) << std::endl;

		return *this;
	}

	Device& Device::close() {
		int err = Pa_CloseStream(stream);
		if (err < 0)
			std::cerr << "Error, couldn't close device: " << Pa_GetErrorText(err) << std::endl;

		return *this;
	}

}
