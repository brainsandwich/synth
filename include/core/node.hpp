#pragma once

#include "core/common.hpp"

#include <vector>

namespace uss {
	namespace core {
		class Context;
		struct Node {
			Context* context = nullptr;

			Node(Context* context);
			virtual ~Node();
			virtual void update(double sampleRate) {}
		};
	}
}
