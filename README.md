# Ultra Sound System

Ultra Sound System is a framework designed for audio synthesis using PortAudio as backend, inspired
by the WebAudio API.

## How does that work ?

The "sketch" test in the test folder is subject to change, but the basics of
firing up a context and throwing audio waves at your speakers are there. All
the definitions are found in the `uss::` namespace.

### uss::core::Context

First, you need a `uss::core::Context`. From that class you can fetch information about
audio devices or apis used. It also contains pointers to `uss::core::Node`s and `uss::core::Device`s in
use, but we'll check the `uss::core::Node`s after.

### uss::core::Device

Now you want an output `uss::core::Device` (for now it can only be output tho). The ideal
is to find out what is the default device to use with your api of choice. In
`sketch.cpp` I'm using the default device of the first `uss::core::HostApi` listed by the
`uss::core::Context`. Just do it like this :

	// Get the list of host apis
	std::vector<uss::core::HostApi> hostApis;
	context.listHostApis(std::back_inserter(hostApis));

	// Get default output device index (integer)
	int devIndex = hostApis[0].defaultOutputDeviceIndex;
	std::cout << "Opening output device " << devIndex << std::endl;
	uss::core::Device& device(&context, devIndex);

Once you've got your device, you can / have to :

* open a stream on it : `device.open(2, 44100)`, using the number of channels
and the sampling rate you require
* start it : `device.start()` to fire up the engine
* stop it : `device.stop()` when you're done and want to restart later
* close it : `device.close()` when you're really done and stopped before

### uss::core::Node

A `uss::core::Node` is a class (defined as structure because o shit waddup) that is linked
to a `uss::core::Context` and has a `virtual` method `void update(double sampleRate)`. It
should have multiple `uss::core::MonoSignal` or `uss::core::StereoSignal`, representing data
to pass around. The update() method is called every 1 / sampleRate seconds by the
`uss::core::Context` currently used. That's how you create an `uss::modular::Oscillator`
node for instance :

	uss::modular::Oscillator osc(&context);
	osc.frequency.value = 100.0;
	osc.shape.value = 0.0f;

The `uss::core::Device` object contains an output `uss::core::StereoSignal` for you to use as sink.
You should use a mixer or other nodes that output stereo signals : 

	uss::modular::Mixer mixer(&context);
	osc.destination.connect(&mixer.getMonoInput(0));
	mixer.left.connect(&device.output.left);
	mixer.right.connect(&device.output.right);

The `uss::modular::Mixer` node has multiple inputs and writes to "left" and "right" phases, usable
for the Device's stereo output. When you "ask" for index 0 in input, you ask the
`uss::modular::Mixer` to initialize one new input `uss::core::[Mono/Stereo]Signal` at that index.
The mixer will gently add up the inputs. And I'll make params like panning, volume
control later.

### A note on uss::core::[Mono/Stereo]Signal

As you may have seen, `uss::core::[Mono/Stereo]Signal` behave weirdly. It's not just a structure to a value, it
can be linked to an other `uss::core::[Mono/Stereo]Signal` to create chains, and make up the audio path. You should
check the `uss::modular::Gain` node for a really simple example of the mechanism (in `include/modular/gain.hpp`).

	// Gain derives from Node
	struct Gain : core::Node {
		core::MonoSignal input;			// Input signal
		core::MonoSignal gain;			// Gain factor
		core::MonoSignal offset;		// Gain offset
		core::MonoSignal destination;	// Output signal

		// Constructor
		Gain(core::Context* context) : core::Node(context) {}

		// Update method
		virtual void update(double sampleRate) override {
			// Set the value of destination to be input * gain factor + gain offset
			destination.value = input.value * gain.value + offset.value;
			
			// Forward parameters values
			input.update();
			gain.update();
			offset.update();
			destination.update();
		}
	};

Each time you make a call to `uss::core::[Mono/Stereo]Signal::update()`, the node copies its value to a list
of targets it has been connected to. That's what you do when you connect the oscillator to
a mixer for example. You route the destination of the oscillator to an input of the
mixer.

## Build

### Portaudio

Ultra Sound System uses Portaudio 19 by default. It's not bundled so you have to get it separately.

* on Linux : [apt-get install / pacman -S / yaourt -S / whatever] portaudio
* on Windows : [this git page](https://github.com/adfernandes/precompiled-portaudio-windows) has precompiled binaries
* on Mac OS X : [this page](http://macappstore.org/portaudio/) tells you how to do
* on anything supported : [the portaudio documentation](http://portaudio.com/docs/v19-doxydocs/tutorial_start.html) tells you how to build the library.

### CMake

The project can be built using CMake 3.0. Be sure you've got a cmake version as new as 3.0.
Since it's cmake and the only dependency is Portaudio, you should be able to build the
project on barely any platform.
