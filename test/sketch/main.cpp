#include <cstdlib>
#include <cmath>

#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>

#include <audio.hpp>

template <typename Input>
void printHostApis(Input first, Input last) {
	std::size_t distance = last - first;
	std::cout << "Host Apis: (" << distance << ")" << std::endl;
	while (first != last) {
		auto api = *first++;
		std::cout << std::string(64, '-') << std::endl;
		std::cout << "api " << api.index << " " << api.name << std::endl;
		std::cout << "- deviceCount : " << api.deviceCount << std::endl;
		std::cout << "- defaultInputDeviceIndex : " << api.defaultInputDeviceIndex << std::endl;
		std::cout << "- defaultOutputDeviceIndex : " << api.defaultOutputDeviceIndex << std::endl;
		std::cout << std::endl;
	}
}

template <typename InputDevices, typename InputHostApis>
void printDevicesInfo(InputDevices first, InputDevices last, InputHostApis hostApisFirst) {
	std::size_t distance = last - first;
	std::cout << "Devices: (" << distance << ")" << std::endl;
	while (first != last) {
		auto dev = *first++;
		std::cout << std::string(64, '-') << std::endl;
		std::cout << "dev " << dev.index << " " << dev.name << std::endl;
		std::cout << "host api : " << (*(hostApisFirst + dev.hostApiIndex)).name << std::endl;
		std::cout << "- defaultSampleRate : " << dev.defaultSampleRate << std::endl;
		std::cout << "- input: " << std::endl;
		std::cout << "\t- maxChannels : " << dev.input.maxChannels << std::endl;
		std::cout << "\t- defaultLowLatency : " << dev.input.defaultLowLatency << std::endl;
		std::cout << "\t- defaultHighLatency : " << dev.input.defaultHighLatency << std::endl;
		std::cout << "- output: " << std::endl;
		std::cout << "\t- maxChannels : " << dev.output.maxChannels << std::endl;
		std::cout << "\t- defaultLowLatency : " << dev.output.defaultLowLatency << std::endl;
		std::cout << "\t- defaultHighLatency : " << dev.output.defaultHighLatency << std::endl;
		std::cout << std::endl;
	}
}

enum class Shape {
	SINE, SAW, SQUARE
};

struct LFO {
	audio::Oscillator osc;
	audio::Gain gain;
	LFO(audio::Context* context)
		: osc(audio::Oscillator(context))
		, gain(audio::Gain(context))
	{
		osc.destination.connect(&gain.input);
	}

	LFO& setShape(const Shape& shape) {
		switch(shape) {
			case Shape::SINE:
				osc.shape.value = 0.0f;
				break;
			case Shape::SAW:
				osc.shape.value = 0.5f;
				break;
			case Shape::SQUARE:
				osc.shape.value = 1.0f;
				break;
		}
		return *this;
	}
};

int main(int argc, char const *argv[]) {
	using namespace std::chrono_literals;

	audio::Context context;

	std::vector<audio::HostApi> hostApis;
	context.listHostApis(std::back_inserter(hostApis));
	printHostApis(hostApis.begin(), hostApis.end());

	// std::vector<audio::DeviceInfo> devicesInfo;
	// context.listDevicesInfo(std::back_inserter(devicesInfo));
	// printDevicesInfo(devicesInfo.begin(), devicesInfo.end(), hostApis.begin());

	int devIndex = hostApis[0].defaultOutputDeviceIndex;
	std::cout << "Opening output device " << devIndex << std::endl;
	audio::Device device(&context, devIndex);

	std::size_t voices = 4;
	std::vector<std::shared_ptr<audio::Oscillator>> oscillators(voices);
	std::vector<std::shared_ptr<audio::Gain>> multipliers(voices);
	std::vector<std::shared_ptr<audio::Gain>> gains(voices);

	audio::Gain output(&context);
	output.destination.connect(&device.output[0]);
	output.gain.value = 1.0f;

	audio::LowPassFilter filter(&context);
	filter.cutoff.value = 1200.0f;
	filter.quality.value = 0.8f;
	filter.destination.connect(&output.input);


	for (std::size_t i = 0; i < voices; i++) {
		oscillators[i] = std::make_shared<audio::Oscillator>(&context);
		oscillators[i]->shape.value = 1.0f;

		gains[i] = std::make_shared<audio::Gain>(&context);
		gains[i]->gain.value = (float) (voices - i) / (float) voices;
		gains[i]->destination.connect(&filter.input);
		oscillators[i]->destination.connect(&gains[i]->input);
	}

	oscillators[0]->frequency.value = 75.0f;
	for (std::size_t i = 1; i < voices; i++) {
		multipliers[i] = std::make_shared<audio::Gain>(&context);
		multipliers[i]->gain.value = 1.5f * i;
		oscillators[0]->frequency.connect(&multipliers[i]->input);
		multipliers[i]->destination.connect(&oscillators[i]->frequency);
	}

	// audio::Oscillator osc(&context);
	// osc.frequency.value = 100.0;
	// osc.shape.value = 0.0f;

	// audio::Gain oscGain(&context);
	// oscGain.gain.value = 1.0f;
	// oscGain.destination.connect(&device.output[0]);
	// osc.destination.connect(&oscGain.input);

	float base = 10.0f;

	LFO lfo(&context);
	lfo.setShape(Shape::SQUARE);
	lfo.osc.frequency.value = base / 2.0f;
	lfo.gain.gain.value = 0.5f;
	for (std::size_t i = 0; i < voices; i++)
		lfo.gain.destination.connect(&oscillators[i]->modulation);

	LFO sublfo(&context);
	sublfo.setShape(Shape::SINE);
	sublfo.osc.frequency.value = base / 5.0f;
	sublfo.gain.gain.value = 20.0f;
	sublfo.gain.destination.connect(&lfo.gain.gain);

	LFO wawlfo(&context);
	wawlfo.setShape(Shape::SINE);
	wawlfo.osc.frequency.value = base / 10.0f;
	wawlfo.gain.gain.value = 20.0f;
	wawlfo.gain.destination.connect(&lfo.osc.modulation);

	audio::Gain cutoffgain(&context);
	cutoffgain.gain.value = 1900.0f;
	cutoffgain.offset.value = 2500.0f;
	cutoffgain.destination.connect(&filter.cutoff);
	sublfo.osc.destination.connect(&cutoffgain.input);

	device.open(2, 44100);
	device.start();

	std::size_t count = 0;
	while (true) {
		std::this_thread::sleep_for(10ms);
		if (count++ >= 3000)
			break;
	}

	device.stop();
	device.close();

	return 0;
}
