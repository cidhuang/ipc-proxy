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
			Packet()
				: _type(ePACKET_TYPE_UNKNOWN)
			{}
			~Packet() {}

			ePACKET_TYPE type() {
				return _type;
			}

			bool isOk() {
				return (_type != ePACKET_TYPE_UNKNOWN);
			}

			bool parse(const char* src) {
				ePACKET_TYPE* tmp = (ePACKET_TYPE*)src;
				if (*tmp > ePACKET_TYPE_COUNT || *tmp < ePACKET_TYPE_UNKNOWN) {
					return false;
				}

				unsigned int* payloadLength = (unsigned int*)(src + sizeof(ePACKET_TYPE));

				if (!_parsePayload(src + sizeof(ePACKET_TYPE) + sizeof(unsigned int), *payloadLength)) {
					return false;
				}

				_type = *tmp;
				return true;
			}

			void setData(char* buffer) {	// header included
				ePACKET_TYPE* tmp = (ePACKET_TYPE*)buffer;
				*tmp = _type;
				unsigned int* length = (unsigned int*)(buffer + sizeof(ePACKET_TYPE));
				*length = _payloadLength();
				_setPayload(buffer + sizeof(ePACKET_TYPE) + sizeof(unsigned int));
			}

			unsigned int dataLength() {
				return sizeof(ePACKET_TYPE) + sizeof(unsigned int) + _payloadLength();
			}

		protected:
			ePACKET_TYPE _type;

			// load from buffer
			virtual bool _parsePayload(const char* payloadSrc, unsigned int payloadLength) = 0;	// no header included

			// output to buffer
			virtual void _setPayload(char* dst) = 0;			// no header included
			virtual unsigned int _payloadLength() = 0;	// no header included

		};

		const unsigned int BufferLength = 1024 * 256;
		struct Data {
			eCOPYDATA_TYPE dataType;	// = eCOPYDATA_TYPE_MIXED_LENGTH_PACKETS; cannot init in vs2008
			unsigned int count;
			char buf[BufferLength];
		};

		extern ePACKET_TYPE getPacketType(char* buf);

		extern Packet* getPacket(char* buf);

		class PacketSender {
		public:
			PacketSender(HWND to, HWND from)
				: _to(to)
				, _from(from)
				, _index(0)
			{
				_data.dataType = eCOPYDATA_TYPE_MIXED_LENGTH;
				_data.count = 0;
				memset(_data.buf, 0, BufferLength);
			}
			~PacketSender() {}

			bool add(Packet& packet) {
				if ((_index + packet.dataLength()) > BufferLength) {
					return false;
				}
				packet.setData(_data.buf + _index);
				_index += packet.dataLength();
				_data.count++;
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

		extern std::queue<Packet*> parse(PCOPYDATASTRUCT pcds);

	}

}