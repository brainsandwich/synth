#pragma once

#include <cmath>

namespace audio {
	
	struct Oscillator : Node {
		Signal frequency;
		Signal modulation;
		Signal shape;
		Signal destination;

		double offset = 0.0;

		Oscillator(Context* context) : Node(context) {}

		virtual void update(double sampleRate) override {
			float base_freq = frequency.value > MIN_FREQ ? frequency.value : MIN_FREQ;
			float mod_freq = modulation.value > MIN_FREQ ? modulation.value : MIN_FREQ;
			float freq = (base_freq + mod_freq) > MIN_FREQ ? (base_freq + mod_freq) : MIN_FREQ;

			double inc = SineTable::SIZE * freq / sampleRate;
			offset = offset >= SineTable::SIZE ? offset - SineTable::SIZE : offset;

			std::size_t offset_floor = floor(offset);
			std::size_t offset_ceil = offset_floor >= (SineTable::SIZE - 1) ? 0 : offset_floor + 1;
			double phasor = offset - offset_floor;
			offset += inc;

			float sine = phasor * SineTable::wave[offset_floor] + (1.0 - phasor) * SineTable::wave[offset_ceil];
			float square = phasor * SquareTable::wave[offset_floor] + (1.0 - phasor) * SquareTable::wave[offset_ceil];
			float saw = phasor * SawTable::wave[offset_floor] + (1.0 - phasor) * SawTable::wave[offset_ceil];
			if (shape.value > (1.0 / 2.0))
				destination.value = (1.0 - (2.0 * shape.value - 1.0)) * saw + (2.0 * shape.value - 1.0) * square;
			else
				destination.value = (1.0 - (2.0 * shape.value)) * sine + (2.0 * shape.value) * saw;

			frequency.update();
			modulation.update();
			shape.update();
			destination.update();
		}
	};

}
