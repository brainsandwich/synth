#pragma once

#include "uss/core/node.hpp"

namespace uss {
	namespace modular {
		struct Saturator : public core::Node {
			core::MonoSignal input;
			core::MonoSignal hardness;
			core::MonoSignal destination;

			Saturator(core::Context* context) : core::Node(context) {}

			virtual void update(double sampleRate) override {
				float x = input.value;
				destination.value = std::copysign(std::pow(std::atan(std::pow(std::abs(x), hardness.value)), (1.0 / hardness.value)), x);

				input.update();
				hardness.update();
				destination.update();
			}
		};
	}
}
