#pragma once

#include "modular/nodes/nodes.hpp"

#include <iostream>
#include <cmath>

namespace audio {
	namespace modular {
	
		struct Oscillator : Node {
			MonoSignal frequency;
			MonoSignal modulation;
			MonoSignal destination;

			WaveTable* wavetable = &sinetable;
			bool unipolar = false;
			double offset = 0.0;

			Oscillator(Instrument* context) : Node(context) {}

			virtual void update(double sampleRate) override {
				if (!wavetable) {
					frequency.update();
					modulation.update();
					destination.update();
					return;
				}

				auto& table = *wavetable;

				float base_freq = frequency.value > MIN_FREQ ? frequency.value : MIN_FREQ;
				float mod_freq = modulation.value > MIN_FREQ ? modulation.value : MIN_FREQ;
				float freq = (base_freq + base_freq * mod_freq) > MIN_FREQ ? (base_freq + base_freq * mod_freq) : MIN_FREQ;

				double inc = table.size() * freq / sampleRate;
				offset = offset >= table.size() ? offset - table.size() : offset;

				std::size_t offset_floor = floor(offset);
				std::size_t offset_ceil = offset_floor >= (table.size() - 1) ? 0 : offset_floor + 1;
				double phasor = offset - offset_floor;
				offset += inc;

				destination.value = phasor * table[offset_floor] + (1.0f - phasor) * table[offset_ceil];
				if (unipolar)
					destination.value = destination.value / 2.0f + 0.5f;

				frequency.update();
				modulation.update();
				destination.update();
			}
		};

	}
}
