#pragma once

#include "modular/nodes/node.hpp"

namespace audio {
	namespace modular {
		struct Filter : Node {
			MonoSignal input;
			MonoSignal quality;
			MonoSignal cutoff;
			MonoSignal destination;
			Filter(Instrument* context) : Node(context) {}
			virtual void update(double sampleRate) = 0;
		};

		struct LowPassFilter : Filter {
			float buffer[3] = { 0.0f, 0.0f, 0.0f };
			struct {
				float quality, cutoff;
				float a0 = 0.0f, a1 = 0.0f, a2 = 0.0f, b1 = 0.0f, b2 = 0.0f;
			} cache;

			LowPassFilter(Instrument* context)
				: Filter(context) {}
			virtual void update(double sampleRate) override {
				if (cache.quality != quality.value || cache.cutoff != cutoff.value) {
					cache.quality = quality.value;
					cache.cutoff = cutoff.value;

					float freq = cutoff.value > MIN_FREQ ? cutoff.value : MIN_FREQ;

		    		float K = tan(PI * freq / (float) sampleRate);
		            float norm = 1.0f / (1.0f + K / quality.value + K * K);
		            cache.a0 = K * K * norm;
		            cache.a1 = 2.0f * cache.a0;
		            cache.a2 = cache.a0;
		            cache.b1 = 2.0f * (K * K - 1.0f) * norm;
		            cache.b2 = (1.0f - K / quality.value + K * K) * norm;
		        }

	            destination.value = cache.a0 * buffer[0] + cache.a1 * buffer[1] + cache.a2 * buffer[2];
	            buffer[2] = buffer[1];
	            buffer[1] = buffer[0];
	            buffer[0] = input.value - cache.b1 * buffer[1] - cache.b2 * buffer[2];

	            destination.update();
	            input.update();
				quality.update();
				cutoff.update();
			}
		};
	}
}
