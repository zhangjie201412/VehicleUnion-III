#ifndef __HAVAL_H__
#define __HAVAL_H__

#include "stm32f10x.h"
#include "vehicles.h"

void haval_setup(Vehicles *vehicle);

void haval_ctrl_window(uint8_t state);
void haval_ctrl_door(uint8_t state);
void haval_ctrl_light(uint8_t state);
void haval_ctrl_sunroof(uint8_t state);
void haval_ctrl_trunk(uint8_t state);
void haval_ctrl_findcar(uint8_t state);

void haval_init(uint8_t type);
void haval_exit(uint8_t type);
void haval_keepalive(uint8_t type);
bool haval_engine_on(void);
uint8_t* haval_data_stream(uint8_t pid, uint8_t *len);
uint32_t *haval_check_fault_code(uint8_t id, uint8_t *len);
void haval_clear_fault_code(void);

#endif
