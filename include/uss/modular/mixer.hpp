#pragma once

#include "uss/core/node.hpp"

#include <iostream>

namespace uss {
	namespace modular {
		struct Mixer : core::Node {
			std::vector<std::shared_ptr<core::MonoSignal>> monoinputs;
			std::vector<std::shared_ptr<core::StereoSignal>> stereoinputs;
			core::StereoSignal destination;

			Mixer(core::Context* context) : core::Node(context) {}
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

			core::MonoSignal& getMonoInput(std::size_t index) {
				if (index >= monoinputs.size())
					monoinputs.resize(index + 1, std::make_shared<core::MonoSignal>());

				return *(monoinputs[index].get());
			}

			core::StereoSignal& getStereoInput(std::size_t index) {
				if (index >= stereoinputs.size())
					stereoinputs.resize(index + 1, std::make_shared<core::StereoSignal>());

				return *(stereoinputs[index].get());
			}
		};
	}
}
