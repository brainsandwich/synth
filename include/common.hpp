#pragma once

#include <memory>
#include <string>

#include <cmath>

const double PI = 3.14159265;

namespace audio {

	const float MIN_FREQ = 0.00001f;
	
	struct HostApi {
		int index;
		std::string name;
		int deviceCount;
		int defaultInputDeviceIndex;
		int defaultOutputDeviceIndex;
	};

	struct DeviceInfo {
		int index;
		std::string name;
		double defaultSampleRate;
		int hostApiIndex;
		struct {
			unsigned int maxChannels;
			double defaultLowLatency;
			double defaultHighLatency;
		} input;
		struct {
			unsigned int maxChannels;
			double defaultLowLatency;
			double defaultHighLatency;
		} output;
	};

	struct StreamInfo {
		double inputLatency;
		double outputLatency;
		double sampleRate;
	};
	
	enum class Format {
		Float32,
		Float64
	};

	struct SineTable {
		static const std::size_t SIZE = 2048;
		static float wave[SIZE];
		static void init() {
			for (std::size_t i = 0; i < SIZE; i++)
				wave[i] = sin(((float) i * 2.0 * PI) / ((float) SIZE));
		}
	};

	struct SquareTable {
		static const std::size_t SIZE = 2048;
		static float wave[SIZE];
		static void init() {
			for (std::size_t i = 0; i < SIZE; i++)
				wave[i] = (i < SIZE / 2) ? 1.0 : 0.0;
		}
	};

	struct SawTable {
		static const std::size_t SIZE = 2048;
		static float wave[SIZE];
		static void init() {
			for (std::size_t i = 0; i < SIZE; i++)
				wave[i] = (float) i / (float) SIZE;
		}
	};

}
