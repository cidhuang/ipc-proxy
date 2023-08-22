#pragma once

#include "IPC_WM_COPYDATA.h"
#include "PacketType.h"
#include <queue>

namespace IPC_WM_COPYDATA {

	namespace FixedLength {

		const unsigned int BufferLength = 1024 * 256;
		struct Data {
			eCOPYDATA_TYPE dataType;	// = eCOPYDATA_TYPE_FIXED_LENGTH; cannot init in vs2008
			ePACKET_TYPE packetType;	// = eLOG_TYPE_UNKNOWN; cannot init in vs2008
			unsigned int packetLength;
			unsigned int count;
			char buf[BufferLength];
		};

		extern ePACKET_TYPE getPacketType(PCOPYDATASTRUCT pcds);

		template<typename T>
		ePACKET_TYPE getPacketType();

		template<typename T>
		extern unsigned int getPacketLength() {
			T packet;
			return sizeof(packet);
		}

		template<typename T>
		class PacketSender {
		public:
			PacketSender(HWND to, HWND from)
				: _to(to)
				, _from(from)
			{
				_data.dataType = eCOPYDATA_TYPE_FIXED_LENGTH;
				_data.packetType = getPacketType<T>();
				_data.packetLength = getPacketLength<T>();
				_data.count = 0;
				memset(_data.buf, 0, BufferLength);
			}
			~PacketSender() {}

			bool add(T& packet) {
				if ((_index() + _data.packetLength) > BufferLength) {
					return false;
				}
				memcpy(_data.buf + _index(), &packet, _data.packetLength);
				_data.count++;
				return true;
			}

			LRESULT send() {
				COPYDATASTRUCT cds;
				cds.cbData = sizeof(_data);
				cds.lpData = &_data;

				LRESULT result = SendMessage(_to, WM_COPYDATA, reinterpret_cast<WPARAM>(_from),
					reinterpret_cast<LPARAM>(&cds));

				_data.count = 0;

				return result;
			}

			ePACKET_TYPE packetType() {
				return _data.packetType;
			}

			unsigned int packetLength() {
				return _data.packetLength;
			}

			unsigned int count() {
				return _data.count;
			}

		protected:
			Data _data;
			unsigned int _index() {
				return _data.count * _data.packetLength;
			}

			HWND _to;
			HWND _from;
		};

		template<typename T>
		std::queue<T> parse(PCOPYDATASTRUCT pcds) {
			std::queue<T> queue;
			ePACKET_TYPE packetType = getPacketType(pcds);
			if (packetType < 0) {
				return false;
			}

			Data* data = (Data*)pcds->lpData;
			for (unsigned int i = 0; i < data->count; i++) {
				T* tmp(new T(*(T*)(data->buf + i * data->packetLength)));
				queue.push(*tmp);
			}

		}

	}

}