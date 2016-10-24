#pragma once

#include "uss/core/common.hpp"
#include "uss/core/node.hpp"

#include "portaudio.h"

#include <iostream>
#include <map>
#include <thread>

namespace uss {

	namespace core {
		class Device;
		class Context {
		protected:
			bool initialized = false;
			std::vector<Node*> nodes;
			PaStream* stream = nullptr;

		public:
			Context();
			virtual ~Context();
			void addNode(Node* node);
			void rmvNode(Node* node);

			void update(double sampleRate);
			void start(double sampleRate = 44100, unsigned long bufferLength = 64);
			void stop();
			void close();

			Device* device_in;
			Device* device_out;

			StreamInfo streamInfo;
			StreamInfo getStreamInfo() const;
			DeviceInfo getDeviceInfo(int index) const;

			HostApi getHostApi(int index) const;
			template <typename Output> void listHostApis(Output out) const {
				if (!initialized) {
					std::cerr << "Error, context is not initialized" << std::endl;
					return;
				}

				int hostApiCount = Pa_GetHostApiCount();
				if (hostApiCount < 0) {
					std::cerr << "Error, no host api found: " << Pa_GetErrorText(hostApiCount) << std::endl;
					return;
				}
				
				for (int i = 0; i < hostApiCount; i++)
					*out++ = getHostApi(i);
			}

			template <typename Output> void listDevicesInfo(Output out) const {
				if (!initialized) {
					std::cerr << "Error, context is not initialized" << std::endl;
					return;
				}

				int deviceCount = Pa_GetDeviceCount();
				if (deviceCount < 0) {
					std::cerr << "Error, no device found: " << Pa_GetErrorText(deviceCount) << std::endl;
					return;
				}
				
				for (int i = 0; i < deviceCount; i++)
					*out++ = getDeviceInfo(i);
			}
		};
	}

}
