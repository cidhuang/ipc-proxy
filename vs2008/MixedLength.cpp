#include "MixedLength.h"

namespace IPC_WM_COPYDATA {

	namespace MixedLength {

		ePACKET_TYPE getPacketType(char* buf) {
			ePACKET_TYPE* tmp = (ePACKET_TYPE*)buf;
			if (*tmp < ePACKET_TYPE_UNKNOWN || *tmp > ePACKET_TYPE_COUNT) {
				return ePACKET_TYPE_UNKNOWN;
			}

			return *tmp;
		}

		std::queue<Packet*> parse(PCOPYDATASTRUCT pcds) {
			std::queue<Packet*> queue;
			if (getCopyDataType(pcds) != eCOPYDATA_TYPE_MIXED_LENGTH) {
				return queue;
			}

			if (pcds->cbData != sizeof(Data)) {
				return queue;
			}

			Data* tmp = (Data*)pcds->lpData;
			unsigned int index = 0;
			for (unsigned int i = 0; i < tmp->count; i++) {
				Packet* packet = getPacket(tmp->buf + index);
				queue.push(packet);
				index += packet->dataLength();
			}
			return queue;
		}

    }

}