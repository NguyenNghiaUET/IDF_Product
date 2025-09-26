#ifndef SUBMODULE_FRAMEWORK_MESSAGEABS_IO_CODEC
#define SUBMODULE_FRAMEWORK_MESSAGEABS_IO_CODEC
#include "codecmessage.h"
#include "monitorcodec.h"
#include  <memory>
class IOCodec
{
public:
	IOCodec(){};
	virtual ~IOCodec(){};
	virtual bool decode(uint8_t data, CodecMessage* outputMessage) = 0;
	virtual uint16_t encode(CodecMessage *msg, uint8_t* outputData) = 0;

	virtual const monitor_codec_t& getMonitorCodec();

protected:
	monitor_codec_t mMonitorCodec {};
};

inline const monitor_codec_t& IOCodec::getMonitorCodec()
{
	return mMonitorCodec;
}

#endif
