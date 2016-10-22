#ifndef __TRANSMIT_H__
#define __TRANSMIT_H__

#include "stm32f10x.h"

typedef struct {
    uint8_t id;
    uint32_t cmd_id;
    uint8_t value;
} CtrlItem;

#endif
