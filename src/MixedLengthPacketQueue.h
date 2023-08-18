#pragma once

#include "MixedLength.h"
#include <singleton.hpp>
#include "SafeQueue.hpp"
#include <memory>
using namespace std;

namespace IPC_WM_COPYDATA {

	namespace MixedLength {

		class PacketQueue :public Singleton<PacketQueue>
		{
		public:
			PacketQueue() {}

			bool add(HWND hWnd, HWND hwndFrom, PCOPYDATASTRUCT pcds) {
				if (getCopyDataType(pcds) != eCOPYDATA_TYPE_MIXED_LENGTH) {
					return false;
				}

				if (pcds->cbData != sizeof(Data)) {
					return false;
				}

				Data* tmp = (Data*)pcds->lpData;
				unsigned int index = 0;
				for (unsigned int i = 0; i < tmp->count; i++) {
					Packet* packet = getPacket(tmp->buf + index);
					PacketHandler::Item<Packet>* tmp = new PacketHandler::Item<Packet>({ hWnd, hwndFrom, unique_ptr<Packet>(packet) });
					_queue.Produce(std::unique_ptr<PacketHandler::Item<Packet>>(tmp));
					index += packet->dataLength();
				}
				return true;
			}

			void start() {
				_quit = false;

				while (!_quit) {
					/*
					std::unique_ptr<PacketHandler::Item<Packet>> packet;
					if (_queue.Consume(packet)) {
						PacketHandler::handle(std::move(packet));
					}
					//*/
					PacketHandler::handle(_queue);
				}
			}

			void stop() {
				_quit = true;
			}

			long long queueSize() {
				return _queue.Size();
			}

		protected:
			bool _quit;
			SafeQueue<std::unique_ptr<PacketHandler::Item<Packet>>> _queue;
		};

	}


}
