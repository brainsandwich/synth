#pragma once

namespace uss {
	namespace modular {
		struct ClockDivider : core::Node {
			core::MonoSignal input;
			core::MonoSignal destination;

			float lastclk = 0.0f;
			uint8_t counter = 0;
			uint8_t divider = 1;

			enum class GateState {
				LOW, HIGH
			} state = GateState::LOW;

			ClockDivider(core::Context* context) : core::Node(context) {}
			virtual void update(double rate) override {
				// Rising edge
				switch (state) {
					case GateState::LOW:
						if (input.value > core::GATE_THRESHOLD) {
							state = GateState::HIGH;
							if (counter == 0)
								destination.value = destination.value == core::GATE_HIGH ? core::GATE_LOW : core::GATE_HIGH;

							counter++;
							if (counter >= divider)
								counter = 0;
						}
						break;
					case GateState::HIGH:
						if (input.value < core::GATE_THRESHOLD)
							state = GateState::LOW;
						break;
				}
				
				input.update();
				destination.update();
			}
		};
	}
}
