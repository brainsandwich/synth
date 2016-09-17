#include <cstdlib>
#include <cmath>

#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>

#include <uss.hpp>

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

	uss::core::init_wavetables();
	uss::core::Context context;

	std::vector<uss::core::HostApi> hostApis;
	context.listHostApis(std::back_inserter(hostApis));
	printHostApis(hostApis.begin(), hostApis.end());

	// std::vector<uss::core::DeviceInfo> devicesInfo;
	// context.listDevicesInfo(std::back_inserter(devicesInfo));
	// printDevicesInfo(devicesInfo.begin(), devicesInfo.end(), hostApis.begin());

	int devIndex = hostApis[0].defaultOutputDeviceIndex;
	std::cout << "Opening output device " << devIndex << std::endl;
	uss::core::Device device(&context, devIndex);
	device.channels = 2;
	device.latency = device.deviceInfo.output.defaultLowLatency;
	device.format = uss::core::Format::Float32;

	context.device_out = &device;
	
	uss::modular::Enveloppe enveloppe(&context);
	enveloppe.attack.value = 0.01f;
	enveloppe.decay.value = 0.01f;
	enveloppe.release.value = 0.01f;
	enveloppe.attackLevel.value = 1.0f;
	enveloppe.sustainLevel.value = 1.0f;

	uss::modular::Oscillator trigger(&context);
	trigger.wavetable = &uss::core::squaretable;
	trigger.unipolar = true;
	trigger.frequency.value = 20.0f;
	trigger.destination.connect(&enveloppe.gate);

	uss::modular::Oscillator osc(&context);
	osc.frequency.value = 500.0f;
	osc.destination.connect(&enveloppe.input);

	uss::modular::Mixer mixer(&context);
	enveloppe.destination.connect(&mixer.getMonoInput(0));
	mixer.destination.connect(&device.output);

	std::size_t count = 0;
	context.start(44100, 64);
	while (true) {
		std::this_thread::sleep_for(10ms);
		std::cout << "env: " << enveloppe.enveloppe.value << std::endl;
		if (count++ >= 3000)
			break;
	}

	context.stop();
	context.close();

	return 0;
}
