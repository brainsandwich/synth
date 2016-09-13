#pragma once

#include "nodes/node.hpp"

#include <iostream>

namespace audio {
	
	struct Mixer : Node {
		std::vector<std::shared_ptr<MonoSignal>> monoinputs;
		std::vector<std::shared_ptr<StereoSignal>> stereoinputs;
		StereoSignal destination;

		Mixer(Context* context) : Node(context) {}
		virtual void update(double sampleRate) override {
			destination.left = 0.0f;
			destination.right = 0.0f;

			for (auto& sig: monoinputs) {
				float value = sig->value;
				destination.left += value;
				destination.right += value;
			}

			for (auto& sig: stereoinputs) {
				destination.left += sig->left;
				destination.right += sig->right;
			}

			destination.update();
		}

		MonoSignal& getMonoInput(std::size_t index) {
			if (index >= monoinputs.size())
				monoinputs.resize(index + 1, std::make_shared<MonoSignal>());

			std::cout << index << " " << monoinputs.size() << std::endl;
			return *(monoinputs[index].get());
		}

		StereoSignal& getStereoInput(std::size_t index) {
			if (index >= stereoinputs.size())
				stereoinputs.resize(index + 1, std::make_shared<StereoSignal>());

			return *(stereoinputs[index].get());
		}
	};

}
