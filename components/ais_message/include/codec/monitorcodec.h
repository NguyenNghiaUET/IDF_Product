/*
 * monitorcodec.h
 *
 *  Created on: Aug 29, 2024
 *      Author: tungtd10
 */

#ifndef BUSINESS_MESSAGE_MESSAGE_CODEC_INC_MONITORCODEC_H_
#define BUSINESS_MESSAGE_MESSAGE_CODEC_INC_MONITORCODEC_H_

#include <cstdint>

#pragma pack(push, 1)
	typedef struct monitor_codec
	{
		uint16_t countErrByteStart;
		uint16_t countErrCrcHeader;
		uint16_t countErrCrcData;
		uint16_t countLostSequence;
		uint16_t numMessageSuccess;
		uint32_t numByteReceived;
		uint32_t numByteError;
	} monitor_codec_t;
#pragma pack(pop)


#endif /* BUSINESS_MESSAGE_VTJC2_MESSAGE_CODEC_INC_MONITORCODEC_H_ */
