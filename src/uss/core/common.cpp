#include "uss/core/common.hpp"

#include <algorithm>

namespace uss {
	namespace core {

		WaveTable sinetable(2048);
		WaveTable sawtable(2048);
		WaveTable squaretable(2048);

		void init_wavetables() {
			generate_sine(sinetable);
			generate_sawtooth(sawtable);
			generate_square(squaretable);
		}

		void generate_sine(WaveTable& table) {
			for (std::size_t i = 0; i < table.size(); i++)
				table[i] = sin(((float) i * 2.0 * PI) / ((float) table.size()));
		}

		void generate_sawtooth(WaveTable& table) {
			for (std::size_t i = 0; i < table.size(); i++)
				table[i] = (i < table.size() / 2) ? 1.0f : -1.0f;
		}

		void generate_square(WaveTable& table) {
			for (std::size_t i = 0; i < table.size(); i++)
				table[i] = (i < table.size() / 2) ? 1.0f : -1.0f;
		}
		
		void MonoSignal::update() {
			for (auto& t: targets)
				t->value = value;
		}

		void MonoSignal::connect(MonoSignal* signal) {
			targets.push_back(signal);
		}

		void MonoSignal::disconnect(MonoSignal* signal) {
			auto iter = std::find(targets.begin(), targets.end(), signal);
			if (iter != targets.end())
				targets.erase(iter);
		}

		void StereoSignal::update() {
			for (auto& t: targets) {
				t->left = left;
				t->right = right;
			}
		}

		void StereoSignal::connect(StereoSignal* signal) {
			targets.push_back(signal);
		}

		void StereoSignal::disconnect(StereoSignal* signal) {
			auto iter = std::find(targets.begin(), targets.end(), signal);
			if (iter != targets.end())
				targets.erase(iter);
		}
	}
}
