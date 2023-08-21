#pragma once

#include "FixedLength.h"
#include "MixedLength.h"

#include <string>

namespace IPC_WM_COPYDATA {

	namespace FixedLength {

		struct Position {
			int x;
			int y;
			long long time;
		};

	}

	namespace MixedLength {

		class PacketQuery : public Packet {
		public:
			PacketQuery();
			~PacketQuery();

			unsigned int from;
			std::string query;

		protected:
			// load from buffer
			bool _parsePayload(const char* payloadSrc, unsigned int payloadLength);	// no header included

			// output to buffer
			void _setPayload(char* dst);	// no header included
			unsigned int _payloadLength();	// no header included

		};

	}

}
