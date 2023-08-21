
#include "PacketHandler.h"
#include "Packet.h"

#include <fstream>
#include <codecvt>
using namespace std;

namespace IPC_WM_COPYDATA {

	namespace PacketHandler {

		string prefix0(string src) {
			if (src.length() == 1) {
				return "0" + src;
			}
			return src;
		}

		string time2Text(long long& time) {
			struct tm timeInfo;
			localtime_s(&timeInfo, &time);
			string text = to_string(timeInfo.tm_year + 1900) + "-";
			text += prefix0(to_string(timeInfo.tm_mon + 1)) + "-";
			text += prefix0(to_string(timeInfo.tm_mday)) + " ";
			text += prefix0(to_string(timeInfo.tm_hour)) + ":";
			text += prefix0(to_string(timeInfo.tm_min)) + ":";
			text += prefix0(to_string(timeInfo.tm_sec));
			return text;
		}

		void appendText(string filename, string text) {
			ofstream outfile;
			outfile.open(filename, ios_base::app); // append instead of overwrite
			outfile << text << "\n";
		}

		template<typename TO, typename FROM>
		unique_ptr<TO> static_cast_unique_ptr(unique_ptr<FROM>&& old) {
			return unique_ptr<TO>{static_cast<TO*>(old.release())};
		}

		template<>
		bool handle(unique_ptr<Item<MixedLength::Packet>> item) {
			if (item->packet->type() == MixedLength::ePACKET_TYPE_Query) {
				unique_ptr<MixedLength::PacketQuery> tmp = static_cast_unique_ptr<MixedLength::PacketQuery>(move(item->packet));
				appendText("MixedLengthQuery.txt", "Query: " + to_string(tmp->from) + ", " + tmp->query);
				return true;
			}
			if (item->packet->type() == MixedLength::ePACKET_TYPE_Comment) {
				unique_ptr<MixedLength::PacketComment> tmp = static_cast_unique_ptr<MixedLength::PacketComment>(move(item->packet));
				appendText("MixedLengthComment.txt", "Comment: " + to_string(tmp->from) + ", " + tmp->comment);
				return true;
			}
			return false;
		}

		template<>
		bool handle(SafeQueue<unique_ptr<Item<MixedLength::Packet>>>& queue) {
			const unsigned int MAX = 100;
			unsigned int count = queue.Size();
			if (count > MAX) {
				count = MAX;
			}

			unsigned i = 0;
			while (i < count) {
				unique_ptr<Item<MixedLength::Packet>> item;
				if (queue.Consume(item)) {
					handle(move(item));
				}
				i++;
			}

			return true;
		}

		template<>
		bool handle(unique_ptr<Item<FixedLength::Position>> item) {
			appendText("FixedLengthPosition.txt", "Position: " + time2Text(item->packet->time) + ", " + to_string(item->packet->x) + ", " + to_string(item->packet->y));
			return true;
		}

		template<>
		bool handle(SafeQueue<unique_ptr<Item<FixedLength::Position>>>& queue) {
			const unsigned int MAX = 100;
			unsigned int count = queue.Size();
			if (count > MAX) {
				count = MAX;
			}

			unsigned i = 0;
			while (i < count) {
				unique_ptr<Item<FixedLength::Position>> item;
				if (queue.Consume(item)) {
					handle(move(item));
				}
				i++;
			}

			return true;
		}

		template<>
		bool handle(unique_ptr<Item<FixedLength::Size>> item) {
			appendText("FixedLengthSize.txt", "Size: " + time2Text(item->packet->time) + ", " + to_string(item->packet->width) + ", " + to_string(item->packet->height));
			return true;
		}

		template<>
		bool handle(SafeQueue<unique_ptr<Item<FixedLength::Size>>>& queue) {
			const unsigned int MAX = 100;
			unsigned int count = queue.Size();
			if (count > MAX) {
				count = MAX;
			}

			unsigned i = 0;
			while (i < count) {
				unique_ptr<Item<FixedLength::Size>> item;
				if (queue.Consume(item)) {
					handle(move(item));
				}
				i++;
			}

			return true;
		}

	}

}