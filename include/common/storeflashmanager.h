#ifndef CONFIG_MANAGER
#define CONFIG_MANAGER
#include "configsystemmessage.h"
#include "codecesp32comunication.h"

class StoreFlashManager
{
private:
    /* data */
public:
    static constexpr char KEY_SYSTEM_CONFIG[] = "KEY_SYSTEM_CONFIG";
    static constexpr int MAX_BUFFER_CONFIG = 300;
    StoreFlashManager();
    ~StoreFlashManager();
    static StoreFlashManager *getInstance();

    ConfigSystemMessage configSystem();
    bool readConfigFromFlash();
    bool saveConfigToFlash(ConfigSystemMessage msg);
private:
    static StoreFlashManager *mInstance;

    ConfigSystemMessage mConfig;
    CodecEsp32Comunication* mCodecEsp32Comunication;
    uint8_t mBufferConfigFlash[MAX_BUFFER_CONFIG] {0};
};

#endif