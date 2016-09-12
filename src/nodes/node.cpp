#include "nodes/node.hpp"
#include "context.hpp"

#include <algorithm>
#include <iostream>

namespace audio {

	void Signal::update() {
		for (auto& t: targets)
			t->value = value;
	}

	void Signal::connect(Signal* signal) {
		targets.push_back(signal);
	}

	void Signal::disconnect(Signal* signal) {
		auto iter = std::find(targets.begin(), targets.end(), signal);
		if (iter != targets.end())
			targets.erase(iter);
	}

	Node::Node(Context* context)
		: context(context)
	{
		context->addNode(this);
	}

	Node::~Node() {
		context->rmvNode(this);
	}
	
}
