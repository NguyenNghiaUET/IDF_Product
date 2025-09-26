#include "common/storeflashmanager.h"
#include "loggermanager.h"
#include "common/common.h"

StoreFlashManager *StoreFlashManager::mInstance = nullptr;

StoreFlashManager::StoreFlashManager()
{
    mCodecEsp32Comunication = new CodecEsp32Comunication();
}

StoreFlashManager::~StoreFlashManager()
{
}

StoreFlashManager *StoreFlashManager::getInstance()
{
    if (mInstance == nullptr)
    {
        mInstance = new StoreFlashManager();
    }
    return mInstance;
}
ConfigSystemMessage StoreFlashManager::configSystem()
{
    return mConfig;
}

bool StoreFlashManager::readConfigFromFlash()
{
    size_t lengthDataReaded = 0;
    if(gFlashManager == nullptr)
    {
        LOG_ERROR("StoreFlashManager", "why gFlashManager null?");
        return false;
    }
    bool ret = gFlashManager->readBlob(KEY_SYSTEM_CONFIG, mBufferConfigFlash, MAX_BUFFER_CONFIG, lengthDataReaded);
    if (ret == false)
    {
        LOG_ERROR("StoreFlashManager", "Ready data system from nvs faild");
        return false;
    }

    CodecMessage codecMsg;
    for (size_t i = 0; i < lengthDataReaded; i++)
    {
        if (mCodecEsp32Comunication->decode(mBufferConfigFlash[i], &codecMsg))
        {
            AbstractMessage::MessageId msgID = static_cast<AbstractMessage::MessageId>(codecMsg.mMsgID);
            switch (msgID)
            {
            case AbstractMessage::MessageId::CONFIG_SYSTEM_MESSAGE:
            {
                auto ret = mConfig.unpackData(&codecMsg);
                return ret;
            }
            default:
            {
                LOG_ERROR("StoreFlashManager", "Why parse msg id: %d", (int)msgID);
                break;
            }
            }
        }
    }
    return true;
}

bool StoreFlashManager::saveConfigToFlash(ConfigSystemMessage msg)
{
    CodecMessage codecMsg;
    uint8_t buffer[CodecMessage::MAX_DATA_LENGTH];
    auto retPack = msg.packData(&codecMsg);
    if(!retPack)
    {
        LOG_ERROR("StoreFlashManager", "ConfigSystemMessage pack data error");
        return false;
    }
    uint16_t msgLength = mCodecEsp32Comunication->encode(&codecMsg, buffer);
    if (msgLength > 0)
    {
        bool ret = gFlashManager->writeBlob(KEY_SYSTEM_CONFIG, buffer, msgLength);
        if (ret == false)
        {
            LOG_ERROR("StoreFlashManager", "write data system from nvs faild");
            return false;
        }
    }
    else
    {
        LOG_ERROR("StoreFlashManager", "encode error");
        return false;
    }
    return true;
}
