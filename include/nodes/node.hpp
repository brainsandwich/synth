#pragma once

#include "common.hpp"

#include <vector>

namespace audio {

	class Context;
	
	struct Node {
		Context* context = nullptr;

		Node(Context* context);
		virtual ~Node();
		virtual void update(double sampleRate) {}
	};

}
