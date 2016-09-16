#pragma once

#include <memory>
#include <string>
#include <vector>

#include <cmath>

const double PI = 3.14159265;

namespace audio {

	const float MIN_FREQ = 0.00001f;
	
	// -------------------------------------------------------------
	// Info

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

	// -------------------------------------------------------------
	// Signals

	struct Signal {
		virtual ~Signal() {}
		virtual void update() = 0;
	};

	struct MonoSignal : Signal {
		std::vector<MonoSignal*> targets;
		float value = 0.0f;

		void update() override;
		void connect(MonoSignal* signal);
		void disconnect(MonoSignal* signal);
	};

	struct StereoSignal : Signal {
		std::vector<StereoSignal*> targets;
		float left = 0.0f;
		float right = 0.0f;

		void update() override;
		void connect(StereoSignal* signal);
		void disconnect(StereoSignal* signal);
	};

	// -------------------------------------------------------------
	// Wave Tables

	struct WaveTable {
	protected:
		std::vector<float> data;

	public:
		WaveTable(std::size_t size = 0)
			: data(size) {}

		float& operator[](std::size_t index) { return data[index]; }
		std::size_t size() const { return data.size(); }
		void resize(std::size_t size) { data.resize(size); }
	};

	extern WaveTable sinetable;
	extern WaveTable sawtable;
	extern WaveTable squaretable;

	void generate_sine(WaveTable& table);
	void generate_sawtooth(WaveTable& table);
	void generate_square(WaveTable& table);

	// -------------------------------------------------------------
	// Math

	template <typename T> 
	T clamp(T value, T min, T max) {
		return std::min(std::max(value, min), max);
	}

	template <typename T>
	T exp(T value) {
		return 1.0 + value + (value * value) / 2.0;
	}
}
