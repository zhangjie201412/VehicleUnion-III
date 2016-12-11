#ifndef __VAG_H__
#define __VAG_H__

#include "stm32f10x.h"
#include "vehicles.h"

void vag_setup(Vehicles *vehicle);

void vag_ctrl_window(uint8_t state);
void vag_ctrl_door(uint8_t state);
void vag_ctrl_light(uint8_t state);
void vag_ctrl_sunroof(uint8_t state);
void vag_ctrl_trunk(uint8_t state);
void vag_ctrl_findcar(uint8_t state);

bool vag_engine_on(void);
uint8_t* vag_data_stream(uint8_t pid, uint8_t *len);
uint32_t *vag_check_fault_code(uint8_t id, uint8_t *len);
void vag_clear_fault_code(void);

#endif
