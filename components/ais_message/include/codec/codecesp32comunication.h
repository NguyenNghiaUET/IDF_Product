/*
 * codecmainecu.h
 *
 *  Created on: May 16, 2024
 *      Author: canhpn2
 */

#ifndef INC_CODECMAINECU_H_
#define INC_CODECMAINECU_H_
#include "stdio.h"
#include <vector>
#include "iocodec.h"

class CodecEsp32Comunication: public IOCodec
{
	enum PARSE_MESSAGE_STEP
	{
		STEP_IDLE,
		STEP_GOT_START_BYTE_1,
		STEP_GOT_START_BYTE_2,
		STEP_GOT_DATA_LENGTH,
		STEP_GOT_SEQUENCE_BYTE_1,
		STEP_GOT_SEQUENCE_BYTE_2,
		STEP_GOT_MSG_ID,
		STEP_GOT_CRC_HEADER_1,
		STEP_GOT_CRC_HEADER_2,
		STEP_GOT_DATA,
		STEP_GOT_CRC_ALL_1,
		STEP_GOT_CRC_ALL_2
	};

	enum PARSER_STATUS
	{
		STATUS_WAIT_PREAMBLE,
		STATUS_GOT_PREAMBLE,
		STATUS_CRC_HEADER_ERROR,
		STATUS_CRC_ALL_ERROR,
		STATUS_SUCCESS
	};
public:
	CodecEsp32Comunication();
	virtual ~CodecEsp32Comunication();

	static const uint8_t START_BYTE_MESSAGE_1 = 0x55;
	static const uint8_t START_BYTE_MESSAGE_2 = 0x55;
	static const uint8_t HEADER_LENGTH = 8;
	static const uint8_t PREAMBLE_LENGTH = 2;
	static const uint8_t SEQUENCE_INDEX = 4;
	static const uint8_t CRC_LENGTH = 2;

	bool decode(uint8_t data, CodecMessage *outputMessage) override;
	uint16_t encode(CodecMessage *msg, uint8_t *outputData) override;
private:
	uint16_t crc16(char *data, int length);
	void updateCRCHeader(char data);
	void updateCRCMsg(char data);
	void resetCrc();
	uint16_t mResultHeaderCRC {0};
	uint16_t mResultAllCRC {0};

private:

	PARSER_STATUS parseFrame(uint8_t data, CodecMessage *msg);

	uint8_t mDataBuffer[CodecMessage::MAX_DATA_LENGTH] {};
	uint16_t mBufferIndex {0};

	uint16_t mSequence {0};
	bool mInitDecode {true};
	uint16_t mLastSequenceReceived {0};
	uint16_t mCurrentSequenceReceived {0};

	uint8_t mDataIndex {0};

	PARSE_MESSAGE_STEP mParseStep {STEP_IDLE};
	PARSER_STATUS mParseStatus {STATUS_WAIT_PREAMBLE};
};

#endif /* INC_CODECMAINECU_H_ */
