#pragma once

#include "modular/nodes/node.hpp"

namespace audio {
	namespace modular {
		
		struct Enveloppe : Node {
			enum class State {
				Idle,
				Attack,
				Decay,
				Sustain,
				Release
			} state = State::Idle;

			// Common
			MonoSignal gate;
			MonoSignal input;
			MonoSignal enveloppe;
			MonoSignal destination;

			// Time
			MonoSignal attack;
			MonoSignal decay;
			MonoSignal release;

			// Levels
			MonoSignal attackLevel;
			MonoSignal sustainLevel;

			bool exponential = true;

			Enveloppe(Instrument* context) : Node(context) {}
			virtual void update(double sampleRate) override {
				switch (state) {
					case State::Idle:
					{
						enveloppe.value = 0.0f;
						if (gate.value >= 0.9f)
							state = State::Attack;
						break;
					}
					case State::Attack:
					{
						if (gate.value <= 0.1f) {
							state = State::Release;
							break;
						}
						// enveloppe.value = enveloppe.value + attack.value;
						enveloppe.value = modulate(enveloppe.value, attackLevel.value, attack.value, sampleRate);
						if (1.01f * enveloppe.value > attackLevel.value)
							state = State::Decay;
						break;
					}
					case State::Decay:
					{
						if (gate.value <= 0.1f) {
							state = State::Release;
							break;
						}
						// enveloppe.value = enveloppe.value - decay.value;
						enveloppe.value = modulate(enveloppe.value, sustainLevel.value, decay.value, sampleRate);
						if (0.99f * enveloppe.value < sustainLevel.value)
							state = State::Sustain;
						break;
					}
					case State::Sustain:
						if (gate.value <= 0.1f) {
							state = State::Release;
							break;
						}
						break;
					case State::Release:
						if (gate.value >= 0.9f)
							state = State::Attack;

						// enveloppe.value = enveloppe.value - release.value;
						enveloppe.value = modulate(enveloppe.value, 0.0f, release.value, sampleRate);
						if (enveloppe.value < 0.0001)
							state = State::Idle;
						break;
				}

				enveloppe.value = audio::clamp(enveloppe.value, 0.0f, 1.0f);
				destination.value = enveloppe.value * input.value;

				enveloppe.update();
				gate.update();
				input.update();
				destination.update();
			}

			float modulate(float current, float target, float timevalue, double sampleRate) {
				float alpha = 1.0 - exp(-(1.0 / sampleRate) / (std::max(timevalue, 0.0001f) / 5.0f));
				return current * (1.0 - alpha) + target * alpha;
				// return 1.0 + (log(std::max(target, 0.001f)) - log(std::max(current, 0.001f))) / (timevalue / sampleRate);
			}
		};

	}
}
