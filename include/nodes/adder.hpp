#pragma once

#include "nodes/node.hpp"

namespace audio {
	
	struct Adder : Node {
		std::vector<std::shared_ptr<Signal>> inputs;
		Signal destination;
		virtual void update(double sampleRate) override {
			destination.value = 0.0f;
			for (auto& sig: inputs)
				destination.value += sig->value;
			destination.update();
		}

		Adder(Context* context) : Node(context) {}
		Signal& operator[](std::size_t index) {
			if (index >= inputs.size())
				inputs.resize(index + 1, std::make_unique<Signal>());
			
			return *inputs[index].get();
		}
	};

}
