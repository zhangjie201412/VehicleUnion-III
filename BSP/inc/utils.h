#ifndef __UTILS_H__
#define __UTILS_H__

#include "stdio.h"
#include "stm32f10x.h"
#include "cJSON.h"
#include "pal.h"
#include "config.h"

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
        printf("[%05d.%03d/I]: " fmt "\r\n",           \
                ticks / 5000, ticks % 5000 / 5,          \
##__VA_ARGS__);                        \
    } while(0)

#define logw(fmt, ...)                                     \
    do {                                                   \
        OS_ERR err;                                        \
        uint32_t ticks = OSTimeGet(&err);                  \
        if(DEBUG_MODE & LOG_WARN)                          \
        printf("[%05d.%03d/W]: " fmt "\r\n",           \
                ticks / 5000, ticks % 5000 / 5,          \
##__VA_ARGS__);                        \
    } while(0)

#define logd(fmt, ...)                                     \
    do {                                                   \
        OS_ERR err;                                        \
        uint32_t ticks = OSTimeGet(&err);                  \
        if(DEBUG_MODE & LOG_DEBUG)                         \
        printf("[%05d.%03d/D]: " fmt "\r\n",           \
                ticks / 5000, ticks % 5000 / 5,          \
##__VA_ARGS__);                        \
    } while(0)

#define loge(fmt, ...)                                     \
    do {                                                   \
        OS_ERR err;                                        \
        uint32_t ticks = OSTimeGet(&err);                  \
        if(DEBUG_MODE & LOG_ERROR)                         \
        printf("[%05d.%03d/E]: " fmt "\r\n",           \
                ticks / 5000, ticks % 5000 / 5,          \
##__VA_ARGS__);                        \
    } while(0)

#define NAME_MAX_SIZE                   32
#define DEVICE_ID_ADDRESS               0x80

#ifdef SERVER_IS_K
    #define KEY_DEVICE_ID       "deviceid"
    #define KEY_MSG_TYPE        "msg_type"
    #define KEY_HEARTBEAT       "heartbeat_count"
    #define KEY_CMD_ID          "cmd_id"
    #define KEY_VEHICLE_TYPE    "model"
    #define KEY_STATUS          "status"
    #define KEY_CMD_TYPE        "cmd_type"
    #define KEY_LNG             "lng"
    #define KEY_LAT             "lat"
#elif defined SERVER_IS_VEHICLE_UNION
    #define KEY_DEVICE_ID       "deviceid"
    #define KEY_MSG_TYPE        "msg_type"
    #define KEY_HEARTBEAT       "heartbeat_count"
    #define KEY_CMD_ID          "cmd_id"
    #define KEY_VEHICLE_TYPE    "model"
    #define KEY_STATUS          "status"
    #define KEY_CMD_TYPE        "cmd_type"
    #define KEY_LNG             "lng"
    #define KEY_LAT             "lat"
#endif

#define LOGIN_DELAYED_TIME          6

#ifdef SERVER_IS_K
    #define MSG_TYPE_HEARTBEAT          0
    #define MSG_TYPE_HEARTBEAT_RSP      1
    #define MSG_TYPE_CTRL               2
    #define MSG_TYPE_CTRL_RSP           3
    #define MSG_TYPE_UPLOAD             4
    #define MSG_TYPE_FAULT_CODE         5
    #define MSG_TYPE_CLEAR_FAULT        6
    #define MSG_TYPE_LOCATION           7
    #define MSG_TYPE_LOGIN              8
    #define MSG_TYPE_LOGIN_RSP          9
    #define MSG_TYPE_VEHICLE_TYPE       10
    #define MSG_TYPE_RETRY              11
#elif defined SERVER_IS_VEHICLE_UNION
    #define MSG_TYPE_HEARTBEAT          2
    #define MSG_TYPE_HEARTBEAT_RSP      2
    #define MSG_TYPE_CTRL               3
    #define MSG_TYPE_CTRL_RSP           4
    #define MSG_TYPE_UPLOAD             0
    #define MSG_TYPE_FAULT_CODE         5
    #define MSG_TYPE_CLEAR_FAULT        6
    /*           UNUSED BELOW          */
    /*                 |               */
    /*                 |               */
    /*                 v               */
    #define MSG_TYPE_LOCATION           7
    #define MSG_TYPE_LOGIN              8
    #define MSG_TYPE_LOGIN_RSP          9
    #define MSG_TYPE_VEHICLE_TYPE       10
    #define MSG_TYPE_RETRY              11
    /*                 ^               */
    /*                 |               */
    /*                 |               */
    /*                                 */
#endif

bool is_connected(void);
void heartbeat(uint8_t count);
void control_rsp(uint32_t cmd_id, uint8_t cmd_type, char *key);
void upload_item(UpdateItem *item, char *key);
void upload_location(void);
void get_deviceid(void);
uint8_t json_get_msg_type(cJSON *json);
uint8_t json_get_heartbeat(cJSON *json);
void login(void);

void xdelay(uint8_t s);
void xdelay_ms(uint16_t s);
#endif
