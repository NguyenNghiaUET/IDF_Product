/*
 * codecmainecu.cpp
 *
 *  Created on: May 16, 2024
 *      Author: canhpn2
 */

#include "codecesp32comunication.h"
#include "string.h"

CodecEsp32Comunication::CodecEsp32Comunication()
{

}

CodecEsp32Comunication::~CodecEsp32Comunication()
{
}

bool CodecEsp32Comunication::decode(uint8_t data, CodecMessage *outputMessage)
{
    if(!outputMessage)
    {
        return false;
    }

	mMonitorCodec.numByteReceived++;
	if (mBufferIndex < CodecMessage::MAX_DATA_LENGTH && mParseStep > STEP_IDLE)
		mDataBuffer[mBufferIndex++] = data;

	PARSER_STATUS status = parseFrame(data, outputMessage);
	switch (status)
	{
	case STATUS_WAIT_PREAMBLE:
	case STATUS_GOT_PREAMBLE:
	{
		break;
	}
    case STATUS_CRC_ALL_ERROR:
    case STATUS_CRC_HEADER_ERROR:
	{
		//try to reparse
		//1.Remove first byte
		mBufferIndex -= 1;
		memcpy(mDataBuffer, mDataBuffer + 1, mBufferIndex);
        mParseStep = STEP_IDLE;
		//2.Reparse
		for (uint16_t index = 0; index < mBufferIndex; index++)
		{
			if (parseFrame(mDataBuffer[index], outputMessage) == STATUS_SUCCESS)
			{
				mBufferIndex = 0;
				mParseStep = STEP_IDLE;
				return true;
			}
		}
		//3.Reset data buffer
		mBufferIndex = 0;
        mParseStep = STEP_IDLE;
		break;
	}
	case STATUS_SUCCESS:
	{
		mBufferIndex = 0;
        mParseStep = STEP_IDLE;
		return true;
		break;
	}
	default:
		break;
	}
	return false;
}

CodecEsp32Comunication::PARSER_STATUS CodecEsp32Comunication::parseFrame(uint8_t data,
		CodecMessage *msg)
{
	switch (mParseStep)
	{
	case STEP_IDLE:
	{
		mParseStatus = STATUS_WAIT_PREAMBLE;
		if (data == START_BYTE_MESSAGE_1)
		{
			mParseStep = STEP_GOT_START_BYTE_1;
            resetCrc();
            updateCRCHeader(data);
			mDataIndex = 0;
		}
		else
		{
			mMonitorCodec.countErrByteStart ++;
		}
		break;
	}
	case STEP_GOT_START_BYTE_1:
	{
		if (data == START_BYTE_MESSAGE_2)
		{
			mParseStep = STEP_GOT_START_BYTE_2;
            updateCRCHeader(data);
			mParseStatus = STATUS_GOT_PREAMBLE;
		} else		//not detect preamble, return to STEP_IDLE
		{
			mMonitorCodec.countErrByteStart ++;
			mParseStep = STEP_IDLE;
		}
		break;
	}
	case STEP_GOT_START_BYTE_2:
	{
		msg->mMsgDataLength = data;
        updateCRCHeader(data);
		mParseStep = STEP_GOT_DATA_LENGTH;
		break;
	}
	case STEP_GOT_DATA_LENGTH:
	{
		mSequence &= data;
        updateCRCHeader(data);
		mParseStep = STEP_GOT_SEQUENCE_BYTE_1;
		break;
	}
	case STEP_GOT_SEQUENCE_BYTE_1:
	{
		mSequence &= data << 8;
        updateCRCHeader(data);
		mParseStep = STEP_GOT_SEQUENCE_BYTE_2;
		break;
	}
	case STEP_GOT_SEQUENCE_BYTE_2:
	{
        msg->mMsgID = data;
        updateCRCHeader(data);
		mParseStep = STEP_GOT_MSG_ID;
		break;
	}
	case STEP_GOT_MSG_ID:
	{
        if (data == (mResultHeaderCRC & 0xFF))
        {
            mParseStep = STEP_GOT_CRC_HEADER_1;
            updateCRCMsg(data);
        } else
        {
        	mMonitorCodec.countErrCrcHeader ++;
            mParseStatus = STATUS_CRC_HEADER_ERROR;
        }
		break;
	}
    case STEP_GOT_CRC_HEADER_1:
    {
        if (data == (mResultHeaderCRC>>8))
        {
            mParseStep = STEP_GOT_CRC_HEADER_2;
            updateCRCMsg(data);
        } else
        {
        	mMonitorCodec.countErrCrcHeader ++;
            mParseStatus = STATUS_CRC_HEADER_ERROR;
        }
        break;
    }
    case STEP_GOT_CRC_HEADER_2:
    {
        updateCRCMsg(data);
        msg->mDataRaw[mDataIndex++] = data;
        if (mDataIndex == msg->mMsgDataLength) //dont update mResultCRC from here
            mParseStep = STEP_GOT_DATA;
        break;
    }
	case STEP_GOT_DATA:
	{
        if (data == (mResultAllCRC & 0xFF))
		{
            mParseStep = STEP_GOT_CRC_ALL_1;
		} else
		{
			mMonitorCodec.countErrCrcData ++;
            mParseStatus = STATUS_CRC_ALL_ERROR;
		}
		break;
	}
    case STEP_GOT_CRC_ALL_1:
	{
        if (data == (mResultAllCRC >> 8))
		{
            mParseStep = STEP_GOT_CRC_ALL_2;
            mParseStatus = STATUS_SUCCESS;
            mMonitorCodec.numMessageSuccess++;
		} else
		{
			mMonitorCodec.countErrCrcData ++;
            mParseStatus = STATUS_CRC_ALL_ERROR;
		}
		break;
	}
	default:
	{
		break;
	}
	}

	return mParseStatus;
}

