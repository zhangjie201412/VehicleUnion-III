#ifndef __VEHICLES_H__
#define __VEHICLES_H__

#include "stm32f10x.h"

#define VEHICLES_TASK_PRIO		6u
#define VEHICLES_STK_SIZE 		512u
#define CONTROL_TASK_PRIO		7u
#define CONTROL_STK_SIZE 		256u

typedef struct {
    void (*control_window)(uint8_t state);
    void (*control_door)(uint8_t state);
    void (*control_light)(uint8_t state);
    void (*control_sunfloor)(uint8_t state);
    void (*control_trunk)(uint8_t state);
    void (*control_findcar)(uint8_t state);
    void (*clear_fault_code)(void);
} VehiclesCtrlOps;

typedef struct {
    uint8_t (*is_engine_on)(void);
    uint8_t *(*transfer_data_stream)(uint8_t pid, uint8_t *len);
    uint32_t *(*check_fault_code)(uint8_t id, uint8_t *len);
} VehiclesDataOps;

typedef struct {
    VehiclesDataOps *dataOps;
    VehiclesCtrlOps *ctrlOps;
} Vehicles;

void vehicles_init(void);
void vehicles_task(void *unused);
void control_task(void *unused);
#endif
