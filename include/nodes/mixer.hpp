#pragma once

#include "nodes/node.hpp"

#include <iostream>

namespace audio {
	
	struct Mixer : Node {
		std::vector<std::shared_ptr<Signal>> inputs;
		Signal left;
		Signal right;

		Mixer(Context* context) : Node(context) {}
		virtual void update(double sampleRate) override {
			left.value = 0.0f;
			right.value = 0.0f;
			for (auto& sig: inputs) {
				float value = sig->value;
				left.value += value / (float) inputs.size();
				right.value += value / (float) inputs.size();
			}
			left.value = left.value;
			right.value = right.value;
		}

		Signal& operator[](std::size_t index) {
			if (index >= inputs.size())
				inputs.resize(index + 1, std::make_unique<Signal>());
			
			return *inputs[index].get();
		}
	};

}
