#include "immo_task.h"
#include "utils.h"
#include "cJSON.h"
#include "flash.h"
#include "sim800.h"
#include "utils.h"
#include "flexcan.h"
#include "rf_module.h"

//任务控制块
OS_TCB ImmoTaskTCB;
//任务堆栈	
CPU_STK IMMO_TASK_STK[IMMO_STK_SIZE];

void immo_task(void *p_arg)
{
    uint8_t i;
    int8_t ret;
    OS_ERR err;
    uint8_t immo_lock;

    p_arg = p_arg;
    rf_module_init();

    immo_lock = get_immo_data();
    if(immo_lock == 0x38) {
        rf_lock();
        rf_lock();
        rf_lock();
        rf_lock();
    } else {
        rf_unlock();
        rf_unlock();
        rf_unlock();
        rf_unlock();
    }

    while(1)
    {
        immo_lock = get_immo_data();
        OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
        if(immo_lock == 0x38) {
            rf_lock();
            //logi("rf lock");
        } else {
            rf_unlock();
            //logi("rf unlock");
        }

    }
}
