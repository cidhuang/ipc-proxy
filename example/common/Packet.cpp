#include "Packet.h"
#include "FixedLength.h"
#include "MixedLength.h"

#include <string>

namespace IPC_WM_COPYDATA {

	namespace FixedLength {

		template<>
		ePACKET_TYPE getPacketType<Position>() {
			return ePACKET_TYPE_Position;
		}

		template<>
		ePACKET_TYPE getPacketType<Size>() {
			return ePACKET_TYPE_Size;
		}

	}

	namespace MixedLength {

		Packet* getPacket(char* buf) {
			Packet* tmp = 0;
			switch (getPacketType(buf))
			{
			case ePACKET_TYPE_Query:
				tmp = new PacketQuery();
				if (!tmp->parse(buf)) {
					return 0;
				}
				break;
			case ePACKET_TYPE_Comment:
				tmp = new PacketComment();
				if (!tmp->parse(buf)) {
					return 0;
				}
				break;
			default:
				break;
			}

			return tmp;
		}

		PacketQuery::PacketQuery()
			: Packet()
			, from(0)
			, query("")
		{
			_type = ePACKET_TYPE_Query;
		}

		PacketQuery::~PacketQuery() {}

		bool PacketQuery::_parsePayload(const char* payloadSrc, unsigned int payloadLength) {
			unsigned int* tmp = (unsigned int*)payloadSrc;
			from = *tmp;
			query = payloadSrc + sizeof(unsigned int);
			if (payloadLength != (sizeof(from) + query.length() + 1)) {
				return false;
			}
			return true;
		}

		void PacketQuery::_setPayload(char* dst) {
			unsigned int* tmp = (unsigned int*)dst;
			*tmp = from;
			query.copy(dst + sizeof(unsigned int), query.length());
			dst[_payloadLength() - 1] = 0;
		}

		unsigned int PacketQuery::_payloadLength() {
			return sizeof(from) + query.length() + 1;
		}

		PacketComment::PacketComment()
			: Packet()
			, from(0)
			, comment("")
		{
			_type = ePACKET_TYPE_Comment;
		}

		PacketComment::~PacketComment() {}

		bool PacketComment::_parsePayload(const char* payloadSrc, unsigned int payloadLength) {
			unsigned int* tmp = (unsigned int*)payloadSrc;
			from = *tmp;
			comment = payloadSrc + sizeof(unsigned int);
			if (payloadLength != (sizeof(from) + comment.length() + 1)) {
				return false;
			}
			return true;
		}

		void PacketComment::_setPayload(char* dst) {
			unsigned int* tmp = (unsigned int*)dst;
			*tmp = from;
			comment.copy(dst + sizeof(unsigned int), comment.length());
			dst[_payloadLength() - 1] = 0;
		}

		unsigned int PacketComment::_payloadLength() {
			return sizeof(from) + comment.length() + 1;
		}

	}
}
