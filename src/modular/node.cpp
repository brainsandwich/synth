#include "modular/nodes/node.hpp"
#include "modular/instrument.hpp"

#include <algorithm>
#include <iostream>

namespace audio {
	namespace modular {
		Node::Node(Instrument* context)
			: context(context)
		{
			context->addNode(this);
		}

		Node::~Node() {
			context->rmvNode(this);
		}
	}
}
