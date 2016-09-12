#pragma once

#include "common.hpp"

#include <vector>

namespace audio {

	class Context;

	struct Signal {
		std::vector<Signal*> targets;

		float value = 0.0f;
		void update();
		void connect(Signal* signal);
		void disconnect(Signal* signal);
	};

	struct Node {
		Context* context = nullptr;

		Node(Context* context);
		virtual ~Node();
		virtual void update(double sampleRate) {}
	};

}
