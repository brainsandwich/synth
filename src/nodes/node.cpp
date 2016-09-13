#include "nodes/node.hpp"
#include "context.hpp"

#include <algorithm>
#include <iostream>

namespace audio {

	Node::Node(Context* context)
		: context(context)
	{
		context->addNode(this);
	}

	Node::~Node() {
		context->rmvNode(this);
	}
	
}
