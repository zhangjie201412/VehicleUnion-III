#ifndef __EOBD_H__
#define __EOBD_H__

#include "stm32f10x.h"
#include "vehicles.h"

void eobd_setup(Vehicles *vehicle);

void eobd_ctrl_window(uint8_t state);
void eobd_ctrl_door(uint8_t state);
void eobd_ctrl_light(uint8_t state);
void eobd_ctrl_sunroof(uint8_t state);
void eobd_ctrl_trunk(uint8_t state);
void eobd_ctrl_findcar(uint8_t state);

void eobd_init(uint8_t type);
void eobd_exit(uint8_t type);
void eobd_keepalive(uint8_t type);

bool eobd_engine_on(void);
uint8_t* eobd_data_stream(uint8_t pid, uint8_t *len);
uint32_t *eobd_check_fault_code(uint8_t id, uint8_t *len);
uint8_t eobd_engine_on(void);
void eobd_clear_fault_code(void);

#endif
