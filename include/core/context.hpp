#pragma once

#include "core/device.hpp"
#include "core/common.hpp"

#include <iostream>
#include <map>
#include <thread>

namespace audio {

	namespace modular { class Instrument; }

	class Context {
	protected:
		bool initialized = false;
		std::vector<Device*> devices;
		modular::Instrument* instrument = nullptr;

		void addDevice(Device* device);
		void rmvDevice(Device* device);

	public:
		Context();
		virtual ~Context();
		void setInstrument(modular::Instrument* instrument);
		void update(double sampleRate);
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
