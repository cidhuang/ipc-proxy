
#pragma once

#include "FixedLength.h"
#include "PacketHandler.h"
#include <singleton.hpp>
#include "SafeQueue.hpp"
#include <memory>
using namespace std;

namespace IPC_WM_COPYDATA {

	namespace FixedLength {

		template<typename T>
		class PacketQueue :public Singleton<PacketQueue<T>>
		{
		public:
			PacketQueue<T>()
				: _packetType(getPacketType<T>())
			{}

			bool add(HWND hWnd, HWND hwndFrom, PCOPYDATASTRUCT pcds) {
				ePACKET_TYPE packetType = getPacketType(pcds);
				if (packetType < 0) {
					return false;
				}

				if (packetType != _packetType) {
					return false;
				}

				Data* data = (Data*)pcds->lpData;
				for (unsigned int i = 0; i < data->count; i++) {
					PacketHandler::Item<T>* tmp = new PacketHandler::Item<T>({ hWnd, hwndFrom, unique_ptr<T>(new T(*(T*)(data->buf + i * data->packetLength))) });
					_queue.Produce(std::unique_ptr<PacketHandler::Item<T>>(tmp));
				}

				return true;
			}

			void start() {
				_quit = false;

				while (!_quit) {
					/*
					std::unique_ptr<PacketHandler::Item<T>> packet;
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
			SafeQueue<std::unique_ptr<PacketHandler::Item<T>>> _queue;
			ePACKET_TYPE _packetType;
		};

	}


}
