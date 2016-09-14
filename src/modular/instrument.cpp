#include "modular/instrument.hpp"

#include <algorithm>

namespace audio {
	namespace modular {
		void Instrument::update(double sampleRate) {
			for (auto& node: nodes)
				node->update(sampleRate);
			for (auto& instrument: subinstruments)
				instrument->update(sampleRate);
		}

		Instrument::Instrument(Instrument* parent)
			: parent(parent) {}

		void Instrument::addNode(Node* node) {
			nodes.push_back(node);
		}

		void Instrument::rmvNode(Node* node) {
			auto iter = std::find(nodes.begin(), nodes.end(), node);
			if (iter != nodes.end())
				nodes.erase(iter);
		}

		void Instrument::addSubInstrument(Instrument* instrument) {
			subinstruments.push_back(instrument);
		}

		void Instrument::rmvSubInstrument(Instrument* instrument) {
			auto iter = std::find(subinstruments.begin(), subinstruments.end(), instrument);
			if (iter != subinstruments.end())
				subinstruments.erase(iter);
		}

	}
}
