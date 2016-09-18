#pragma once

namespace uss {
	namespace modular {
		
		struct Clock : core::Node {
			core::MonoSignal frequency;
			core::MonoSignal destination;

			bool high = false;
			float phase = 0.0f;

			Clock(core::Context* context) : core::Node(context) {}
			virtual void update(double sampleRate) override {
				phase += 1.0 / sampleRate;
				if (phase > (1.0 / frequency.value) / 2.0) {
					high = !high;
					phase -= (1.0 / frequency.value) / 2.0;
				}

				destination.value = high ? core::GATE_HIGH : core::GATE_LOW;

				destination.update();
				frequency.update();
			}
		};

	}
}
