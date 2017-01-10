#ifndef __PASSAT_H__
#define __PASSAT_H__

#include "stm32f10x.h"
#include "vehicles.h"

void passat_setup(Vehicles *vehicle);

void passat_ctrl_window(uint8_t state);
void passat_ctrl_door(uint8_t state);
void passat_ctrl_light(uint8_t state);
void passat_ctrl_sunroof(uint8_t state);
void passat_ctrl_trunk(uint8_t state);
void passat_ctrl_findcar(uint8_t state);

void passat_init(uint8_t type);
void passat_exit(uint8_t type);
void passat_keepalive(uint8_t type);
bool passat_engine_on(void);
uint8_t* passat_data_stream(uint8_t pid, uint8_t *len);
uint32_t *passat_check_fault_code(uint8_t id, uint8_t *len);
void passat_clear_fault_code(void);

#endif
