#pragma once

#include <windows.h>

namespace IPC_WM_COPYDATA {

	enum eCOPYDATA_TYPE {
		eCOPYDATA_TYPE_UNKNOWN = -1,
		eCOPYDATA_TYPE_FIXED_LENGTH,
		eCOPYDATA_TYPE_MIXED_LENGTH,
		eCOPYDATA_TYPE_COUNT,
	};

	extern eCOPYDATA_TYPE getCopyDataType(PCOPYDATASTRUCT pcds);

};

