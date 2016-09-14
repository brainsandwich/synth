#pragma once

#include "modular/nodes/node.hpp"

namespace audio {
	namespace modular {
		struct Gain : Node {
			MonoSignal input;
			MonoSignal gain;
			MonoSignal offset;
			MonoSignal destination;

			Gain(Instrument* context) : Node(context) {}
			virtual void update(double sampleRate) override {
				destination.value = input.value * gain.value + offset.value;
				
				input.update();
				gain.update();
				offset.update();
				destination.update();
			}
		};
	}	
}
