#include "utils.h"
#include "cJSON.h"
#include "flash.h"
#include "stdio.h"
#include "config.h"
#include "l206.h"

uint8_t DEBUG_MODE = LOG_ERROR | LOG_INFO;
uint8_t deviceid[17];

bool is_connected(void)
{
    return l206_is_connected();
}

void heartbeat(uint8_t count)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;

    get_deviceid();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_HEARTBEAT);
    cJSON_AddNumberToObject(root, KEY_HEARTBEAT, count);

    out = cJSON_Print(root);
    length = strlen(out);
    l206_send((uint8_t *)out, length);

    cJSON_Delete(root);
    free(out);
}

void control_rsp(uint32_t cmd_id, uint8_t cmd_type, char *key)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;

    get_deviceid();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_CTRL_RSP);
#ifdef SERVER_IS_K
    cJSON_AddNumberToObject(root, KEY_CMD_ID, cmd_id);
#endif
    cJSON_AddNumberToObject(root, KEY_STATUS, 0);
    cJSON_AddStringToObject(root, KEY_CMD_TYPE, key);

    out = cJSON_Print(root);
    length = strlen(out);
    l206_send((uint8_t *)out, length);

    cJSON_Delete(root);
    free(out);
}

void upload_item(UpdateItem *item, char *key)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;
    char val_buf[16];
    uint8_t i;
    uint8_t index = 0, n = 0;

    get_deviceid();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_UPLOAD);
    //make value
    memset(val_buf, 0x00, 16);
    for(i = 0; i < item->len; i++) {
        n = sprintf(val_buf + index, "%02x", item->data[i]);
        index += n;
    }
    cJSON_AddStringToObject(root, key, val_buf);

    //printf("%s: %s\r\n", __func__, getPidKey(item->pid));
    out = cJSON_Print(root);
    length = strlen(out);
    l206_send((uint8_t *)out, length);

    cJSON_Delete(root);
    free(out);
}

void upload_location(void)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;
    uint32_t lng, lat;

    get_deviceid();

    l206_get_location(&lng, &lat);
    if(lng == 0 || lat == 0) {
        return;
    }
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_LOCATION);
    cJSON_AddNumberToObject(root, KEY_LNG, lng);
    cJSON_AddNumberToObject(root, KEY_LAT, lat);

    out = cJSON_Print(root);
    length = strlen(out);
    l206_send((uint8_t *)out, length);

    cJSON_Delete(root);
    free(out);
}

void get_deviceid(void)
{
    flash_page_read(deviceid, DEVICE_ID_ADDRESS, 17);
}

uint8_t g_immo_data[2];
void set_immo_data(uint8_t data)
{
    uint8_t ret;
    g_immo_data[0] = data;
    //flash_page_write(g_immo_data, IMMO_DATA_ADDRESS, 1);
    ret = bsp_WriteCpuFlash(IMMO_DATA_ADDRESS, g_immo_data, 2);
}

uint8_t get_immo_data(void)
{
    //flash_page_read(g_immo_data, IMMO_DATA_ADDRESS, 1);
    bsp_ReadCpuFlash(IMMO_DATA_ADDRESS, g_immo_data, 2);

    return g_immo_data[0];
}

uint8_t json_get_msg_type(cJSON *json)
{
    cJSON *item = cJSON_GetObjectItem(json, KEY_MSG_TYPE);
    if(item) {
        return item->valueint;
    } else {
        return 0;
    }
}

uint8_t json_get_heartbeat(cJSON *json)
{
    cJSON *item = cJSON_GetObjectItem(json, KEY_HEARTBEAT);
    if(item) {
        return item->valueint;
    } else {
        return 0;
    }
}

void login(void)
{
    cJSON *root = cJSON_CreateObject();
    char *out;
    uint16_t length;

    get_deviceid();
    cJSON_AddStringToObject(root, KEY_DEVICE_ID, (const char *)deviceid);
    cJSON_AddNumberToObject(root, KEY_MSG_TYPE, MSG_TYPE_LOGIN);

    out = cJSON_Print(root);
    length = strlen(out);
    l206_send((uint8_t *)out, length);
    cJSON_Delete(root);
    free(out);
}

void xdelay(uint8_t s)
{
    OS_ERR err;
    OSTimeDlyHMSM(0, 0, s, 0, OS_OPT_TIME_HMSM_STRICT, &err);
}

void xdelay_ms(uint16_t s)
{
    OS_ERR err;
    OSTimeDlyHMSM(0, 0, 0, s, OS_OPT_TIME_HMSM_STRICT, &err);
}
