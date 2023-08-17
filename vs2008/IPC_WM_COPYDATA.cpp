#include "stdafx.h"
#include "IPC_WM_COPYDATA.h"

namespace IPC_WM_COPYDATA {

	eCOPYDATA_TYPE getCopyDataType(PCOPYDATASTRUCT pcds) {
		eCOPYDATA_TYPE* tmp = (eCOPYDATA_TYPE*)pcds->lpData;
		if (*tmp < eCOPYDATA_TYPE_UNKNOWN || *tmp > eCOPYDATA_TYPE_COUNT) {
			return eCOPYDATA_TYPE_UNKNOWN;
		}
		return *tmp;
	}

};

