#include "core/context.hpp"
#include "core/device.hpp"

#include <iostream>
#include <algorithm>
#include <chrono>

namespace uss {
	namespace core {

		namespace {
			static int callback(const void *inputBuffer, void *outputBuffer,
                   unsigned long framesPerBuffer,
                   const PaStreamCallbackTimeInfo* timeInfo,
                   PaStreamCallbackFlags statusFlags,
                   void* userData)
			{
				auto& self = *(Context*) userData;
				auto in = (float*) inputBuffer;
			    auto out = (float*) outputBuffer;
			    
			    // Working only with output mono or stereo for now
			    for (unsigned long i = 0; i < framesPerBuffer; i++) {
			    	self.update(self.streamInfo.sampleRate);
			    	switch (self.device_out->channels) {
			    		case 1:
			    			*out++ = self.device_out->output.left;
			    			break;
			    		case 2:
			    			*out++ = self.device_out->output.left;
			    			*out++ = self.device_out->output.right;
			    			break;
			    	}
			    }
			    return 0;
			}
		}

		Context::Context() {
			int err = Pa_Initialize();
			if (err < 0) {
				std::cerr << "Error, couldn't initialize PortAudio: " << Pa_GetErrorText(err) << std::endl;
				return;
			}

			initialized = true;
		}

		Context::~Context() {
			if (initialized)
				Pa_Terminate();
		}

		void Context::addNode(Node* node) {
			nodes.push_back(node);
		}

		void Context::rmvNode(Node* node) {
			auto iter = std::find(nodes.begin(), nodes.end(), node);
			if (iter != nodes.end())
				nodes.erase(iter);
		}

		void Context::update(double sampleRate) {
			for (auto& node: nodes)
				node->update(sampleRate);
		}

		void Context::start(double sampleRate, unsigned long bufferLength) {
			if (!initialized)
				return;

			PaStreamParameters parameters_in, parameters_out;

			if (device_out) {
				parameters_out.device = device_out->index;
				parameters_out.channelCount = device_out->channels;
				parameters_out.sampleFormat = paFloat32;	// TODO convert Format to paFormat
				parameters_out.suggestedLatency = device_out->latency;
				parameters_out.hostApiSpecificStreamInfo = nullptr;
			}

			if (device_in) {
				parameters_in.device = device_in->index;
				parameters_in.channelCount = device_in->channels;
				parameters_in.sampleFormat = paFloat32;	// TODO convert Format to paFormat
				parameters_in.suggestedLatency = device_in->latency;
				parameters_in.hostApiSpecificStreamInfo = nullptr;
			}

			int err = Pa_OpenStream(&stream,
				device_in ? &parameters_in : nullptr,
				device_out ? &parameters_out : nullptr,
				sampleRate, bufferLength,
				paNoFlag, callback,
				this);

			if (err < 0)
				std::cerr << "Error, couldn't open stream: " << Pa_GetErrorText(err) << std::endl;

			streamInfo = getStreamInfo();

			err = Pa_StartStream(stream);
			if (err < 0)
				std::cerr << "Error, couldn't start device: " << Pa_GetErrorText(err) << std::endl;
		}

		void Context::stop() {
			if (!initialized)
				return;

			int err = Pa_StopStream(stream);
			if (err < 0)
				std::cerr << "Error, couldn't stop device: " << Pa_GetErrorText(err) << std::endl;
		}

		void Context::close() {
			if (!initialized)
				return;

			int err = Pa_CloseStream(stream);
			if (err < 0)
				std::cerr << "Error, couldn't close device: " << Pa_GetErrorText(err) << std::endl;
		}

		StreamInfo Context::getStreamInfo() const {
			const PaStreamInfo* inf = Pa_GetStreamInfo(stream);
			StreamInfo result;
			result.inputLatency = inf->inputLatency;
			result.outputLatency = inf->outputLatency;
			result.sampleRate = inf->sampleRate;
			return result;
		}

		DeviceInfo Context::getDeviceInfo(int index) const {
			if (!initialized) {
				std::cerr << "Error, context is not initialized" << std::endl;
				return DeviceInfo();
			}

			const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(index);
			DeviceInfo result;

			// Commmon
			result.index = index;
			result.name = deviceInfo->name;
			result.defaultSampleRate = deviceInfo->defaultSampleRate;
			result.hostApiIndex = deviceInfo->hostApi;

			// Input
			result.input.maxChannels = deviceInfo->maxInputChannels;
			result.input.defaultLowLatency = deviceInfo->defaultLowInputLatency;
			result.input.defaultHighLatency = deviceInfo->defaultHighInputLatency;

			// Output
			result.output.maxChannels = deviceInfo->maxOutputChannels;
			result.output.defaultLowLatency = deviceInfo->defaultLowOutputLatency;
			result.output.defaultHighLatency = deviceInfo->defaultHighOutputLatency;
			return result;
		}

		HostApi Context::getHostApi(int index) const {
			const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(index);
			HostApi hostApi;
			hostApi.index = hostApiInfo->type;
			hostApi.name = hostApiInfo->name;
			hostApi.deviceCount = hostApiInfo->deviceCount;
			hostApi.defaultInputDeviceIndex = hostApiInfo->defaultInputDevice;
			hostApi.defaultOutputDeviceIndex = hostApiInfo->defaultOutputDevice;
			return hostApi;
		}

	}
}
