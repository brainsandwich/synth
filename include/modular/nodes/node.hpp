#pragma once

#include "common.hpp"

#include <vector>

namespace audio {
	namespace modular {
		class Instrument;
		
		struct Node {
			Instrument* context = nullptr;

			Node(Instrument* context);
			virtual ~Node();
			virtual void update(double sampleRate) {}
		};
	}
}
