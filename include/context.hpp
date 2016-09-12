#pragma once

#include "device.hpp"
#include "common.hpp"
#include "nodes/node.hpp"

#include <map>
#include <iostream>

namespace audio {

	class Context {
		friend struct Node;

	protected:
		bool initialized = false;
		std::vector<Device*> devices;
		std::vector<Node*> nodes;

		void addDevice(Device* device);
		void rmvDevice(Device* device);
		void addNode(Node* node);
		void rmvNode(Node* node);

	public:
		Context();
		virtual ~Context();
		void init();
		DeviceInfo getDeviceInfo(int index) const;
		HostApi getHostApi(int index) const;
		void update(double sampleRate);

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
