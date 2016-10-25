#ifndef __TRANSMIT_H__
#define __TRANSMIT_H__

#include "stm32f10x.h"
#include "includes.h"
#include "utils.h"

#define TRANSMIT_CALLBACK_TASK_PRIO		8u
#define TRANSMIT_CALLBACK_STK_SIZE 		512u
//heart beat thread
#define HEARTBEAT_TASK_PRIO		16u
#define HEARTBEAT_STK_SIZE 		512u
//upload thread
#define UPLOAD_TASK_PRIO        10u
#define UPLOAD_STK_SIZE         512u

typedef struct {
    uint8_t id;
    char key[NAME_MAX_SIZE];
} CtrlItem;

typedef struct {
    uint8_t id;
    uint32_t cmd_id;
    uint8_t value;
} CtrlMsg;

extern OS_TCB TransmitCallbackTaskTCB;

void transmit_init(void);
void transmit_control_rsp(uint32_t cmd_id, uint8_t id);

#endif