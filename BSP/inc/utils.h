#ifndef __UTILS_H__
#define __UTILS_H__

#include "stdio.h"
#include "stm32f10x.h"

extern uint8_t DEBUG_MODE;

enum {
    LOG_ERROR       = 1 << 0,
    LOG_INFO        = 1 << 1,
    LOG_WARN        = 1 << 2,
    LOG_DEBUG       = 1 << 3,
};

#define logi(fmt, ...)                                     \
    do {                                                   \
        OS_ERR err;                                        \
        uint32_t ticks = OSTimeGet(&err);                  \
        if(DEBUG_MODE & LOG_INFO)                          \
            printf("[%04d.%02d/I]: " fmt "\r\n",           \
                    ticks / 200, ticks / 2 % 100,          \
                    ##__VA_ARGS__);                        \
    } while(0)

#define logw(fmt, ...)                                     \
    do {                                                   \
        OS_ERR err;                                        \
        uint32_t ticks = OSTimeGet(&err);                  \
        if(DEBUG_MODE & LOG_WARN)                          \
            printf("[%04d.%02d/W]: " fmt "\r\n",           \
                    ticks / 200, ticks / 2 % 100,          \
                    ##__VA_ARGS__);                        \
    } while(0)

#define logd(fmt, ...)                                     \
    do {                                                   \
        OS_ERR err;                                        \
        uint32_t ticks = OSTimeGet(&err);                  \
        if(DEBUG_MODE & LOG_DEBUG)                         \
            printf("[%04d.%02d/D]: " fmt "\r\n",           \
                    ticks / 200, ticks / 2 % 100,          \
                    ##__VA_ARGS__);                        \
    } while(0)

#define loge(fmt, ...)                                     \
    do {                                                   \
        OS_ERR err;                                        \
        uint32_t ticks = OSTimeGet(&err);                  \
        if(DEBUG_MODE & LOG_ERROR)                         \
            printf("[%04d.%02d/E]: " fmt "\r\n",           \
                    ticks / 200, ticks / 2 % 100,          \
                    ##__VA_ARGS__);                        \
    } while(0)

#endif
