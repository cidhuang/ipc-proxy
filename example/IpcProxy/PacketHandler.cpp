
#include "PacketHandler.h"
#include "Packet.h"

#include <fstream>

namespace IPC_WM_COPYDATA {

	namespace PacketHandler {

		std::string time2Text(long long& time) {
			struct tm timeInfo;
			localtime_s(&timeInfo, &time);
			std::string text = std::to_string(timeInfo.tm_year + 1900) + "-";
			text += std::to_string(timeInfo.tm_mon + 1) + "-";
			text += std::to_string(timeInfo.tm_mday) + " ";
			text += std::to_string(timeInfo.tm_hour) + ":";
			text += std::to_string(timeInfo.tm_min) + ":";
			text += std::to_string(timeInfo.tm_sec);
			return text;
		}

		void appendText(std::string filename, std::string text) {
			std::ofstream outfile;
			outfile.open(filename, std::ios_base::app); // append instead of overwrite
			outfile << text << "\n";
		}

		template<typename TO, typename FROM>
		std::unique_ptr<TO> static_cast_unique_ptr(std::unique_ptr<FROM>&& old) {
			return std::unique_ptr<TO>{static_cast<TO*>(old.release())};
		}

		template<>
		bool handle(std::unique_ptr<Item<MixedLength::Packet>> item) {
			if (item->packet->type() == MixedLength::ePACKET_TYPE_Query) {
				std::unique_ptr<MixedLength::PacketQuery> tmp = static_cast_unique_ptr<MixedLength::PacketQuery>(std::move(item->packet));
				appendText("MixedLength.txt", tmp->from + ": " + tmp->query);
			}
			return true;
		}

		template<>
		bool handle(SafeQueue<std::unique_ptr<Item<MixedLength::Packet>>>& queue) {
			const unsigned int MAX = 100;
			unsigned int count = queue.Size();
			if (count > MAX) {
				count = MAX;
			}

			unsigned i = 0;
			while (i < count) {
				std::unique_ptr<Item<MixedLength::Packet>> item;
				if (queue.Consume(item)) {
					handle(std::move(item));
				}
			}

			return true;
		}

		template<>
		bool handle(std::unique_ptr<Item<FixedLength::Position>> item) {
			appendText("FixedLengthPosition.txt", time2Text(item->packet->time) + ": " + std::to_string(item->packet->x) + ", " + std::to_string(item->packet->y));
			return true;
		}

		template<>
		bool handle(SafeQueue<std::unique_ptr<Item<FixedLength::Position>>>& queue) {
			const unsigned int MAX = 100;
			unsigned int count = queue.Size();
			if (count > MAX) {
				count = MAX;
			}

			unsigned i = 0;
			while (i < count) {
				std::unique_ptr<Item<FixedLength::Position>> item;
				if (queue.Consume(item)) {
					handle(std::move(item));
				}
			}

			return true;
		}

	}

}