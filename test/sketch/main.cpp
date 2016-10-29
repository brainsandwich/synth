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

	std::vector<uss::core::DeviceInfo> devicesInfo;
	context.listDevicesInfo(std::back_inserter(devicesInfo));
	printDevicesInfo(devicesInfo.begin(), devicesInfo.end(), hostApis.begin());

	int devIndex = hostApis[0].defaultOutputDeviceIndex;
	std::cout << "Opening output device " << devIndex << " using API " << hostApis[0].name << std::endl;
	uss::core::Device device(&context, devIndex);
	device.channels = 2;
	device.latency = device.deviceInfo.output.defaultLowLatency;
	device.format = uss::core::Format::Float32;

	context.device_out = &device;
	
	uss::modular::Enveloppe enveloppe(&context);
	enveloppe.attack.value = 0.01f;
	enveloppe.decay.value = 0.01f;
	enveloppe.release.value = 0.5f;
	enveloppe.attackLevel.value = 1.0f;
	enveloppe.sustainLevel.value = 1.0f;

	uss::modular::Sequencer sequencer(&context);
	sequencer.clockRate = uss::bpmtf(120.0);
	sequencer.noteRate = 0.25f;
	sequencer.noteLength.value = 0.005f;
	sequencer.gate.connect(&enveloppe.gate);
	sequencer
		.addNote(uss::ntf(25))
		.addNote(uss::ntf(25))
		.addNote(uss::ntf(2))
		.addNote(uss::ntf(2))
		.addNote(uss::ntf(2))
		.addNote(uss::ntf(2))
		.addNote(uss::ntf(28))
		.addNote(uss::ntf(28))
		.addNote(uss::ntf(28))
		.addNote(uss::ntf(28))
		.addNote(uss::ntf(28))
		.addNote(uss::ntf(28))
		.addNote(uss::ntf(29))
		.addNote(uss::ntf(29))
		.addNote(uss::ntf(35))
		.addNote(uss::ntf(35));

	uss::modular::Oscillator lfo(&context);
	lfo.frequency.value = uss::bpmtf(120.0f) / 16.0f;
	lfo.unipolar = true;

	uss::modular::Gain lfogain(&context);
	lfogain.gain.value = 500.0f;
	lfogain.offset.value = 200.0f;
	lfo.destination.connect(&lfogain.input);

	uss::modular::Oscillator osc(&context);
	sequencer.currentNote.connect(&osc.frequency);
	osc.destination.connect(&enveloppe.input);
	osc.wavetable = &uss::core::squaretable;

	uss::modular::Mixer mixer(&context);
	mixer.destination.connect(&device.output);

	uss::modular::LowPassFilter lpf(&context);
	lpf.quality = 0.1f;
	lpf.cutoff = 500.0f;
	enveloppe.destination.connect(&lpf.input);
	lfogain.destination.connect(&lpf.cutoff);
	
	uss::modular::Saturator saturator(&context);
	saturator.hardness.value = 12.5f;
	lpf.destination.connect(&saturator.input);
	saturator.destination.connect(&mixer.getMonoInput(0));

	std::size_t count = 0;
	context.start(96000, 32);
	while (true) {
		std::this_thread::sleep_for(1ms);
		if (count++ >= 30000)
			break;
	}

	context.stop();
	context.close();

	return 0;
}
