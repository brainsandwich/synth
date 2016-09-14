#pragma once

#include "modular/nodes/nodes.hpp"

#include <thread>

namespace audio {
	namespace modular {
		
		class Instrument {
			friend struct Node;

		protected:
			Instrument* parent = nullptr;
			std::vector<Node*> nodes;
			std::vector<Instrument*> subinstruments;
			
		public:
			Instrument(Instrument* parent = nullptr);
			void update(double sampleRate);

			void addNode(Node* node);
			void rmvNode(Node* node);
			void addSubInstrument(Instrument* instrument);
			void rmvSubInstrument(Instrument* instrument);
		};

	}
}
