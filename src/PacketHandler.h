#pragma once

#include <windows.h>
#include "SafeQueue.hpp"
#include <memory>
using namespace std;

namespace IPC_WM_COPYDATA {

	namespace PacketHandler{

		template<typename T>
		struct Item {
			HWND hWnd;
			HWND hwndFrom;
			unique_ptr<T> packet;
		};

		template<typename T>
		bool handle(unique_ptr<Item<T>> item);

		template<typename T>
		bool handle(SafeQueue<unique_ptr<Item<T>>>& queue);

		template<typename TO, typename FROM>
		unique_ptr<TO> static_cast_unique_ptr(unique_ptr<FROM>&& old) {
			return unique_ptr<TO>{static_cast<TO*>(old.release())};
		}

	}

}