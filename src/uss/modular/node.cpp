#include "uss/core/node.hpp"
#include "uss/core/context.hpp"

#include <algorithm>
#include <iostream>

namespace uss {
	namespace core {
		Node::Node(Context* context)
			: context(context)
		{
			context->addNode(this);
		}

		Node::~Node() {
			context->rmvNode(this);
		}
	}
}
