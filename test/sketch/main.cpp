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

	std::size_t voices = 3;
	std::vector<std::shared_ptr<audio::Oscillator>> oscillators(voices);
	std::vector<std::shared_ptr<audio::Gain>> multipliers(voices);
	std::vector<std::shared_ptr<audio::Gain>> gains(voices);

	for (std::size_t i = 0; i < voices; i++) {
		oscillators[i] = std::make_shared<audio::Oscillator>(&context);
		oscillators[i]->shape.value = 0.5f;

		gains[i] = std::make_shared<audio::Gain>(&context);
		// gains[i]->gain.value = 1.0f;
		gains[i]->gain.value = (float) (voices - i) / (float) voices;
		gains[i]->destination.connect(&device.output[i]);
		oscillators[i]->destination.connect(&gains[i]->input);
	}

	oscillators[0]->frequency.value = 100.0f;
	for (std::size_t i = 1; i < voices; i++) {
		multipliers[i] = std::make_shared<audio::Gain>(&context);
		multipliers[i]->gain.value = 1.1f * i;
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

	audio::Oscillator lfo(&context);
	lfo.frequency.value = 2.0f;
	lfo.shape.value = 1.0f;

	audio::Gain lfoGain(&context);
	lfoGain.gain.value = 10.0f;
	for (std::size_t i = 0; i < voices; i++)
		lfoGain.destination.connect(&oscillators[i]->modulation);
	lfo.destination.connect(&lfoGain.input);

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
