#pragma once

namespace uss {
	namespace modular {
		
		/**
		 *	The Sequencer generates a gate output and a note level
		 *	according to a array of programmed notes, a clock to set the
		 *	tempo, a base note, and the length of the note.
		 *	On rising edge, the gate signal value is 1.0f and the current
		 *	note is updated according to the sequence.
		 *	After "noteLength" seconds have passed, the gate signal is 
		 *	reset to 0.0f
		 */
		struct Sequencer : core::Node {
			using note_pair = std::pair<std::shared_ptr<core::MonoSignal>, bool>;
			std::vector<note_pair> notes;
			core::MonoSignal clock;
			core::MonoSignal baseNote;
			core::MonoSignal noteLength;

			core::MonoSignal currentNote;
			core::MonoSignal gate;

			double gate_time = 0.0;
			float lastclk = 0.0f;
			std::size_t note_offset = 0;

			enum class GateState {
				LOW, HIGH
			} state = GateState::LOW;

			Sequencer(core::Context* context)
				: core::Node(context)
			{
				gate.value = core::GATE_LOW;
			}

			virtual void update(double rate) override {

				// Rising edge
				switch (state) {
					case GateState::LOW: {
						if (clock.value > core::GATE_THRESHOLD) {
							state = GateState::HIGH;
							gate_time = 0.0;
							gate.value = notes[note_offset].second ? core::GATE_HIGH : core::GATE_LOW;

							if (!notes.empty() && notes[note_offset].second) 
								currentNote.value = baseNote.value + notes[note_offset].first->value;
							
							note_offset++;
							if (note_offset >= notes.size())
								note_offset = 0;
						}
						break;
					}
					case GateState::HIGH: {
						if (clock.value < core::GATE_THRESHOLD)
							state = GateState::LOW;
						break;
					}
				}

				// Note length
				if (gate.value > core::GATE_THRESHOLD) {
					gate_time += 1.0 / rate;
					if (gate_time > noteLength.value)
						gate.value = core::GATE_LOW;
				}

				// Signals updates
				for (auto& n: notes)
					n.first->update();

				clock.update();
				baseNote.update();
				noteLength.update();

				currentNote.update();
				gate.update();
			}

			Sequencer& addNote(float value) {
				notes.push_back(note_pair(std::make_shared<core::MonoSignal>(value), value != 0.0f));
				return *this;
			}

			Sequencer& setNote(std::size_t index, float value) {
				if (index >= notes.size())
					notes.resize(index + 1, note_pair(std::make_shared<core::MonoSignal>(0.0f), false));
				notes[index].first->value = value;
				return *this;
			}
			
			float getNote(std::size_t index) {
				if (index >= notes.size())
					notes.resize(index + 1, note_pair(std::make_shared<core::MonoSignal>(0.0f), false));
				return notes[index].first->value;
			}
			
			Sequencer& setNoteCount(std::size_t count) {
				notes.resize(count, note_pair(std::make_shared<core::MonoSignal>(0.0f), false));
				return *this;
			}
		};

	}
}
