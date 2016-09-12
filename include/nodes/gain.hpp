#pragma once

#include "nodes/node.hpp"

namespace audio {

	struct Gain : Node {
		Signal input;
		Signal gain;
		Signal offset;
		Signal destination;

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
