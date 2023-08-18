#pragma once

#include <windows.h>
#include "SafeQueue.hpp"
#include <memory>

namespace IPC_WM_COPYDATA {

	namespace PacketHandler{

		template<typename T>
		struct Item {
			HWND hWnd;
			HWND hwndFrom;
			std::unique_ptr<T> packet;
		};

		template<typename T>
		bool handle(SafeQueue<std::unique_ptr<Item<T>>>& queue);

		template<typename T>
		bool handle(std::unique_ptr<Item<T>> packet);

	}

}