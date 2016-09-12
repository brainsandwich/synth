#pragma once

#include "nodes/node.hpp"

namespace audio {

	struct Gain : Node {
		Signal input;
		Signal gain;
		Signal destination;

		Gain(Context* context) : Node(context) {}
		virtual void update(double sampleRate) override {
			gain.update();
			destination.value = input.value * gain.value;
			destination.update();
		}
	};
	
}
