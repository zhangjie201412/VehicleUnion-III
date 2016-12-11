#include "vehicles.h"
#include "includes.h"
#include "utils.h"
#include "pal.h"
#include "transmit.h"
#include "flexcan.h"
#include "eobd.h"
#include "gm.h"
#include "toyota.h"
#include "vag.h"
#include "config.h"
#include "rf_module.h"

#define CONTROL_QUEUE_SIZE  10

OS_TCB VehiclesTaskTCB;
CPU_STK VEHICLES_TASK_STK[VEHICLES_STK_SIZE];
OS_TCB ControlTaskTCB;
CPU_STK CONTROL_TASK_STK[CONTROL_STK_SIZE];

OS_MUTEX mVehicleMutex;

uint16_t mVehiclesInterval;
//update list
UpdateItem mUpdateList[PID_SIZE];
Vehicles mVehicles;

bool mEngineOn = FALSE;

void setVehiclesInterval(uint16_t interval)
{
    mVehiclesInterval = interval;
}

void vehicles_task(void *unused)
{
    uint8_t i, len, j;
    uint8_t *data;
    OS_ERR err;
    unused = unused;

    mVehiclesInterval = 30;
    while(1) {
		OSTimeDlyHMSM(0, 0, mVehiclesInterval,
                0, OS_OPT_TIME_HMSM_STRICT, &err);
        if(mVehicles.ctrlOps == NULL ||
                mVehicles.dataOps == NULL ||
                mVehicles.init == FALSE) {
            loge("vehicle init failed");
            continue;
        }
        //check engine is on
        if(mVehicles.dataOps->is_engine_on()) {
            //engine on
            logi("engine on");
        } else {
            //engine off
            logi("engine off");
        }

        for(i = 0; i < PID_SIZE; i++) {
            xdelay(2);
            vehicle_lock();
            if(mVehicles.dataOps->transfer_data_stream == NULL) {
                loge("transfer data stream function is null");
                vehicle_unlock();
                break;
            }
            vehicle_unlock();
            data = mVehicles.dataOps->transfer_data_stream(i, &len);
            if(data == NULL) {
                if(len != UNSUPPORTED_LEN) {
                    i = (i < ENG_DATA_SIZE) ? ENG_DATA_SIZE : i;
                }
                continue;
            }
            //save data
            mUpdateList[i].len = len;
            for(j = 0; j < len; j++) {
                mUpdateList[i].data[j] = data[j];
            }
            mUpdateList[i].updated = TRUE;
        }
    }
}

void control_task(void *unused)
{
    OS_ERR err;
    OS_MSG_SIZE size;
    CtrlMsg *ctrlMsg;
    uint8_t id;
    uint8_t val;
    uint32_t cmd_id;

    unused = unused;
    while(1) {
        //wait for queue post
        //wait for cmd to control the vehicle
        ctrlMsg = OSTaskQPend(
                (OS_TICK)0,
                (OS_OPT)OS_OPT_PEND_BLOCKING,
                (OS_MSG_SIZE*)&size,
                (CPU_TS*)0,
                (OS_ERR*)&err
                );
        logi("id = %d, cmd_id = %d, value = %d",
                ctrlMsg->id,
                ctrlMsg->cmd_id,
                ctrlMsg->value);
        id = ctrlMsg->id;
        val = ctrlMsg->value;
        cmd_id = ctrlMsg->cmd_id;
        vehicle_lock();
        switch(id) {
            case CONTROL_WINDOW:
                mVehicles.ctrlOps->control_window(val);
                break;
            case CONTROL_DOOR:
                mVehicles.ctrlOps->control_door(val);
                break;
            case CONTROL_LIGHT:
                mVehicles.ctrlOps->control_light(val);
                break;
            case CONTROL_SUNROOF:
                mVehicles.ctrlOps->control_sunroof(val);
                break;
            case CONTROL_TRUNK:
                mVehicles.ctrlOps->control_trunk(val);
                break;
            case CONTROL_FINDCAR:
                mVehicles.ctrlOps->control_findcar(val);
                break;
            case CONTROL_IMMOLOCK:
                if(val) {
                    //rf_lock();
                    //save lock param
                    set_immo_data(0x38);
                    logi("save lock");
                } else {
                    //rf_unlock();
                    //save unlock param
                    set_immo_data(0x00);
                    logi("save unlock");
                }
                break;

            default:
                loge("%s: invalid id", __func__);
                break;
        }
        vehicle_unlock();
        transmit_control_rsp(cmd_id, id);
    }
}

void vehicles_init(void)
{
    OS_ERR err;

    OSMutexCreate(
            (OS_MUTEX *)&mVehicleMutex,
            (CPU_CHAR *)"VEHICLE_MUTEX",
            &err
            );
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
    memset(&mVehicles, 0x00, sizeof(Vehicles));
    flexcan_init(CAN_500K);
    mVehicles.init = FALSE;
#ifdef SERVER_IS_VEHICLE_UNION
#ifdef VEHICLE_TYPE_EOBD
    eobd_setup(&mVehicles);
    flexcan_set_engine_id(0x7e8);
#elif defined VEHICLE_TYPE_TOYOTA
    toyota_setup(&mVehicles);
    flexcan_set_engine_id(0x7e8);
#elif defined VEHICLE_TYPE_GM
    gm_setup(&mVehicles);
    flexcan_set_engine_id(0x7e8);
#elif defined VEHICLE_TYPE_VAG
    vag_setup(&mVehicles);
#endif
#endif
}

void vehicle_setup(uint8_t type)
{
    if(type == VEHICLE_TOYOTA) {
        toyota_setup(&mVehicles);
    } else if(type == VEHICLE_GM) {
        gm_setup(&mVehicles);
    } else if(type == VEHICLE_VAG) {
        vag_setup(&mVehicles);
    } else if(type == VEHICLE_EOBD) {
        eobd_setup(&mVehicles);
    }
}

bool vehicle_engine_on(void)
{
    bool ret;
    vehicle_lock();
    ret = mVehicles.dataOps->is_engine_on();
    vehicle_unlock();
    mEngineOn = ret;
    return ret;
}

bool vehicle_check_engine(void)
{
    return mEngineOn;
}

uint32_t *vehicle_fault_code(uint8_t id, uint8_t *len)
{
    uint32_t *ret;
    vehicle_lock();
    ret = mVehicles.dataOps->check_fault_code(id, len);
    vehicle_unlock();

    return ret;
}

void vehicle_clear_code(void)
{
    vehicle_lock();
    mVehicles.ctrlOps->clear_fault_code();
    vehicle_unlock();
}

void vehicle_lock(void)
{
    OS_ERR err;

//    logi("+++%s+++", __func__);
    OSMutexPend(&mVehicleMutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
}

void vehicle_unlock(void)
{
    OS_ERR err;

//    logi("---%s---", __func__);
    OSMutexPost(&mVehicleMutex, OS_OPT_POST_NONE, &err);
}

