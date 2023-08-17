#include "MixedLength.h"

namespace IPC_WM_COPYDATA {

	namespace MixedLength {

		ePACKET_TYPE Packet::type() {
			return _type;
		}

		ePACKET_TYPE getPacketType(char* buf) {
			ePACKET_TYPE* tmp = (ePACKET_TYPE*)buf;
			if (*tmp < ePACKET_TYPE_UNKNOWN || *tmp > ePACKET_TYPE_COUNT) {
				return ePACKET_TYPE_UNKNOWN;
			}

			return *tmp;
		}
        
    }

}