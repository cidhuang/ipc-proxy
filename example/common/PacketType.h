#pragma once

namespace IPC_WM_COPYDATA {

	namespace FixedLength {

		enum ePACKET_TYPE {
			ePACKET_TYPE_UNKNOWN = -1,
			ePACKET_TYPE_Position,
			ePACKET_TYPE_COUNT,
		};

	}

	namespace MixedLength {

		enum ePACKET_TYPE {
			ePACKET_TYPE_UNKNOWN = -1,
			ePACKET_TYPE_Query,
			ePACKET_TYPE_COUNT,
		};

	}

}