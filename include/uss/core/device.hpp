#pragma once

#include "uss/core/common.hpp"

namespace uss {
	namespace core {

		class Context;
		class Device {
			friend class Context;
		protected:
			Context* context = nullptr;

		public:
			Device(Context* context, int index);

			int index;
			int channels;
			double latency;
			Format format;
			StereoSignal output;
			DeviceInfo deviceInfo;
		};

	}
}
