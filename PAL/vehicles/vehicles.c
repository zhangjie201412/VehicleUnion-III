#include "vehicles.h"
#include "includes.h"
#include "utils.h"
#include "pal.h"

#define CONTROL_QUEUE_SIZE

OS_TCB VehiclesTaskTCB;
CPU_STK VEHICLES_TASK_STK[VEHICLES_STK_SIZE];
OS_TCB ControlTaskTCB;
CPU_STK CONTROL_TASK_STK[CONTROL_STK_SIZE];

uint16_t mVehiclesInterval;
//update list
UpdateItem mUpdateList[PID_SIZE];
Vehicles mVehicles;

void setVehiclesInterval(uint16_t interval)
{
    mVehiclesInterval = interval;
}

void vehicles_task(void *unused)
{
    OS_ERR err;
    unused = unused;

    mVehiclesInterval = 10;
    while(1) {
		OSTimeDlyHMSM(0, 0, mVehiclesInterval,
                0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

void control_task(void *unused)
{
    OS_ERR err;
    MS_MSG_SIZE size;
    unused = unused;

    while(1) {
        //wait for queue post
        //wait for cmd to control the vehicle
		OSTimeDlyHMSM(0, 0, mVehiclesInterval,
                0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

void vehicles_init(void)
{
    OS_ERR err;

    //register the vehicles for your car

    OSTaskCreate((OS_TCB 	* )&VehiclesTaskTCB,		
            (CPU_CHAR	* )"vehicles task", 		
            (OS_TASK_PTR )vehicles_task,	
            (void		* )0,					
            (OS_PRIO	  )VEHICLES_TASK_PRIO,     
            (CPU_STK   * )&VEHICLES_TASK_STK[0],	
            (CPU_STK_SIZE)VEHICLES_STK_SIZE/10,	
            (CPU_STK_SIZE)VEHICLES_STK_SIZE,		
            (OS_MSG_QTY  )0,					
            (OS_TICK	  )0,					
            (void   	* )0,					
            (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
            (OS_ERR 	* )&err);				
    OSTaskCreate((OS_TCB 	* )&ControlTaskTCB,		
            (CPU_CHAR	* )"control task", 		
            (OS_TASK_PTR )control_task,	
            (void		* )0,					
            (OS_PRIO	  )CONTROL_TASK_PRIO,     
            (CPU_STK   * )&CONTROL_TASK_STK[0],	
            (CPU_STK_SIZE)CONTROL_STK_SIZE/10,	
            (CPU_STK_SIZE)CONTROL_STK_SIZE,		
            (OS_MSG_QTY  )CONTROL_QUEUE_SIZE,	
            (OS_TICK	  )0,					
            (void   	* )0,					
            (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
            (OS_ERR 	* )&err);				
}
