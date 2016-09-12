# Synth

"Synth" is a modular synthesizer using PortAudio as backend, inspired
by the WebAudio API.

I made it because I was looking for a modular synthesis library for modern C++
and found nothing (eh, there was something, but too high level, or written
in ol C or C++). This lib will not be revolutionary, but at least it's open source
and the model looks quite friendly.

## How does that work ?

The "sketch" test in the test folder is subject to change, but the basics of
firing up a context and throwing audio waves at your speakers are there. All
the definitions are found in the `audio::` namespace.

### audio::Context

First, you need a `audio::Context`. From that class you can fetch information about
audio devices or apis used. It also contains pointers to `audio::Node`s and `audio::Device`s in
use, but we'll check the `audio::Node`s after.

### audio::Device

Now you want an output `audio::Device` (for now it can only be output tho). The ideal
is to find out what is the default device to use with your api of choice. In
`sketch.cpp` I'm using the default device of the first `audio::HostApi` listed by the
`audio::Context`. Just do it like this :

	// Get the list of host apis
	std::vector<audio::HostApi> hostApis;
	context.listHostApis(std::back_inserter(hostApis));

	// Get default output device index (integer)
	int devIndex = hostApis[0].defaultOutputDeviceIndex;
	std::cout << "Opening output device " << devIndex << std::endl;
	audio::Device& device(&context, devIndex);

Once you've got your device, you can / have to :

* open a stream on it : `device.open(2, 44100)`, using the number of channels
and the sampling rate you require
* start it : `device.start()` to fire up the engine
* stop it : `device.stop()` when you're done and want to restart later
* close it : `device.close()` when you're really done and stopped before

### audio::Node

A `audio::Node` is a class (defined as structure because o shit waddup) that is linked
to a `audio::Context` and has a `virtual` method `void update(double sampleRate)`. It
should have multiple `audio::Signal`, representing data to pass around. The update()
method is called every 1 / sampleRate seconds by the `audio::Device` currently used (which is
okay for now but I sense trouble with multiple devices :/). That's how you create an
`audio::Oscillator` node for instance :

	audio::Oscillator osc(&context);
	osc.frequency.value = 100.0;
	osc.shape.value = 0.0f;

The `audio::Device` object contains an output `audio::Node` for you to use as sink. The
output is an `audio::Mixer` node :

	osc.destination.connect(&device.output[0]);

The `audio::Mixer` node has multiple inputs and writes to "left" and "right" phases, usable
for the Device's stereo output (so yes, you can choose channels but there's a fixed amount
of 2 that will be processed, so even mono won't work correctly, yey).
When you "ask" for index 0 in output, you ask the `audio::Mixer` to initialize one new input
`audio::Signal` at that index. The mixer will gently add up the inputs. And I'll make params
like panning, volume control later. It just mixes signals altogether for now.

### A note on audio::Signal

As you may have seen, `audio::Signal` behave weirdly. It's not just a structure to a value, it
can be linked to an other `audio::Signal` to create chains, and make up the audio path. You should
check the `audio::Gain` node for a really simple example of the mechanism (in `include/nodes/gain.hpp`).

	// Gain derives from Node
	struct Gain : Node {
		Signal input;			// Input signal
		Signal gain;			// Gain factor
		Signal destination;		// Output signal

		// Constructor
		Gain(Context* context) : Node(context) {}

		// Update method
		virtual void update(double sampleRate) override {
			// Set the value of destination to be input * gain factor
			destination.value = input.value * gain.value;

			// Update the destination value
			destination.update();
		}
	};

Each time you make a call to `audio::Signal::update()`, the node copies its value to a list
of targets it has been connected to. That's what you do when you connect the oscillator to
the output mixer for example. You route the destination of the oscillator to an input of the
mixer.

## Build

You need PortAudio installed to build this library. I should use a dependency system with cmake
in order to incorporate source and let you build it, but you'll have to wait for that. For now,
just install it :

* on Linux : [apt-get install / pacman -S / yaourt -S / whatever] portaudio
* on Windows : [this git page](https://github.com/adfernandes/precompiled-portaudio-windows) has precompiled binaries
* on Mac OS X : [this page](http://macappstore.org/portaudio/) tells you how to do
* on anything supported : [the portaudio documentation](http://portaudio.com/docs/v19-doxydocs/tutorial_start.html) tells you how to build the library.

And then:

	mkdir build
	cd build
	cmake .. && make

If you want to try the demo :

	cd build
	test/sketch/sketch
