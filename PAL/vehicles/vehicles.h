#ifndef __VEHICLES_H__
#define __VEHICLES_H__

#include "stm32f10x.h"
#include "includes.h"
#include "pal.h"
#include "transmit.h"

#define VEHICLES_TASK_PRIO		6u
#define VEHICLES_STK_SIZE 		512u
#define CONTROL_TASK_PRIO		7u
#define CONTROL_STK_SIZE 		256u

typedef struct {
    void (*control_window)(uint8_t state);
    void (*control_door)(uint8_t state);
    void (*control_light)(uint8_t state);
    void (*control_sunroof)(uint8_t state);
    void (*control_trunk)(uint8_t state);
    void (*control_findcar)(uint8_t state);
    void (*clear_fault_code)(void);
} VehiclesCtrlOps;

typedef struct {
    bool (*is_engine_on)(void);
    uint8_t *(*transfer_data_stream)(uint8_t pid, uint8_t *len);
    uint32_t *(*check_fault_code)(uint8_t id, uint8_t *len);
} VehiclesDataOps;

typedef struct {
    VehiclesDataOps *dataOps;
    VehiclesCtrlOps *ctrlOps;
} Vehicles;

extern OS_TCB ControlTaskTCB;
extern UpdateItem mUpdateList[PID_SIZE];

void vehicles_init(void);
void vehicles_task(void *unused);
void control_task(void *unused);
bool vehicle_engine_on(void);
uint32_t *vehicle_fault_code(uint8_t id, uint8_t *len);
void vehicle_clear_code(void);


#endif