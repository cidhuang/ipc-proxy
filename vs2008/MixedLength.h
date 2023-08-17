#pragma once

#include "IPC_WM_COPYDATA.h"
#include <queue>

namespace IPC_WM_COPYDATA {

	namespace MixedLength {

		typedef int ePACKET_TYPE;
		const ePACKET_TYPE ePACKET_TYPE_UNKNOWN = -1;
		extern const ePACKET_TYPE ePACKET_TYPE_COUNT;

		class Packet {
		public:
			Packet();
			~Packet();

			ePACKET_TYPE type();

			virtual unsigned int length() = 0;
			virtual const char* content() = 0;
			virtual bool load(const char* buf) = 0;

		protected:
			ePACKET_TYPE _type;
		};

		const unsigned int BufferLength = 1024 * 256;
		struct Data {
			eCOPYDATA_TYPE dataType;	// = eCOPYDATA_TYPE_MIXED_LENGTH_PACKETS; cannot init in vs2008
			unsigned int count;
			char buf[BufferLength];
		};

		extern ePACKET_TYPE getPacketType(char* buf);

		extern Packet* getPacket(char* buf);

		class Sender {
		public:
			Sender(HWND to, HWND from)
				: _to(to)
				, _from(from)
				, _index(0)
			{
				_data.dataType = eCOPYDATA_TYPE_MIXED_LENGTH;
				memset(_data.buf, 0, BufferLength);
			}
			~Sender() {}

			bool add(Packet& packet) {
				if ((_index + packet.length()) > BufferLength) {
					return false;
				}
				memcpy(_data.buf + _index, packet.content(), packet.length());
				_index += packet.length();
				return true;
			}

			LRESULT send() {
				COPYDATASTRUCT cds;
				cds.cbData = sizeof(_data);
				cds.lpData = &_data;

				LRESULT result = SendMessage(_to, WM_COPYDATA, reinterpret_cast<WPARAM>(_from),
					reinterpret_cast<LPARAM>(&cds));

				_index = 0;
				_data.count = 0;

				return result;
			}

		protected:
			Data _data;
			unsigned int _index;

			HWND _to;
			HWND _from;
		};

		extern std::queue<Packet> parse(PCOPYDATASTRUCT pcds);

	}

}