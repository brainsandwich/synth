#include "uss/core/device.hpp"
#include "uss/core/context.hpp"

namespace uss {
	namespace core {
		Device::Device(Context* context, int index)
			: index(index)
			, context(context)
		{
			deviceInfo = context->getDeviceInfo(index);
		}
	}
}
