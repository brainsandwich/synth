#pragma once

#include "nodes/node.hpp"

namespace audio {

	struct Gain : Node {
		MonoSignal input;
		MonoSignal gain;
		MonoSignal offset;
		MonoSignal destination;

		Gain(Context* context) : Node(context) {}
		virtual void update(double sampleRate) override {
			destination.value = input.value * gain.value + offset.value;
			
			input.update();
			gain.update();
			offset.update();
			destination.update();
		}
	};
	
}
