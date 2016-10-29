#pragma once

#include "uss/core/node.hpp"

namespace uss {
	namespace modular {
		struct Gain : core::Node {
			core::MonoSignal input;
			core::MonoSignal gain;
			core::MonoSignal offset;
			core::MonoSignal destination;

			Gain(core::Context* context) : core::Node(context) {}
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
