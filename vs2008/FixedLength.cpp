#include "FixedLength.h"

namespace IPC_WM_COPYDATA {

	namespace FixedLength {

		ePACKET_TYPE getPacketType(PCOPYDATASTRUCT pcds) {
			if (getCopyDataType(pcds) == eCOPYDATA_TYPE_UNKNOWN) {
				return ePACKET_TYPE_UNKNOWN;
			}

			if (pcds->cbData != sizeof(Data)) {
				return ePACKET_TYPE_UNKNOWN;
			}

			Data* tmp = (Data*)pcds->lpData;
			if (tmp->packetType < ePACKET_TYPE_UNKNOWN || tmp->packetType > ePACKET_TYPE_COUNT) {
				return ePACKET_TYPE_UNKNOWN;
			}

			return tmp->packetType;
		}
        
    }

}