uint16_t CodecEsp32Comunication::encode(CodecMessage *msg, uint8_t *outputData)
{
    if (outputData && msg && (msg->mMsgDataLength > 0))
	{
		//Start byte
		outputData[0] = START_BYTE_MESSAGE_1;
		outputData[1] = START_BYTE_MESSAGE_2;
		//1. Append msg length
		outputData[2] =  msg->mMsgDataLength;
		//2. Append sequence
		mSequence++;
		outputData[3] =  mSequence;
		outputData[4] =  mSequence >> 8;
		//3. Append msgID
        outputData[5] =  msg->mMsgID;
		//4. Append CRC Header
		uint16_t crcHeader = crc16((char*)outputData,
				HEADER_LENGTH - CRC_LENGTH);
		outputData[6] = crcHeader;
		outputData[7] = crcHeader >> 8;

		//5. Append data byte
		memcpy(outputData + HEADER_LENGTH, msg->mDataRaw, msg->mMsgDataLength);

		//6. Append crc
		uint16_t crcAll = crc16((char*)outputData,
				HEADER_LENGTH + msg->mMsgDataLength);
		outputData[HEADER_LENGTH + msg->mMsgDataLength] = crcAll;
		outputData[HEADER_LENGTH + msg->mMsgDataLength + 1] = crcAll >> 8;

        return HEADER_LENGTH + msg->mMsgDataLength + CRC_LENGTH;
    }
    return 0;
}

uint16_t CodecEsp32Comunication::crc16(char *data, int length)
{
	uint16_t resultCheckSum = 0;

	for (int i = 0; i < length; i++)
	{
		resultCheckSum += static_cast<uint8_t>(data[i]);
	}

	return resultCheckSum;
}

void CodecEsp32Comunication::updateCRCHeader(char data)
{
    mResultHeaderCRC += static_cast<uint8_t>(data);
    mResultAllCRC  += static_cast<uint8_t>(data);
}

void CodecEsp32Comunication::updateCRCMsg(char data)
{
    mResultAllCRC += static_cast<uint8_t>(data);
}

void CodecEsp32Comunication::resetCrc()
{
    mResultAllCRC = 0;
    mResultHeaderCRC = 0;
}
